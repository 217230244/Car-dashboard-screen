/**
 * @file iso_tp_cfg.c
 * @author argonSong
 * @brief 标准tp层配置文件 使用can 波特率为500kbps
 * @version 0.1
 * @date 2024-10-30
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "iso_tp_cfg.h"
#include "isotp.h"
#include "canif.h"
#define CAN_MSG_DATA_HEAD (0xAF) // CAN数据有效帧头
#define ISOTP_BUFSIZE 4096        // TP层缓冲区大小

static IsoTpLink g_link;
static uint8_t g_isotpRecvBuf[ISOTP_BUFSIZE];
static uint8_t g_isotpSendBuf[ISOTP_BUFSIZE];

uint8_t payload[4096] = {0};
uint16_t payload_size = 4096;
static uint8_t tp_ota_flag = 0; // tp层接收到升级包标志位





/**
 * @brief TP层初始化
 *
 * @return uint8_t
 */
uint8_t iso_tp_init()
{
    isotp_init_link(&g_link, CAN_ID_OTA_SEND,
                    g_isotpSendBuf, sizeof(g_isotpSendBuf),
                    g_isotpRecvBuf, sizeof(g_isotpRecvBuf));

    return 0;
}

/**
 * @brief ota 升级数据包发送函数
 *
 * @param pack_offset 数据包 写入flash 地址偏移
 * @param data 数据内容
 * @param pack_size 数据长度
 */
void iso_tp_send_ota_data(uint32_t pack_offset, uint8_t *data, uint32_t pack_size)
{
    uint32_t send_size = 0;
#if 1    
    /*
    TP: |1字节 | 4字节 |  4字节  |  1字节  | n字节数据|
        |0xBF |地址偏移| 数据长度| TP Index| 数据内容 |
    */
    payload[0] = 0xBF;
    payload[1] = (pack_offset >> 24) & 0xFF;
    payload[2] = (pack_offset >> 16) & 0xFF;
    payload[3] = (pack_offset >> 8) & 0xFF;
    payload[4] = (pack_offset >> 0) & 0xFF;
    
    payload[5] = (pack_size >> 24) & 0xFF;
    payload[6] = (pack_size >> 16) & 0xFF;
    payload[7] = (pack_size >> 8) & 0xFF;
    payload[8] = (pack_size >> 0) & 0xFF;
    memcpy(payload+9, data, pack_size);

    send_size = pack_size + 9; // 加上头部数据包长度

    //send_size = 32;
    int ret = isotp_blocking_send(&g_link, payload, send_size, 0xFFFFFF); // 发送数据包
    // int ret = isotp_send(&g_link, payload, send_size);
    printf("send daat ret:%d\r\n", ret);


#endif
}

///**
// * @brief 发送结束升级标志
// * 
// */
//void iso_tp_send_ota_end()
//{
//    uint32_t send_size = 0;
//    /*
//    TP: |1字节 | 4字节 |  4字节  |  1字节  | n字节数据|
//        |0xAF |地址偏移| 数据长度| TP Index| 数据内容 |
//    */
//    payload[0] = 0xAF;
//    payload[1] = (pack_offset >> 24) & 0xFF;
//    payload[2] = (pack_offset >> 16) & 0xFF;
//    payload[3] = (pack_offset >> 8) & 0xFF;
//    payload[4] = (pack_offset >> 0) & 0xFF;
//    
//    payload[5] = (pack_size >> 24) & 0xFF;
//    payload[6] = (pack_size >> 16) & 0xFF;
//    payload[7] = (pack_size >> 8) & 0xFF;
//    payload[8] = (pack_size >> 0) & 0xFF;


//    int ret = isotp_blocking_send(&g_link, payload, send_size, 0xFFFFFF); // 发送数据包
//    // int ret = isotp_send(&g_link, payload, send_size);
//    printf("send daat ret:%d\r\n", ret);
//}

void iso_tp_send_test()
{
    uint8_t txdata[128] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                           0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                           0x18, 0x19, 0x20};
    for (uint8_t i = 0; i < 128; i++)
    {

        txdata[i] = i;
    }

    // int  ret = isotp_send(&g_link, txdata, sizeof(txdata));
    int ret = isotp_blocking_send(&g_link, txdata, sizeof(txdata), 0xffffff);
    if (ISOTP_RET_OK == ret)
    {
        /* Send ok */
        printf("isotp_send\r\n");
    }
    else
    {
        printf("send recv error\r\n");
    }
}

void iso_tp_test()
{
    uint8_t payload[32] = {0};
    static uint32_t cnt = 0;
    for (uint32_t i = 0; i < 16; i++)
    {
        payload[i] = i;
    }
    int send_size = 32;
    // int ret = isotp_blocking_send(&g_link, payload, send_size, 0xFFFFFF); // 发送数据包

    // isotp_poll(&g_link); // tp层服务server

    cnt++;
    if (cnt == 1000)
    {
        cnt = 0;
        int ret = isotp_send(&g_link, payload, send_size);
        printf("send daat ret:%d\r\n", ret);
    }

    HAL_Delay(1);
}


/**
 * @brief TP层服务函数
 *
 */
void iso_tp_server()
{
    uint32_t recv_id = 0; // 接收到数据的ID
    uint16_t out_size = 0;
    uint8_t recv_len = 0;
    int ret = 0;
    uint8_t rxdata[8] = {0}; // canif层抛上来的接收数据

    recv_len = CAN1_Recv_Msg(&recv_id, rxdata);
    if (recv_len > 0)
    {
        if (recv_id == CAN_ID_OTA_RECV) // 收到接收ID
        {
            isotp_on_can_message(&g_link, rxdata, recv_len); // 讲数据传递给tp层
        }
    }
    isotp_poll(&g_link); // tp层服务server

    ret = isotp_receive(&g_link, payload, payload_size, &out_size);
    if (ISOTP_RET_OK == ret)
    {
        /* Handle received message */
        // printf("isotp_receive: \r\n");
        // for (uint8_t i = 0; i < out_size; i++)
        // {
        //     printf("%02X ", payload[i]);
        // }
        // printf("\r\n");

        /* And send message with isotp_send */
        // ret = isotp_send(&g_link, payload, payload_size);
        // if (ISOTP_RET_OK == ret)
        // {
        //     /* Send ok */
        //     printf("isotp_send\r\n");
        // }
        // else
        // {
        //     printf("send recv error\r\n");
        // }
    }
}

// https://blog.csdn.net/kian9one/article/details/140085589#:~:text=CAN%20Transp
// 单帧测试:
//  04 11 22 33 44 55 55 55

// 多帧测试： 2E F1 90 01 02 03 01 02 03 04 05 06 07 08 09 10 11 12 13 14
//  首帧：(LEN:20)
//  10 14 2E F1 90 01 02 03

// 连续帧:(20+1)
// 21 01 02 03 04 05 06 07
// 连续帧:(20+2)
// 22 08 09 10 11 12 13 14

// 最长数据7x15 + 6 = 111
