#include "canif.h"
#include <stdio.h>
// CAN_TxHeaderTypeDef can_Tx;
static CAN_TxHeaderTypeDef TxMessage; // CAN发送的消息的消息头
static CAN_RxHeaderTypeDef RxMessage; // CAN接收的消息的消息头


static void CANFilter_Config(void);

/**
 * @brief CAN 外设初始化
 *
 * @return uint8_t
 */
uint8_t canif_init()
{

    // HAL_CAN_Start(&hcan);

    /* 1. CAN Filter Config */
    CANFilter_Config();

    /* 2. 使能CAN通信 */
    if (HAL_CAN_Start(&hcan) != HAL_OK)
    {
        printf("CAN Start Fail\r\n");
        Error_Handler();
    }
    printf("CAN Start Success\r\n");

    return 0;
}


/* CAN过滤器配置函数 */
static void CANFilter_Config(void)
{
    CAN_FilterTypeDef  sFilterConfig;
    
    sFilterConfig.FilterBank = 0;                       //CAN过滤器编号，范围0-27
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;   //CAN过滤器模式，掩码模式或列表模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;  //CAN过滤器尺度，16位或32位
    sFilterConfig.FilterIdHigh = 0x000 << 5;			//32位下，存储要过滤ID的高16位
    sFilterConfig.FilterIdLow = 0x0000;					//32位下，存储要过滤ID的低16位
    sFilterConfig.FilterMaskIdHigh = 0x0000;			//掩码模式下，存储的是掩码
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;				//报文通过过滤器的匹配后，存储到哪个FIFO
    sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;    		//激活过滤器
    sFilterConfig.SlaveStartFilterBank = 14;             ////指定为CAN1分配多少个滤波器组
    
    if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK) {
        printf("CAN Filter Config Fail!\r\n");
        Error_Handler();
    }
    printf("CAN Filter Config Success!\r\n");

}

uint8_t CAN1_Send_Msg(uint32_t id, uint8_t *msg, uint8_t len)
{
    uint16_t i = 0;
    uint32_t txMailBox = CAN_TX_MAILBOX0;
    uint8_t send_buf[8];
	
    TxMessage.StdId = id;
    TxMessage.ExtId = id;
    TxMessage.IDE = CAN_ID_STD;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = len;
	
    for(i = 0; i < len; i++)
	send_buf[i] = msg[i];
	
    if(HAL_CAN_AddTxMessage(&hcan, &TxMessage, send_buf, &txMailBox) != HAL_OK)
	    return 1;	

    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) != 3);

    return 0;
}

/**
 * @brief 数据接收查询
 * 
 * @param buf 接收到数据的缓存区
 * @return uint8_t 接收到的数据长度
 */
uint8_t CAN1_Recv_Msg(uint32_t *id, uint8_t *buf)
{
    uint16_t i = 0;

    if(HAL_CAN_GetRxFifoFillLevel(&hcan, CAN_RX_FIFO0) == 0)         //无数据
    {
        return 1;
    }

    HAL_CAN_GetRxMessage(&hcan, CAN_RX_FIFO0, &RxMessage, buf);
#if 0	
    if(RxMessage.IDE == CAN_ID_STD)
	printf("StdId ID: %d\r\n", RxMessage.StdId);
    else
	printf("ExtId ID: %d\r\n", RxMessage.ExtId);
	
    printf("CAN IDE: %d\r\n", RxMessage.IDE);
    printf("CAN RTR: %d\r\n", RxMessage.RTR);
    printf("CAN DLC: %d\r\n", RxMessage.DLC);
    printf("Recv Data: ");
	
    for(i = 0; i < RxMessage.DLC; i++)
	printf("%x ",buf[i]);
	
    printf("\n");
#endif
    
    if(id != 0)
    {
        *id = RxMessage.StdId;
    }
    

    return RxMessage.DLC;
}

