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
#include "ChaoJi_Trans.h"

/**
* @brief   After Reliable Message send all procedure is done call back to notify
*          application layer.
* @param   Msgcb Pointer to a ChaoJi_RM_Mcb struct.
* @retval  None.
*/
void ChaoJi_RM_Send_Notify(struct ChaoJi_RM_Mcb *Msgcb);

/**
* @brief   After unreliable Reliable Message send all procedure is done call back 
*          to notify application layer.
* @param   Msgcb Pointer to a ChaoJi_URM_Mcb struct body.
* @retval  None.
*/
void ChaoJi_URM_Send_Notify(struct ChaoJi_URM_Mcb *Msgcb);

/**
 * @brief	when Received ack of the rsm or lm, call back this fuctoin ,the RSM_SendProcess or LM_SendProcessof will be triggered.
 * @param	Pdu Pointer to a input Can_Pdu data.
 * @param	Msgcb Pointer to a ChaoJi_RM_Mcb struct.
 * @retval	None.
 */
err_Cj ChaoJi_ACK_Notify(struct Can_Pdu *Pdu,struct ChaoJi_RM_Mcb *Msgcb);


/**
* @brief   Reliable message send, Deal with application data buffer and request Msgcb
*          according to data buffer length then call ChaoJi_RM_SendProcess.
* @param   Msgcb Pointer to a ChaoJi_RM_Mcb struct,for the reliable message to send data.
* @param   data Pointer to the data to be sending.
* @param   length data length in bytes.
* @param   err Pointer to a error,unused.
* @retval  ERR_OK If the transmit data is written to the buffer.
*/
err_Cj ChaoJi_RM_write(struct ChaoJi_RM_Mcb *Msgcb, uint8_t* data, uint32_t length);

/**
* @brief   unreliable message send, Deal with application data buffer and Convert the application 
*		   data format to Can_Pdu frame ,then call ChaoJi_Output.
* @param   Msgcb Pointer to a ChaoJi_URM_Mcb struct,for the unreliable message to send data.
* @param   data Pointer to the data to be sending.
* @param   length Pointer to data length in bytes.
* @param   err Pointer to a error,unuse.
* @retval  ERR_OK If there is an error in sending.
*/
<<<<<<< HEAD
<<<<<<< HEAD
err_Cj ChaoJi_URSM_send(struct ChaoJi_Urm_Mcb *Msgcb, uint8_t* data, uint32_t length);
=======
err_Cj ChaoJi_URSM_send(struct ChaoJi_Urm_Mcb *Msgcb, uint8_t* data, uint32_t length, err_Cj *err);
>>>>>>> 891a7b0d69f7d552151d0d86204c7ce699018c5f
=======
err_Cj ChaoJi_URSM_send(struct ChaoJi_URM_Mcb *Msgcb, uint8_t* data, uint32_t length);
>>>>>>> 4fd69b8e18d8bdbb76f2c4cc0eb1691cc8acd07a

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
//Cj_Com_state ChaoJi_RM_SendProcess(struct ChaoJi_RM_Mcb *Msgcb, struct Can_Pdu *pdu);
//Cj_Com_state ChaoJi_RM_SendProcess(struct ChaoJi_RM_Mcb *Msgcb);


/**
* @brief   Deal with data link layer to make real traffic by using CAN PDU，
*			usage：After ChaoJi_URSM_write, ChaoJi_RM_TxProcess or rferenced by receiv procedure
* @param   pdu Pointer to the output can link protocol data unit,the pdu will be sent.
* @retval  ERR_OK If the data has been sent ,else ERR_ERR.
*/
err_Cj ChaoJi_Output(struct Can_Pdu *pdu);

/**
<<<<<<< HEAD
<<<<<<< HEAD
* @brief   Deal with data link layer to make real traffic by using CAN PDU，
*			usage：After ChaoJi_URSM_write, ChaoJi_RM_TxProcess or rferenced by receiv procedure
=======
* @brief   This function is called back in 1 millisecond period, which is used to send the timer timing，
>>>>>>> 4fd69b8e18d8bdbb76f2c4cc0eb1691cc8acd07a
* @param   None.
* @retval  None.
*/
void ChaoJi_Send_1msTimeTick(void);


/**
=======
>>>>>>> 891a7b0d69f7d552151d0d86204c7ce699018c5f
* @brief   Polling method to get the sending procedure status for application layer.
* @param   Msgcb Pointer to a ChaoJi_RM_Mcb struct body.
* @retval  reliable message send state,define in Cj_Com_state
*/
enum Cj_Com_state ChaoJi_RM_GetSendState(struct ChaoJi_RM_Mcb *Msgcb);

<<<<<<< HEAD

=======
>>>>>>> 891a7b0d69f7d552151d0d86204c7ce699018c5f
#endif // CHAOJI_SEND_H.
