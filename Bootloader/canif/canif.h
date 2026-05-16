#include "main.h"
#include "can.h"
#ifndef __CANIF_H__
#define __CANIF_H__


#define CAN_ID_OTA_RECV 0x0B1            // OTA升级 目标端-》主机端 数据ID
#define CAN_ID_OTA_SEND 0x0B2            // OTA升级 主机端-》目标端 数据ID


#define CAN_ID_CALL_OTA 0x0B3       // 请求升级标志 数据ID
#define CAN_ID_UI 0x0B4             // UI 刷新数据ID

#define CAN_ID_CALL_OTA_ACK 0x0D3  // 请求升级确认 数据ID
uint8_t canif_init();

void CAN1_Send_Test();
uint8_t CAN1_Send_Msg(uint32_t id, uint8_t *msg, uint8_t len);
uint8_t CAN1_Recv_Msg(uint32_t *id, uint8_t *buf);
// uint8_t CAN1_Send_Msg(uint8_t *msg, uint8_t len);
// uint8_t CAN1_Recv_Msg(uint8_t *buf);
#endif /* __CANIF_H__ */