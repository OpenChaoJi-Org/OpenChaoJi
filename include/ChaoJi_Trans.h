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
#ifndef CHAOJI_TRANS_H
#define CHAOJI_TRANS_H

#include <stdint.h>

// Error Code defintions.
typedef short err_Cj;
#define ERR_OK		(0)
#define ERR_ERR		(-1)

// Message type defintions.
#define URSM_TYPE	(2)
#define LM_TYPE		(1)
#define RSM_TYPE	(0)

// CAN Source Address defintions.
#define MACID_EVCC	0x5f
#define MACID_SECC	0x25
// ChaoJi communication states defintions.
// Connect(s0) data transfer(s1) wait ack(s2) wait end(s3) connection close(s4).
enum Cj_Com_state{
//	RSM_S0	= 0,
//	RSM_S1,
//	RSM_S2,
	RSM_IDLE	= 0,
	RSM_WAIT_RESP_ACK,
	RSM_SEND_END,
	RSM_SEND_FAILURE,
//	LM_S0	= 10,
//	LM_S1,
//	LM_S2,
	LM_IDLE		= 10,
	LM_SENT_CONN,
	LM_RCVD_RESP,
	LM_ESTABLISHED,
	LM_WAIT_RESP_ACK,
	LM_WAIT_FIN_ACK,
	LM_CLOSED,
	LM_SEND_FAILURE,
	URSM_S0	= 100,
	URSM_S1,
	URSM_S2
};

// Data structure of reliable Message.
struct ChaoJi_RM_Mcb
{
	enum Cj_Com_state cj_rm_com_state;
	uint8_t	msg_type; 		// 0: RSM_TYPE, 1: LM_TYPE.
	uint8_t port;
	uint8_t pgi;
    uint8_t sm_data[8];
	uint8_t len;
	uint8_t *recv_buf;
	uint16_t tfs;			// Total fram size: ACK's total frame size.
    uint16_t rcved_tfs;		// recved Total fram size: ACK's total frame size.
	uint16_t crt_seq;		// Current sequence no: ACK's start sequence number included.
	uint16_t rst_seq;		// Request seq no.
	uint8_t *recved_flag;	// Received flag: include all ACK, NACK and ANDACK, number of pending data frames and its initial sequence number.
	uint8_t pause_flag;		// Transfer pause.
	uint8_t trans_fin_flag;	// Transfer finish flag.
	err_Cj (* recv)(struct ChaoJi_RM_Mcb *rm_Mcb); // Call back function to notify appliacation layer recv have been made.
	uint8_t* snd_buf;		// Send the data buffer..
    uint32_t snd_len;		// Send the byte size of data.
    enum Cj_Com_state snd_state;// Send states.
    uint32_t snd_total_frames;
	uint32_t snd_frame_cnt;
	uint32_t snd_currSN; 	// Send the current serial number.
	uint32_t snd_time_out; 	// Send the timeout.
	uint32_t snd_max_timer; // Send the maxium timer round.
	err_Cj (* sent)(struct ChaoJi_RM_Mcb *rm_Mcb); // Call back func to notify application layer.
	uint32_t t1_intvl;	//RSM ACK massage timeout and resend interval time setting.or LMS_T1 is LM Send information data interval time setting.
	int32_t t1_cnt;	
	uint32_t t2_intvl;// LMS_T2 LM control message or data message timeout setting.
	int32_t t2_cnt;
	uint32_t t3_intvl;// LMS_T3 LM Send the maxium timeout setting.
	int32_t t3_cnt;
	uint8_t resend_times;
	uint8_t resend_cnt;
};

// Data structure of unreliable Message
struct ChaoJi_URM_Mcb
{
	enum Cj_Com_state cj_urm_com_state;
	uint8_t port;
	uint8_t pgi;
    uint8_t sm_data[8];
	uint8_t len;
	uint8_t *recv_buf;
	uint16_t tfs;			// Total fram size.
	uint16_t crt_seq;		// Current sequence no.
	uint16_t rst_seq;		// Request seq no.
	uint8_t *recved_flag;	// Received flag.
	uint8_t pause_flag;		// Transfer pause flag.
	uint8_t trans_fin_flag;	// Transter finish flag.
	err_Cj (* recv)(struct ChaoJi_URM_Mcb *urm_Mcb); // Call back function to notify application layer recv have been made.
	uint8_t* snd_buf;		// Send procedure data buffer.
	uint32_t snd_len;		// Send the byte size of data.
	uint32_t snd_state;		// Send the status.
	uint32_t snd_currSN;	// Send the current serial number.
	uint32_t snd_time_out; 	// Send the timeout.
	uint32_t snd_max_timer; // Send the maxium timer round.
	err_Cj (* sent)(struct ChaoJi_URM_Mcb *urm_Mcb); // Call back func to notify application layer.
};

// CAN ID
typedef union
{
	uint32_t uint32_tWord;
	struct
	{
		uint32_t sa		:8; // Source address.
        uint32_t ps		:8; // Destination address.
		uint32_t pf		:8; // PDU type.
		uint32_t dp		:1; // Data Page.
		uint32_t edp	:1; // Extended data page.
		uint32_t p		:3; // Priority.
		uint32_t rsvd	:3; // Reserved.
	} bit;
} CanId_t;

// CAN PDU defination.
struct Can_Pdu
{
	CanId_t	can_id;
    uint8_t	can_dlc;
    uint8_t	data[8];
};

#endif // CHAOJI_TRANS_H.
