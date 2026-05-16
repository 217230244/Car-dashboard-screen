/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include "canif.h"
//uint8_t uart_rx_data[8] ={0};
uint8_t uart_rx_data;    //改成一个字节变量
static UART_RX_typeDef struct_uart2_rx_data; 
/* USER CODE END 0 */



UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void uart2_recv_it_enable(void)
{
		HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
}

int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//		if(huart == &huart2)
//		{
//				uint8_t can_tx_data[8] = {0};
//				for(int i=0; i<8; i++)
//						can_tx_data[i] = uart_rx_data[i];
//				if(can_tx_data[0] == 0xaf)
//				{
//						uint32_t ret = Can_Send_Msg(0x0B4, 8,can_tx_data);
//						if(ret == 0)
//						{
//								printf("can send");
//								for(int x=0; x<8; x++)
//								{
//										printf("%X ",can_tx_data[x]);
//								}
//								printf("\r\n");
//						}
//				}
//				HAL_UART_Receive_IT(&huart2, uart_rx_data, 8);				
//		}
//}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if(huart ->Instance == USART2)
  { 
    struct_uart2_rx_data.rx_buf[struct_uart2_rx_data.rx_cnt] =  uart_rx_data;
    struct_uart2_rx_data.rx_cnt ++;

  
		if(struct_uart2_rx_data.rx_buf[0] == 0xAF) //判断包头AF
		{
			if(struct_uart2_rx_data.rx_cnt >= 9) //数据过长，包数据归零
			{ 
				struct_uart2_rx_data.rx_cnt = 0;
        printf("data len error\r\n");
			}	
      else
      {
        if(struct_uart2_rx_data.rx_cnt > 2) 
        {
          uint8_t data_len = struct_uart2_rx_data.rx_buf[1]; //读取一帧  数据的长度值

          if((data_len > 1) && (data_len < 6))  //判断数据长度值对不对 我们用到了 2 4 5 三种长度  
          {
            if(struct_uart2_rx_data.rx_cnt >= (data_len + 3)) //确定接收到了 整包数据 
            {
              if(struct_uart2_rx_data.rx_buf[data_len + 2] == 0xFA) //校验包尾FA
              {
								uint8_t can_tx_data[8] = {0};
								can_tx_data[0] = 0xAF;
								printf("uart recv:");   //串口一打印接收到的数据   测试使用，实际工程可以毙掉
                printf("%X ",struct_uart2_rx_data.rx_buf[0]);
								printf("%X ",struct_uart2_rx_data.rx_buf[1]);
								for(int i=0; i<data_len; i++)  //提取有用数据，通过can发送给zet6
								{
										can_tx_data[i + 1] = struct_uart2_rx_data.rx_buf[i + 2]; //	其实就是去掉了 数据长度那个字节
										printf("%X ",struct_uart2_rx_data.rx_buf[i + 2]);									
								}
								printf("%X ",struct_uart2_rx_data.rx_buf[data_len + 2]);
								printf("\r\n");
								uint32_t ret = Can_Send_Msg(0x0B4, 8,can_tx_data);
								if(ret == 0)
								{
									printf("can send:");  //串口一把can发送的数据打印出来
									for(int x=0; x<8; x++)
									{
										printf("%X ",can_tx_data[x]);
								  }
								  printf("\r\n\r\n");
						    }
								struct_uart2_rx_data.rx_cnt = 0;                
              }
              else
              {
                struct_uart2_rx_data.rx_cnt = 0;
                printf("data tail error\r\n");
              }
            }
          }
          else
          {
            struct_uart2_rx_data.rx_cnt = 0;
            printf("data len error\r\n");
          }
        }
      }	
		}
		else
		{
		struct_uart2_rx_data.rx_cnt = 0;
		}
    HAL_UART_Receive_IT(&huart2, &uart_rx_data, 1);
  }
}

/* USER CODE END 1 */
