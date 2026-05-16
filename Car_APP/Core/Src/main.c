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
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_init.h"
#include "lcd.h"
//#include "pic.h"
//#include "touch.h" 
#include "lvgl.h"
#include "lv_port_disp_template.h"
//#include "lv_port_indev_template.h"
//#include "lv_demo_stress.h"
//#include "lv_demo_widgets.h"
#include "ui.h"
//#include "dht11.h"
#include <stdio.h>
#include "canif.h"
#include "can_handle.h"
#include "inter_flashif.h"
#include "inter_flash_cfg.h"

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

RTC_TimeTypeDef struct_rtc_time;
RTC_DateTypeDef struct_rtc_date;
uint8_t time_buff[15] = {0};
uint8_t date_buff[15] = {0};
uint32_t timetick = 0;
uint32_t lvtick = 0;

uint32_t rpm_value,water_temp_value,safty_belt_value,light_value,turn_light_value,date_value,time_value;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  __set_PRIMASK(0);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_RTC_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_Base_Start_IT(&htim6);
	Can_Filter_Init();
	printf("                    Car APP  uart1\r\n");
  printf("----------------------------------------------------------------\r\n");

  printf("flash cfg size:%d\r\n", sizeof(inter_flash_cfg_param_typeDef));
  printf("----------------------------------------------------------------\r\n");
  inter_flash_cfg_init();          //   初始化flash配置   内部flash 参数区参数校验
	
	lv_init();
	lv_port_disp_init();
	//lv_port_indev_init(); //触摸已关闭
	//lv_demo_widgets();
	ui_init();
	//lv_demo_stress();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
     if (get_ota_flag() == 1) 
     {
          inter_flash_cfg_set_app_update_flag(1);
          printf("Restarting...\r\n");
          HAL_Delay(1000);
          HAL_NVIC_SystemReset();
     } 
		
		if(HAL_GetTick() - lvtick > 5)  //LVGL要求每隔5ms调用一次
		{
				lvtick = HAL_GetTick();
				lv_timer_handler();
		}
	
		if(HAL_GetTick() - timetick >500) //每隔500ms刷新一下时间显示
		{
				timetick = HAL_GetTick();
						
				HAL_RTC_GetTime(&hrtc, &struct_rtc_time, RTC_FORMAT_BIN);	
				sprintf((char *)(time_buff),"%2u:%2u:%2u", struct_rtc_time.Hours, struct_rtc_time.Minutes, struct_rtc_time.Seconds);			
				HAL_RTC_GetDate(&hrtc, &struct_rtc_date, RTC_FORMAT_BIN);
				sprintf((char *)date_buff,"20%2u-%2u-%2u", struct_rtc_date.Year, struct_rtc_date.Month, struct_rtc_date.Date);

				ui_set_date_time_value(date_buff, time_buff);	
		}			
				
		Can_Handle();  //不断接收can数据
		
		if(ui_get_rpm_value(&rpm_value) == 0) //仪表盘指针刷新
		{
				ui_set_rpm_angle(rpm_value);			
		}
					
		if(ui_get_water_temp_value(&water_temp_value) == 0)  //水温图标状态
		{
				ui_set_water_temp_color(water_temp_value);		
		}
		
		if(ui_get_safty_belt_value(&safty_belt_value) == 0)  //安全带图标状态
		{
				ui_set_safty_belt_color(safty_belt_value);
		}
		
		if(ui_get_light_value(&light_value) == 0) //灯图标状态
		{
				ui_set_light_color(light_value);
		}	
		
		if(ui_get_turn_light_value(&turn_light_value) == 0) //转向灯图标状态
		{
				ui_set_turn_light_color(turn_light_value);
		}		
		
		if(ui_get_date_value(&date_value) == 0) //日期更新
		{				
				RTC_DateTypeDef struct_rtc_set_date;
			
				struct_rtc_set_date.Year = ((date_value >> 16) & 0xff);
				struct_rtc_set_date.Month = ((date_value >> 8) & 0xff);
				struct_rtc_set_date.Date = ((date_value >> 0) & 0xff);
			
			  if (HAL_RTC_SetDate(&hrtc, &struct_rtc_set_date, RTC_FORMAT_BIN) != HAL_OK)
				{
						Error_Handler();
				}
		}	

		if(ui_get_time_value(&time_value) == 0)  //时间更新
		{
				RTC_TimeTypeDef struct_rtc_set_time;
			
				struct_rtc_set_time.Hours = ((time_value >> 24) & 0xff);
				struct_rtc_set_time.Minutes = ((time_value >> 16) & 0xff);
				struct_rtc_set_time.Seconds = ((time_value >> 8) & 0xff);
			
				if (HAL_RTC_SetTime(&hrtc, &struct_rtc_set_time, RTC_FORMAT_BIN) != HAL_OK)
				{
						Error_Handler();
				}
		}		
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM6)
	{
	 	lv_tick_inc(1);
	}

}
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
