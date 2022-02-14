/*
 *  Copyright (c) 2022 OpenChaoJi Org
 *
 *  Use of this source code is governed by an MIT-style
 *  license that can be found in the LICENSE file or at
 *  https://opensource.org/licenses/MIT.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the "Software"), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 *  the Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 */
#ifndef CHAOJI_SEND_H
#define CHAOJI_SEND_H

#include <stdint.h>
//#include "include/ChaoJi_Trans.h"
#include "ChaoJi_Trans.h"


/**
* @brief   After Reliable Message send all procedure is done call back to notify
*          application layer.
* @param   Msgcb Pointer to a ChaoJi_RM_Mcb struct.
* @retval  None.
*/
void ChaoJi_RM_Send_Notify(struct ChaoJi_RM_Mcb *Msgcb)
{
	
}

/**
* @brief   After unreliable Reliable Message send all procedure is done call back 
*          to notify application layer.
* @param   Msgcb Pointer to a ChaoJi_URM_Mcb struct body.
* @retval  None.
*/
void ChaoJi_URM_Send_Notify(struct ChaoJi_URM_Mcb *Msgcb)
{
	
}

/**
* @brief   Reliable message send, Deal with application data buffer and request Msgcb 
*          according to data buffer length then call ChaoJi_RM_SendProcess.
* @param   Msgcb Pointer to a ChaoJi_RM_Mcb struct,for the reliable message to send data.
* @param   data Pointer to the data to be sending.
* @param   length data length in bytes.
* @param   err Pointer to a error,unused.
* @retval  ERR_OK If the transmit data is written to the buffer.
*/
err_Cj ChaoJi_RM_write(struct ChaoJi_RM_Mcb *Msgcb, uint8_t* data, uint32_t length, err_Cj *err)
{
	err_Cj error = ERR_OK;

	if (Msgcb == NULL){
		return ERR_ERR;
	}

	if (length == 0) {
		return ERR_ERR;
	}

	Msgcb->snd_buf = (uint8_t*)ChaoJi_pbuf_alloc(length);
	if (Msgcb->snd_buf == NULL){
		return ERR_ERR;
	}
	//发送数据缓存
	memcpy(Msgcb->snd_buf,data,length);

	if (length <= 8)//短消息处理
	{
		Msgcb->msg_type = RSM_TYPE;
		Msgcb->snd_state = RSM_IDLE;
		Msgcb->snd_len = length;
		error = RSM_SentData(Msgcb);
		Msgcb->t1_cnt = Msgcb->t1_intvl;
	} 
	else //长消息处理
	{
		Msgcb->msg_type = LM_TYPE;
		Msgcb->snd_len = length;
		Msgcb->snd_total_frames = length/7+1;
		Msgcb->snd_frame_cnt = 0;

		//发送LM(0),开启 LMS_T2,LMS_T3,进入S0
		error = LM_DoConnect(Msgcb);
		Msgcb->t1_cnt = -1;
		Msgcb->t2_cnt = Msgcb->t2_intvl;
		Msgcb->t3_cnt = Msgcb->t3_intvl;
		Msgcb->snd_state = LM_SENT_CONN;
	}

	return error;
}

//rm_write_checks


/**
* @brief   unreliable message send, Deal with application data buffer and Convert the application 
*		   data format to Can_Pdu frame ,then call ChaoJi_Output.
* @param   Msgcb Pointer to a ChaoJi_URM_Mcb struct,for the unreliable message to send data.
* @param   data Pointer to the data to be sending.
* @param   length Pointer to data length in bytes.
* @param   pdu Pointer to the can link protocol data unit frame.
* @retval  ERR_OK If the transmit data is written to the buffer.
*/
/*
err_Cj ChaoJi_URSM_write(struct ChaoJi_Urm_Mcb *Msgcb, uint8_t* data, uint32_t length, struct Can_Pdu *pdu)
{
	err_Cj error;


	return error;
}*/

