/*
 *  Copyright (c) 2022 OpenChaoJi Org
 *

 *
 *
 */
#define CAN_PORT_MAX  2

#include <stdint.h>
#include <stdlib.h>
#include "ChaoJi_Connect.h"


/**
 * @brief   Callback to notify application layer after Reliable Message connection
 *          has been made.
 * @param   Pointer to a ChaoJi_RM_Mcb struct.
 * @retval  None.
 */
void ChaoJi_RM_Connect_Notify(struct ChaoJi_RM_Mcb *Msgcb)
{
    return;
}

/**
 * @brief   ChaoJi connection setup, initializ a ChaoJi RM Mcb structure, return 
 *          address of Memory.
 * @param   None.
 * @retval  None.
 */
struct ChaoJi_RM_Mcb *ChaoJi_RM_new(void)
{
    return  malloc(sizeof(struct ChaoJi_RM_Mcb));
}

/**
 * @brief   Bind the connection(with LM and RSM) with local CAN port. If the CAN 
 *          port is invalid, return error code.
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 *          CAN port.
 * @retval  error code.
 */
err_Cj ChaoJi_RM_bind(struct ChaoJi_RM_Mcb *Msgcb, uint16_t port)
{
    if(port < CAN_PORT_MAX)
  	{
  	    Msgcb->port = port;
  	    return ERR_OK;
  	}
  	return ERR_ERR;
}

/*The Client(EVSE Master) connects to the Server(EV Slave)*/
/**
 * @brief   The Client(EVSE Master) connects to the Server(EV Slave) 
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 *          CAN port.
 *          a call back function pointer
 * @retval  error code.
 */
err_Cj ChaoJi_RM_connect(struct ChaoJi_RM_Mcb *Msgcb,  uint16_t port, void (* connected)(struct ChaoJi_RM_Mcb *Msgcb))
{
    if(Msgcb->port == port)
  	{
  		  return ERR_OK;
  	}
  	return ERR_ERR;
}
/**
 * @brief   The server(Slave) listen port before receiving the connection from the Client(Master).
 *          The server(Slave) will not trigger ChaoJi_RM_accept() until it received the connection 
 *          from the client(master).it return success by default (ChaoJi based on CAN2.0B only 
 *          allow one conection at the time
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 * @retval  None.
 */
struct ChaoJi_RM_Mcb *ChaoJi_RM_listen(struct ChaoJi_RM_Mcb *Msgcb)
{
    return Msgcb;
}

/**
 * @brief   The server(Slave) responses to return success when it receives the connection 
 *          from the Client(Master), default succeed
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 *          call back function pointer 
 * @retval  None.
 */
void ChaoJi_RM_accept(struct ChaoJi_RM_Mcb *Msgcb, void (* accept)(struct ChaoJi_RM_Mcb *Msgcb))
{
    return;
}

/**
 * @brief   close the connection and free the memory.
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 * @retval  None.
 */
void ChaoJi_RM_Close(struct ChaoJi_RM_Mcb *Msgcb)
{
    free(Msgcb);
    return;
}

/**
 * @brief   obtain the mcb object by the corresponding mcb's type.
 * @param   Pointer to the message cotrol block type. 
 * @retval  the mcb object pointer
 */
struct ChaoJi_RM_Mcb *Mcb_Get(enum Mcb_type type)
{	
	uint8_t* ptr = NULL;
	
	static struct ChaoJi_RM_Mcb *send_rsm_mcb = NULL;
	static struct ChaoJi_RM_Mcb *send_lm_mcb = NULL;
	static struct ChaoJi_RM_Mcb *recv_rsm_mcb = NULL;
	static struct ChaoJi_RM_Mcb *recv_lm_mcb = NULL;

	struct ChaoJi_RM_Mcb *mcb = NULL;

	switch (type)
	{
		case SEND_RSMMCB:
			if (NULL == send_rsm_mcb){
				send_rsm_mcb = ChaoJi_RM_new();
				if (send_rsm_mcb != NULL){
					//the sending rsm key parameter initialization
					send_rsm_mcb->t1_intvl = 250;	 //重发间隔
					send_rsm_mcb->resend_times = 4;//超时重发次数
					send_rsm_mcb->rcved_ack_flag = 0xff;
					ptr = (uint8_t*)Mem_get(8);
					if (ptr != NULL){
						send_rsm_mcb->snd_buf = ptr;
					}
					else{
						//error
					}
				}
				else{
					//error
				}
			}
			mcb = send_rsm_mcb;
			break;
			
		case SEND_LMMCB:
			if (NULL == send_lm_mcb){
				send_lm_mcb = ChaoJi_RM_new();
				if (send_lm_mcb != NULL){
					//the sending lm key parameter initialization
					send_lm_mcb->t1_intvl = 8;	//LM传输中信息帧发送间隔
					send_lm_mcb->t2_intvl = 100;//控制帧超时间隔
					send_lm_mcb->t3_intvl = 10000;//长消息传输超时间隔
					send_lm_mcb->resend_times = 3;//LM超时重发次数  
					send_lm_mcb->msg_type = LM_TYPE;
					send_lm_mcb->rcved_ack_flag = 0xff;
					ptr = (uint8_t*)Mem_get(64);//long message alloc 64 bytes buffer
					if (ptr != NULL){
						send_rsm_mcb->snd_buf = ptr;
					}
					else{
						//error
					}
				}
				else{
					//error
				}
			}
			mcb = send_lm_mcb;
			break;
			
		case RECV_RSMMCB:
			if (NULL == recv_rsm_mcb){
				recv_rsm_mcb = ChaoJi_RM_new();
				if (recv_rsm_mcb != NULL){
					//key parameter initialization
					
				}
				else{
					//error
				}
			}
			mcb = recv_rsm_mcb;
			break;
			
		case RECV_LMMCB:
			if (NULL == recv_lm_mcb){
				recv_lm_mcb = ChaoJi_RM_new();
				if (recv_rsm_mcb != NULL){
					//key parameter initialization

				}
				else{
					//error
				}
			}
			mcb = recv_lm_mcb;
			break;
		default: 
			//error log
			break;
	}
	
	return mcb;
}


