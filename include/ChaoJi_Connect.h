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
#ifndef CHAOJI_CONNECT_H
#define CHAOJI_CONNECT_H

#include <stdint.h>
#include "ChaoJi_Trans.h"

/**
 * @brief   Callback to notify application layer after Reliable Message connection
 *          has been made.
 * @param   Pointer to a ChaoJi_RM_Mcb struct.
 * @retval  None.
 */
void ChaoJi_RM_Connect_Notify(struct ChaoJi_RM_Mcb *Msgcb);


/**
 * @brief   ChaoJi connection setup, initializ a ChaoJi RM Mcb structure, return 
 *          address of Memory.
 * @param   None.
 * @retval  None.
 */
struct ChaoJi_RM_Mcb *ChaoJi_RM_new(void);


/**
 * @brief   Bind the connection(with LM and RSM) with local CAN port. If the CAN 
 *          port is invalid, return error code.
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 *          CAN port.
 * @retval  error code.
 */
err_Cj ChaoJi_RM_bind(struct ChaoJi_RM_Mcb *Msgcb, uint16_t port);

/*The Client(EVSE Master) connects to the Server(EV Slave)*/
/**
 * @brief   The Client(EVSE Master) connects to the Server(EV Slave) 
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 *          CAN port.
 *          a call back function pointer
 * @retval  error code.
 */
err_Cj ChaoJi_RM_connect(struct ChaoJi_RM_Mcb *Msgcb,  uint16_t port, void (* connected)(struct ChaoJi_RM_Mcb *Msgcb));

/**
 * @brief   The server(Slave) listen port before receiving the connection from the Client(Master).
 *          The server(Slave) will not trigger ChaoJi_RM_accept() until it received the connection 
 *          from the client(master).it return success by default (ChaoJi based on CAN2.0B only 
 *          allow one conection at the time
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 * @retval  None.
 */
struct ChaoJi_RM_Mcb *ChaoJi_RM_listen(struct ChaoJi_RM_Mcb *Msgcb);


/**
 * @brief   The server(Slave) responses to return success when it receives the connection 
 *          from the Client(Master), default succeed
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 *          call back function pointer 
 * @retval  None.
 */
void ChaoJi_RM_accept(struct ChaoJi_RM_Mcb *Msgcb, void (* accept)(struct ChaoJi_RM_Mcb *Msgcb));

/**
 * @brief   close the connection and free the memory.
 * @param   Pointer to a ChaoJi_RM_Mcb struct. 
 * @retval  None.
 */
void ChaoJi_RM_Close(struct ChaoJi_RM_Mcb *Msgcb);

/**
 * @brief   obtain the mcb object by the corresponding mcb's type.
 * @param   Pointer to the message cotrol block type. 
 * @retval  the mcb object pointer
 */
struct ChaoJi_RM_Mcb *Mcb_Get(enum Mcb_type type);

#endif // CHAOJI_CONNECT_H.