err_Cj ChaoJi_URSM_send(struct ChaoJi_Urm_Mcb *Msgcb, uint8_t* data, uint32_t length)
{
	struct Can_Pdu pdu;
	err_Cj error;
	u8_t i;

#if EVCC_NODE
	pdu->can_id.bit.sa = MACID_EVCC;
	pdu->can_id.bit.ps = MACID_SECC;
#elif SECC_NODE
	pdu->can_id.bit.sa = MACID_SECC;
	pdu->can_id.bit.ps = MACID_EVCC;
#endif
	pdu->can_id.bit.pf = 0x36;
	pdu->can_id.bit.dp = 0;
	pdu->can_id.bit.edp = 0;
	pdu->can_id.bit.p = 6;
	pdu->can_id.bit.rsvd = 0;
	
	pdu->can_dlc = 8;

	for (i = 0;i < 8;i++){
		if (i < length){
			pdu->data[i] = data[i];
		}
		else{
			pdu->data[i] = 0xff;
		}
	}

	error = ChaoJi_Output(&pdu);
	return error;
}

//1ms周期运行任务，主要维护可靠消息的周期发送定时器、超时定时器，定时时间到调用对应状态机执行
void ChaoJi_RSM_1msTimeTick(void)
{
	struct ChaoJi_RM_Mcb *Msgcb;

	if (Msgcb->t1_cnt > 0){
		Msgcb->t1_cnt--;
		if (Msgcb->t1_cnt == 0){
			ChaoJi_RM_SendProcess(Msgcb);
		}
	}

	if (Msgcb->t2_cnt > 0){
		Msgcb->t2_cnt--;
		if (Msgcb->t2_cnt == 0){
			ChaoJi_RM_SendProcess(Msgcb);
		}
	}

	if (Msgcb->t3_cnt > 0){
		Msgcb->t3_cnt--;
		if (Msgcb->t3_cnt == 0){
			ChaoJi_RM_SendProcess(Msgcb);
		}
	}
}


/**
* @brief   Relaible message send state machine, connection established, sending data, pause, 
*		   ending states transient, after execute finished call back ChaoJi_RM_Send_Notify to notify application layer.
*		   the system periodic call ChaoJi_RM_TxProcess(lm_Msgcb)、ChaoJi_RM_TxProcess(rsm_Msgcb).
*		   and call back by RSM ACK, LM ACK, NACK to update the Cj_state
* @param   Msgcb Pointer to a ChaoJi_RM_Mcb struct,for the reliable message to send data.
* @param   data Pointer to the data to be sending.
* @param   length Pointer to data length in bytes.
* @param   pdu Pointer to the output can link protocol data unit,the pdu will be sent.
* @retval  reliable message send state,define in Cj_Com_state
*/
//enum Cj_Com_state ChaoJi_RM_SendProcess(struct ChaoJi_RM_Mcb *Msgcb, struct Can_Pdu *pdu)
enum Cj_Com_state ChaoJi_RM_SendProcess(struct ChaoJi_RM_Mcb *Msgcb)
{
	enum Cj_Com_state state;
	u8_t i;
	err_Cj error;
	
	//要发送短消息处理
	if (Msgcb->msg_type == RSM_TYPE)
	{
		switch (Msgcb->snd_state)
		{
			case RSM_WAIT_RESP_ACK://发送请求
				if (Msgcb->recved_flag == URM_ACK){
					Msgcb->t1_cnt = -1;
					Msgcb->resend_cnt = 0;
					Msgcb->snd_state = RSM_SEND_END;
				}
				
				if (Msgcb->t1_cnt == 0){
					//完成信息发送后，等待接收方URM_ACK应答确认,T2时间超时后重发本次传输的最后一帧，连续出现4次超时后
					//放弃连接，本次发送失败
					Msgcb->resend_cnt++;
					if (Msgcb->resend_cnt <= Msgcb->resend_times){
						error = RSM_SentData(Msgcb);
					}
					else{
						Msgcb->t1_cnt = -1;
						Msgcb->resend_cnt = 0;
						Msgcb->snd_state = RSM_SEND_FAILURE;
					}
				}
				break;
			default:	break;
		}
	}

