#include "ChaoJi_recv.h"
#include "ChaoJi_Trans.h"

struct ChaoJi_RM_Mcb lm_Mcb;//temp
struct ChaoJi_RM_Mcb rsm_Mcb;//temp
struct ChaoJi_URM_Mcb ursm_Mcb;//temp


void ChaoJi_RM_Recv_Notify(struct ChaoJi_RM_Mcb *Msgcb)
{

    if(Msgcb->recv_buf!=NULL)
    {
        free(Msgcb->recv_buf);
        Msgcb->recv_buf=NULL;
    }


}

void ChaoJi_URM_Recv_Notify(struct ChaoJi_URM_Mcb *Msgcb)
{
//if need
//    if(Msgcb->recv_buf!=NULL)
//    {
//        free(Msgcb->recv_buf);
//        Msgcb->recv_buf=NULL;
//    }
}

err_Cj ChaoJi_input(struct Can_Pdu * Pdu)
{
    uint8_t sa=0xf4;
    uint8_t ps=0x56;//temp need change to config file

    if((Pdu->can_id.bit.ps==ps)&&(Pdu->can_id.bit.ps==sa))//Judge legal address
    {
       ChaoJi_RecvProcess(Pdu);
    }
}

err_Cj ChaoJi_RecvProcess(struct Can_Pdu * Pdu)
{
    uint8_t pf=Pdu->can_id.bit.pf;//get pf of pdu
    uint8_t ack_type=0;

    if(pf==0x01)//LM
    {
        ChaoJi_LM_Recv(Pdu,&lm_Mcb);
    }
    else if(pf==0x02)//SM_RM
    {
        ChaoJi_RSM_Recv(Pdu,&rsm_Mcb);
        //ChaoJi_URSM_Recv(Pdu,&rsm_Mcb);
    }
    else if(pf==0x03)//SM_URM
    {
        ChaoJi_URSM_Recv(Pdu,&ursm_Mcb);
    }
    else if(pf==0x04)//ACK info
    {
        ack_type=Pdu->data[0];
        if(ack_type==SM_ACK)
        {
            ChaoJi_ACK_SM_Recv(Pdu,&rsm_Mcb);//RSM ACK
        }
        else if(ack_type==LM_ACK||ack_type==LM_NACK||ack_type==LM_EndACK)
        {
            ChaoJi_ACK_LM_Recv(Pdu,&lm_Mcb);//LM ACK
        }
        else
        {
            //err
        }

    }
    else
    {
        //err
    }
}

err_Cj ChaoJi_RSM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_RM_Mcb *Msgcb)
{

    uint8_t i=0;
    Msgcb->msg_type=RSM_TYPE;
    for(i=0;i<7;i++)
    {
        Msgcb->sm_data[i]=Pdu->data[i];

    }

    Msgcb->recv(Msgcb);

}

err_Cj ChaoJi_URSM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_URM_Mcb *Msgcb)
{
    uint8_t i=0;

    for(i=0;i<7;i++)
    {
        Msgcb->sm_data[i]=Pdu->data[i];

    }

    Msgcb->recv(Msgcb);
}

err_Cj ChaoJi_LM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_RM_Mcb *Msgcb)
{
    uint8_t fram_idx=Pdu->data[0];

    if(fram_idx==0)
    {
        uint8_t *p=NULL;
        uint8_t *p_flag=NULL;
        uint8_t redundancy=0;
        uint8_t need_start_no=0;
        uint8_t need_total_no=0;

        uint8_t state=Msgcb->cj_rm_com_state;

        //some para correct check ?
        Msgcb->msg_type=LM_TYPE;
        Msgcb->tfs=Pdu->data[1];// Total fram size: ACK's total frame size.
        Msgcb->len=(Pdu->data[2]|(Pdu->data[3]<<8));//data len in byte
        Msgcb->rcved_tfs=0;

        redundancy=Msgcb->len%7;


        if(redundancy>0)
        {
            if(Msgcb->recv_buf!=NULL)
            {
                free(Msgcb->recv_buf);
                Msgcb->recv_buf=NULL;
            }
            p = (uint8_t *)malloc(Msgcb->len+redundancy);//add redundancy
        }
        else if(redundancy==0)
        {
            p = (uint8_t *)malloc(Msgcb->len);
        }


        if(p!=NULL)
        {
           Msgcb->recv_buf=p;
        }
        else
        {
            return;
            //err
        }



        if(Msgcb->recved_flag!=NULL)
        {
            free(Msgcb->recved_flag);
            Msgcb->recved_flag=NULL;
        }
        p_flag = (uint8_t *)malloc(Msgcb->tfs);//tfs

        if(p_flag!=NULL)
        {
           Msgcb->recved_flag=p_flag;
        }
        else
        {
            return;
            //err
        }



        if(state==RSM_S0||state==LM_S0)//?
        {
            Msgcb->cj_rm_com_state=LM_S1;
        }
        else
        {
            //err not idle
            return;
        }

        need_start_no=1;//can change
        need_total_no=Msgcb->tfs;
        Msgcb->rst_seq=(need_total_no<<8)|need_start_no;
        //send ACK 1, Msgcb->tfs

    }
    else if(fram_idx>0)
    {
        uint8_t i=0;
        uint8_t byte_pos=(fram_idx-1)*7;
        //memcpy();?
        if(Msgcb->recv_buf!=NULL&&(Msgcb->recved_flag[fram_idx-1]!=1))//&&fram_idx<Msgcb->tfs
        {
            if((byte_pos+8)<=Msgcb->len)
            {


                //Msgcb->cj_rm_com_state=LM_S1;
                for(i=0;i<7;i++)
                {
                    Msgcb->recv_buf[byte_pos+i]=Pdu->data[i+1];

                }
                Msgcb->recved_flag[fram_idx-1]=1;
                Msgcb->crt_seq=fram_idx;
                Msgcb->rcved_tfs++;

                if(Msgcb->rcved_tfs>=Msgcb->tfs)
                {

                    Msgcb->cj_rm_com_state=LM_S0;

                    Msgcb->recv(Msgcb);
                    //send end of ack rcved_tfs| Msgcb->len<<8?

                }


                //send ack start 1 total package Msgcb->tfs
            }
            else
            {
                //err
            }
        }
        else
        {
            //err
        }


    }
    else
    {
        //log err
        return;
    }
}


err_Cj ChaoJi_ACK_SM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_RM_Mcb *Msgcb)
{
     //notify send
	 
     ChaoJi_ACK_Notify(Pdu,Msgcb);
}

err_Cj ChaoJi_ACK_LM_Recv(struct Can_Pdu * Pdu,struct ChaoJi_RM_Mcb *Msgcb)
{
    //notify send
    ChaoJi_ACK_Notify(Pdu,Msgcb);
}
