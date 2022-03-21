#include"ChaoJi_Trans.h"

int main()
{
  	//-----------------Init--------------------------------------------------
	struct ChaoJi_RM_Mcb* lm_Mcb = Mcb_Get(SEND_LMMCB);
	//...
	struct ChaoJi_RM_Mcb* rsm_Mcb = Mcb_Get(SEND_RSMMCB);
	//...

	// Send
	uint8_t data1[] = {1, 2, 3, 4, 5,6,7,8,9};
	ChaoJi_RM_write(lm_Mcb, data1, sizeof(data1));

	uint8_t data2[] = {1, 2, 3, 4, 5};
	ChaoJi_RM_write(rsm_Mcb, data2, sizeof(data2));
	
	uint8_t data3[] = {1, 2, 3, 4, 5,6,7};
	ChaoJi_URSM_send(NULL, data3, sizeof(data3));
  return 0;
}
