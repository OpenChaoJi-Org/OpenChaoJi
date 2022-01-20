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


