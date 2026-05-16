#include "canif.h"
#include "can.h"
#include "usart.h"


void Can_Filter_Init(void)
{
	CAN_FilterTypeDef Can_FilterConfig;

	Can_FilterConfig.FilterBank = 0;
	Can_FilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
	Can_FilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
	Can_FilterConfig.FilterIdHigh = 0x0;
	Can_FilterConfig.FilterIdLow = 0x0;
	Can_FilterConfig.FilterMaskIdHigh = 0x0;
	Can_FilterConfig.FilterMaskIdLow = 0x0;
	Can_FilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	Can_FilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	Can_FilterConfig.SlaveStartFilterBank = 14;

	if (HAL_CAN_ConfigFilter(&hcan, &Can_FilterConfig) != HAL_OK)
	{
			Error_Handler();
	}

	HAL_CAN_Start(&hcan);

}



uint32_t Can_Send_Msg(uint32_t id, uint32_t len,uint8_t *buf)
{

		CAN_TxHeaderTypeDef TxMessage;
		uint32_t Tx_Mail = CAN_TX_MAILBOX0;
		TxMessage.StdId = id;
		TxMessage.IDE = CAN_ID_STD;
		TxMessage.DLC = len;
		TxMessage.RTR  = CAN_RTR_DATA;		


		if(HAL_CAN_AddTxMessage(&hcan, &TxMessage,buf, &Tx_Mail) != HAL_OK)
		return 1;
		
		while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) !=3);		
		return 0;
}


uint32_t Can_Recv_Msg(uint32_t *id, uint8_t *buf)
{

		CAN_RxHeaderTypeDef RxMessage;
		
		if(HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_FILTER_FIFO0) == 0)
			return 0;
	
		HAL_CAN_GetRxMessage(&hcan, CAN_FILTER_FIFO0,&RxMessage, buf);
	
		*id = RxMessage.StdId;

		return RxMessage.DLC;		
}