	//要发送长消息处理
	if (Msgcb->msg_type == LM_TYPE)
	{
		switch (Msgcb->snd_state)
		{
			case LM_IDLE://发送请求
				
				break;
				
			case LM_SENT_CONN://发送连接
				if (Msgcb->recved_flag == LM_ACK){
					//根据应答发送 LM(n), send_cnt 置 1. 开启LMS T1,关闭 LMS_T2, 进入S1
					error = LM_SentData(Msgcb,Msgcb->crt_seq);
					Msgcb->snd_frame_cnt = 1;
					Msgcb->t1_cnt = Msgcb->t1_intvl;
					Msgcb->t2_cnt = -1;
					Msgcb->snd_state = LM_ESTABLISHED;
				}
				
				if (Msgcb->recved_flag == NACK){
					LM_CloseConnect(Msgcb);
				}
				
				if (Msgcb->t2_cnt == 0)
				{	
					Msgcb->resend_cnt++;
					//3次重发
					if (Msgcb->resend_cnt <= Msgcb->resend_times){
						//发	送LM(0), 重置LMS_T2,进入S0
						LM_DoConnect(Msgcb);
						Msgcb->t2_cnt = Msgcb->t2_intvl;
					}
					else{
						//3次重发超时，结束本次发送
						LM_SentNAck(Msgcb);
						LM_CloseConnect(Msgcb);
					}
				}
				
				if (Msgcb->t3_cnt == 0)
				{
					//长消息传输超时，结束本次发送
					LM_SentNAck(Msgcb);
					LM_CloseConnect(Msgcb);
				}
				break;
				
			case LM_ESTABLISHED:	//数据传输状态
				if (Msgcb->recved_flag == LM_ACK){
					//保存k,根据应答发送LM(n), send_cnt置1,开启LMS_T1,关LMS_T2,进入S1
					error = LM_SentData(Msgcb,Msgcb->crt_seq);
					Msgcb->snd_frame_cnt = 1;
					Msgcb->t1_cnt = Msgcb->t1_intvl;
					Msgcb->t2_cnt = -1;
				}
				
				if (Msgcb->recved_flag == NACK){
					LM_CloseConnect(Msgcb);
				}

				if (Msgcb->t1_cnt == 0)
				{
					//期望发送为长消息最后一帧，进入等待结束确认状态
					if ((Msgcb->snd_frame_cnt + Msgcb->crt_seq - 1) == Msgcb->snd_total_frames){
						Msgcb->snd_frame_cnt++;
						Msgcb->t1_cnt = -1;
						Msgcb->t1_cnt = Msgcb->t2_intvl;
						Msgcb->snd_state = LM_WAIT_FIN_ACK;
						Msgcb->resend_cnt = 0;
					}
					//期望发送帧为接收方请求的最后一帧，且非长消息最后一帧,进入等待应答确认状态
					else if (Msgcb->snd_frame_cnt == Msgcb->tfs){
						Msgcb->snd_frame_cnt++;
						Msgcb->t1_cnt = -1;
						Msgcb->t1_cnt = Msgcb->t2_intvl;
						Msgcb->snd_state = LM_WAIT_RESP_ACK;
						Msgcb->resend_cnt = 0;
					}
					else{
						//未结束，继续发送
						error = LM_SentData(Msgcb,Msgcb->crt_seq + Msgcb->snd_frame_cnt);
						Msgcb->snd_frame_cnt++;
						Msgcb->t1_cnt = Msgcb->t1_intvl;
					}
				}
				
				if (Msgcb->t3_cnt == 0)
				{
					//长消息传输超时，结束本次发送
					LM_SentNAck(Msgcb);
					LM_CloseConnect(Msgcb);
				}
				break;

			case LM_WAIT_RESP_ACK:	//等待应答确认状态
				if (Msgcb->recved_flag == LM_ACK){
					//根据应答调整LM的帧序号为n, send_cnt 置 1, 开启LMS_T1,关闭LMS_T2, 进入S1
					error = LM_SentData(Msgcb,Msgcb,Msgcb->crt_seq);
					Msgcb->snd_frame_cnt = 1;
					Msgcb->t1_cnt = Msgcb->t1_intvl;
					Msgcb->t2_cnt = -1;
					Msgcb->snd_state = LM_ESTABLISHED;
				}

				if (Msgcb->recved_flag == LM_NACK){
					LM_CloseConnect(Msgcb);
				}

				if (Msgcb->t2_cnt == 0){
					//完成信息发送后，等待接收方LM_ACK,T2时间超时后重发本次传输的最后一帧，连续出现3次超时后
					//发送LM_NACK放弃连接
					Msgcb->resend_cnt++;
					if (Msgcb->resend_cnt <= Msgcb->resend_times){
						LM_SentData(Msgcb,Msgcb->crt_seq + Msgcb->tfs - 1);//TBD 发送LM(n+k-1)
						Msgcb->t2_cnt = Msgcb->t2_intvl;
					}
					else{
						LM_SentNAck(Msgcb);
						LM_CloseConnect(Msgcb);
					}
				}

				if (Msgcb->t3_cnt == 0)
				{
					LM_SentNAck(Msgcb);
					LM_CloseConnect(Msgcb);
				}

				break;

			case LM_WAIT_FIN_ACK:
				if (Msgcb->recved_flag == LM_ACK){
					//根据应答调整LM的帧序号为n, send_cnt 置 1, 开启LMS_T1,关闭LMS_T2, 进入S1
					error = LM_SentData(Msgcb,Msgcb,Msgcb->crt_seq);
					Msgcb->snd_frame_cnt = 1;
					Msgcb->t1_cnt = Msgcb->t1_intvl;
					Msgcb->t2_cnt = -1;
					Msgcb->snd_state = LM_ESTABLISHED;
				}

				if (Msgcb->recved_flag == LM_NACK || Msgcb->recved_flag == LM_END_OF_ACK){
					LM_CloseConnect(Msgcb);
				}
				
				if (Msgcb->t2_cnt == 0){
					//完成信息发送后，等待接收方LM_ACK或LM_EndofACK,T2时间超时后重发本次传输的最后一帧，连续出现3次超时后
					//发送LM_NACK放弃连接
					if (Msgcb->resend_cnt <= Msgcb->resend_times){
						LM_SentData(Msgcb,Msgcb->snd_total_frames);
						Msgcb->t2_cnt = Msgcb->t2_intvl;
					}
					else{
						LM_SentNAck(Msgcb);
						LM_CloseConnect(Msgcb);
					}
				}

				if (Msgcb->t3_cnt == 0)
				{
					LM_SentNAck(Msgcb);
					LM_CloseConnect(Msgcb);
				}
				break;

			default: break;
		}
	}
	return state;
}

