/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "boot_manager.h"
#include "inter_flashif.h"
#include "inter_flash_cfg.h"
#include "ymodem.h"
#include "common.h"
#include <stdio.h>


#include "common.h"
#include "canif.h"
#include "isotp.h"
#include "iso_tp_cfg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
uint8_t ymodule_run_flag = 0; // ymodule 运行标志
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define OTA_CALL_MAX_WAIT           (5*1000)            //5S ota升级呼叫等待超时
uint8_t tx_data[8] ={0};


static uint8_t pData[128];
static uint8_t ota_call_flag = 0;           //ota升级呼叫标志
static uint32_t ota_call_tick = 0;           //ota升级呼叫等待


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_CAN_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("uart1:usb tool...\r\n");
  HAL_UART_Transmit(&huart2, "uart2:usb tool...\r\n", strlen("uart2:usb tool\r\n"), 0xff);
  canif_init(); // CAN底层功能初始化
  iso_tp_init(); // TP层功能初始化

  while (1)
  {

#if 1
    //等待上位机发送升级
    if (HAL_UART_Receive(&huart2, pData, 4, 0xFF) == HAL_OK) // 0xAF 0x00 升级标志 0xFA
    {
      if ((pData[0] == 0xAF) && (pData[3] == 0xFA)) // 检测到升级标志
      {
        if (pData[2] == 0x01) // 升级使能位置
        {
            printf("Upgrade will start...\r\n");
          
            uint8_t send_msg[8] = {0xBE, 0xAD, 0xBE, 0xEF, 0x00, 0xAA, 0xAA, 0xAA};
            send_msg[4] = 0x01;             //确认升级
            CAN1_Send_Msg(CAN_ID_CALL_OTA, (uint8_t *)send_msg, 8);
            ota_call_flag = 1;          //设置升级标志
            
            ota_call_tick = OTA_CALL_MAX_WAIT + HAL_GetTick(); // 设置升级超时时间
        }
      }
    }
    
    //建立升级请求
    if(ota_call_flag == 1)
    {
        if((ota_call_tick >=  HAL_GetTick()) && (ota_call_tick != 0))    //升级超时未到
        {
          uint32_t recv_id = 0; // 接收到数据的ID
          uint8_t recv_len = 0;
          uint8_t rxdata[8] = {0};
          
          recv_len = CAN1_Recv_Msg(&recv_id, rxdata);
          if(recv_len > 0)
          {
            if(recv_id == CAN_ID_CALL_OTA_ACK)      //收到升级确认信息
            {
                printf("Upgrade ack received...\r\n");
                ota_call_tick = 0;
                ota_call_flag = 0;
                ymodule_run_flag = 1; // 启动ymodem升级程序
            }
          }
        }
        else if((ota_call_tick <  HAL_GetTick()) && (ota_call_tick != 0))              //升级超时
        {
            ota_call_flag = 0;
            ota_call_tick = 0;
            printf("Upgrade timeout...\r\n");
        }
    
    }
#endif  
 
#if 1 // ymodem 升级程序
        if (ymodule_run_flag == 1)
        {
            printf("Ymodule&TPClient is run,waiting for upgrade...\r\n");
            // Ymodem 串口协议测试
            extern uint8_t aFileName[FILE_NAME_LENGTH];

            uint8_t number[11] = {0};
            uint32_t size = 0;
            COM_StatusTypeDef result;
            result = Ymodem_Receive(&size);
            if (result == COM_OK)
            {
                printf((uint8_t *)"\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: %s", aFileName);

                Int2Str(number, size);
                printf("Size: %dBytes", number);
  
                // 升级完成后，清除升级标志 
                ymodule_run_flag = 0;
            }
            else
            {
                printf("=---<ret:%d\r\n", result);
            }
        }
        else
        {
            // HAL_Delay(1000);
            // printf("system error\r\n");
            // printf("reboot...\r\n");
            // HAL_Delay(2000);

            // // 重启
            // NVIC_SystemReset();
        }
#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
