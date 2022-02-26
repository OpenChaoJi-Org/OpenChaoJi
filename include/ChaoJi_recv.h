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
#ifndef CHAOJI_RECV_H
#define CHAOJI_RECV_H

#include "ChaoJi_Trans.h"
#include "malloc.h"

/* move to ChaoJi_Trans.h file
#define SM_ACK 	(0)
#define LM_ACK 	(1)
#define LM_NACK (2)
#define LM_EndACK (3)
*/

/**
 * @brief   After RM data has been received, Application layer will trigger ChaoJi_RM_Notify to notify
 *          the transfer layer to release the memory.
 * @param   Pointer to a ChaoJi_RM_Mcb struct.
 * @retval  None.
 */
void ChaoJi_RM_Recv_Notify(struct ChaoJi_RM_Mcb *Msgcb);


/**
 * @brief   After URM data has been received, Application layer will trigger ChaoJi_RM_Notify to notify the transfer
 *          layer to release the memory.
 * @param   Pointer to a ChaoJi_URM_Mcb struct.
 * @retval  None.
 */
void ChaoJi_URM_Recv_Notify(struct ChaoJi_URM_Mcb *Msgcb);


/*Receive the data from data link layer specified by Pdu*/
/**
 * @brief   Receive the data from data link layer specified by Pdu
 *          
 * @param   Pointer to a Can_Pdu struct.
 * @retval  None.
 */
err_Cj ChaoJi_input(struct Can_Pdu * Pdu);


/**
 * @brief   Parse the received original CAN data to figure out the type of messages, which include Reliable Short Message, UnreliableShort Message,
 *          Long Message and ACK data and will raise ChaoJi_RSM_Recv,ChaoJi_URSM_Recv,ChaoJi_LM_Recv,ChaoJi_ACK_Recv respectively.
 *          Search for Reliable Short Message, Unreliable Short Message and ACK data and pass argument pcb by calling ChaoJi_RSM_Recv、ChaoJi_URSM_Recv、
 *          ChaoJi_LM_Recv、ChaoJi_ACK_Recv respectively.
 * @param   Pointer to a Can_Pdu struct.
 * @retval  None.
 */
err_Cj ChaoJi_RecvProcess(struct Can_Pdu * Pdu);


/**
 * @brief   Raise the callback function of Reliable Short Message,
 *          Acknowledge to the short message requiring a response.
 * @param   Pointer to a ChaoJi_RM_Mcb struct.
 * @retval  None.
 */
err_Cj ChaoJi_RSM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_RM_Mcb *Msgcb);


/**
 * @brief   Trigger the callback function to handle Unreliable Short Message
 *          layer to release the memory.
 * @param   Pointer to a ChaoJi_URM_Mcb struct.
 * @retval  None.
 */
err_Cj ChaoJi_URSM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_URM_Mcb *Msgcb);


/**
 * @brief   Receive the long message data, assemble data frames, flow control, segment numbering, sequence check.
 *          Deal with the connection failure, connection dropping,receiving time-out and other issues.
 *          When data is fully received, the callback function of the long message will be triggered.
 * @param   Pointer to a ChaoJi_RM_Mcb struct.
 * @retval  None.
 */
err_Cj ChaoJi_LM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_RM_Mcb *Msgcb);


/**
 * @brief   Handle all RSM_ACK messag.
 * @param   Pointer to a ChaoJi_RM_Mcb struct.
 * @retval  None.
 */
err_Cj ChaoJi_ACK_SM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_RM_Mcb *Msgcb);

/**
 * @brief   Handle all kinds of LM_ACK message,eg LM_ACK, NACK. Set flags to the sent data and put them into the data structure.
 * @param   Pointer to a ChaoJi_RM_Mcb struct.
 * @retval  None.
 */
err_Cj ChaoJi_ACK_LM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_RM_Mcb *Msgcb);

#endif // CHAOJI_RECV_H.