//可靠短消息信息发送
err_Cj RSM_SentData(struct ChaoJi_RM_Mcb *Msgcb)
{
	struct Can_Pdu pdu;
	err_Cj error;
	u8_t i;

#if EVCC_NODE
	pdu->can_id.bit.sa = MACID_EVCC;
	pdu->can_id.bit.ps = MACID_SECC;
#elif SECC_NODE
	pdu->can_id.bit.sa = MACID_SECC;
	pdu->can_id.bit.ps = MACID_EVCC;
#endif
	pdu->can_id.bit.pf = 0x35;
	pdu->can_id.bit.dp = 0;
	pdu->can_id.bit.edp = 0;
	pdu->can_id.bit.p = 3;
	pdu->can_id.bit.rsvd = 0;
	
	pdu->can_dlc = Msgcb->snd_len;

	for (i = 0;i < 8;i++){
		if (i < Msgcb->snd_len){
			pdu->data[i] = Msgcb->snd_buf[i];
		}
		else{
			pdu->data[i] = 0xff;
		}
	}

	error = ChaoJi_Output(&pdu);
	return error;
}

//长消息发送连接
err_Cj LM_DoConnect(struct ChaoJi_RM_Mcb *Msgcb)
{
	struct Can_Pdu pdu;
	err_Cj error;

	#if EVCC_NODE
		pdu->can_id.bit.sa = MACID_EVCC;
		pdu->can_id.bit.ps = MACID_SECC;
	#elif SECC_NODE
		pdu->can_id.bit.sa = MACID_SECC;
		pdu->can_id.bit.ps = MACID_EVCC;
	#endif
	pdu.can_id.bit.pf = 0x34;
	pdu.can_id.bit.dp = 0;
	pdu.can_id.bit.edp = 0;
	pdu.can_id.bit.p = 6;
	pdu.can_id.bit.rsvd = 0;
	pdu.can_dlc = 8;
	pdu.data[0] = 0;
	pdu.data[1] = Msgcb->snd_total_frames;	//发送总帧数
	pdu.data[2] = Msgcb->snd_len & 0x00ff;	//总字节数
	pdu.data[3] = Msgcb->snd_len>>8;
	pdu.data[4] = 0xff;
	pdu.data[5] = 0xff;
	pdu.data[6] = 0xff;
	pdu.data[7] = 0xff;
	
	error = ChaoJi_Output(&pdu);

	return error;
}

//长消息关闭连接
err_Cj LM_CloseConnect(struct ChaoJi_RM_Mcb *Msgcb)
{
	Msgcb->t1_cnt = -1;
	Msgcb->t2_cnt = -1;
	Msgcb->t3_cnt = -1;
	Msgcb->snd_frame_cnt = 0;
	Msgcb->snd_state = LM_IDLE;
	Msgcb->resend_cnt = 0;
	
	ChaoJi_pbuf_free(Msgcb->snd_buf);
}

//长消息信息数据发送
err_Cj LM_SentData(struct ChaoJi_RM_Mcb *Msgcb,uint8_t sndFrameNo)
{
	struct Can_Pdu pdu;
	err_Cj error;
	u8_t i;
	u16_t offset;

	#if EVCC_NODE
		pdu->can_id.bit.sa = MACID_EVCC;
		pdu->can_id.bit.ps = MACID_SECC;
	#elif SECC_NODE
		pdu->can_id.bit.sa = MACID_SECC;
		pdu->can_id.bit.ps = MACID_EVCC;
	#endif
	pdu->can_id.bit.pf = 0x34;
	pdu->can_id.bit.dp = 0;
	pdu->can_id.bit.edp = 0;
	pdu->can_id.bit.p = 6;
	pdu->can_id.bit.rsvd = 0;
	pdu->can_dlc = 8;
	pdu->data[0] = sndFrameNo;

	offset = (sndFrameNo-1)*7;//tbd ,Msgcb->crt_seq
	
	for (i = 0;i < 7;i++)
	{
		if ((i+offset) < Msgcb->snd_len)
		{
			pdu->data[1+i] = Msgcb->snd_buf[i+offset];
		}
		else
		{
			pdu->data[1+i] = 0xff;
		}
	}
	error = ChaoJi_Output(&pdu);

	return error;
}

//长消息NACK发送
err_Cj LM_SentNAck(struct ChaoJi_RM_Mcb *Msgcb)
{
	struct Can_Pdu pdu;
	err_Cj error;

	#if EVCC_NODE
		pdu->can_id.bit.sa = MACID_EVCC;
		pdu->can_id.bit.ps = MACID_SECC;
	#elif SECC_NODE
		pdu->can_id.bit.sa = MACID_SECC;
		pdu->can_id.bit.ps = MACID_EVCC;
	#endif
	pdu.can_id.bit.pf = 0x37;
	pdu.can_id.bit.dp = 0;
	pdu.can_id.bit.edp = 0;
	pdu.can_id.bit.p = 3;
	pdu.can_id.bit.rsvd = 0;
	pdu.can_dlc = 8;
	pdu.data[0] = 2;
	pdu.data[1] = 0xff;
	pdu.data[2] = 0xff;
	pdu.data[3] = 0xff;
	pdu.data[4] = 0xff;
	pdu.data[5] = 0xff;
	pdu.data[6] = 0xff;
	pdu.data[7] = 0xff;
	error = ChaoJi_Output(&pdu);

	return error;
}

/**
* @brief   Deal with data link layer to make real traffic by using CAN PDU，
*			usage：After ChaoJi_URSM_write, ChaoJi_RM_TxProcess or rferenced by receiv procedure
* @param   pdu Pointer to the output can link protocol data unit,the pdu will be sent.
* @retval  ERR_OK If the data has been sent ,else ERR_ERR.
*/
err_Cj ChaoJi_Output(struct Can_Pdu *pdu)
{
	err_Cj err;

	return err;
}

/**
* @brief   Polling method to get the sending procedure status for application layer.
* @param   Msgcb Pointer to a ChaoJi_RM_Mcb struct body.
* @retval  reliable message send state,define in Cj_Com_state
*/
enum Cj_Com_state ChaoJi_RM_GetSendState(struct ChaoJi_RM_Mcb *Msgcb)
{
	enum Cj_Com_state state;

	return state;
}

//内存分配
void* ChaoJi_pbuf_alloc(uint16_t length)
{
	//返回一个值
	return ;
}

//内存释放
uint8_t ChaoJi_pbuf_free(void* pbuff)
{
	
}

/*some example for how to use send*/
/**/
void example(void)
{
	//-----------------Init--------------------------------------------------
	struct ChaoJi_RM_Mcb lm_Mcb = *ChaoJi_RM_new(void);
	//...
	struct ChaoJi_RM_Mcb rsm_Mcb = *ChaoJi_RM_new(void);
	//...
	struct ChaoJi_URSM_Mcb ursm_Mcb = *ChaoJi_URSM_new(void);
	//...
	err_Cj errVal = ERR_OK;

	// Send
/*	uint8_t data1[] = {1, 2, 3, 4, 5,6,7,8,9};
	ChaoJi_RM_write(lm_Mcb, data1, sizeof(data1), &errVal);
	ChaoJi_RM_SendProcess(lm_Mcb, pdu);
	ChaoJi_Output(pdu);

	uint8_t data2[] = {1, 2, 3, 4, 5};
	ChaoJi_RM_write(rsm_Mcb, data2, sizeof(data2), &errVal);
	ChaoJi_RM_SendProcess(rsm_Mcb, pdu);
	ChaoJi_Output(pdu);

	uint8_t data3[] = {1, 2, 3, 4, 5,6,7};
	ChaoJi_URSM_write(ursm_Mcb, data3, sizeof(data3), pdu);
	ChaoJi_Output(pdu);
*/

	// Send
	uint8_t data1[] = {1, 2, 3, 4, 5,6,7,8,9};
	lm_Mcb->t1_intvl = 8;
	lm_Mcb->t2_intvl = 100;
	lm_Mcb->t3_intvl = 10000;
	lm_Mcb->resend_times = 3;//超时重发次数
	ChaoJi_RM_write(lm_Mcb, data1, sizeof(data1), &errVal);

	uint8_t data2[] = {1, 2, 3, 4, 5};
	lm_Mcb->t1_intvl = 250;	 //重发间隔
	lm_Mcb->resend_times = 4;//超时重发次数
	ChaoJi_RM_write(rsm_Mcb, data2, sizeof(data2), &errVal);
	
	uint8_t data3[] = {1, 2, 3, 4, 5,6,7};
	ChaoJi_URSM_send(ursm_Mcb, data3, sizeof(data3),);
};

#endif // CHAOJI_SEND_H.
