/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  * This software is written for the test of the PPM module based on Nucleo-746.
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PPM.h"
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
uint8_t msg_buffer[200];
const uint8_t packet_lost_msg [] = "Packet Lost...\n";
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  if ( HAL_TIM_Base_Start_IT(&htim6) != HAL_OK )
  {
	  HAL_UART_Transmit(&huart3,(const uint8_t*) "Timer6 Configuration Error\n",
	  				sizeof("Timer6 Configuration Error\n"), 10);
  }
  if (HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1) != HAL_OK )
	{
		HAL_UART_Transmit(&huart3,(const uint8_t*) "Timer3 Configuration Error\n",
				sizeof("Timer3 Configuration Error\n"), 10);
	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  /*sprintf((char*)msg_buffer,"channel[%d]=%d\n",0,PpmGetChannelRaw(0));
	  HAL_UART_Transmit(&huart3, msg_buffer, sizeof(msg_buffer),10);*/
	  if( PpmStatusCheck() == kPpmPacketLost )
	  {
		  HAL_UART_Transmit(&huart3, packet_lost_msg, sizeof(packet_lost_msg),10);
	  }
	  else
	  {
		  sprintf((char*)msg_buffer,"channel[%d]=%d\n",kPpmChannel1,PpmGetChannelRaw(kPpmChannel1));
		  HAL_UART_Transmit(&huart3, msg_buffer, sizeof(msg_buffer),10);
	  }
	  HAL_Delay(500);

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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
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
uint32_t pulse_width = 0;
static uint16_t tim3_capture_width = 0;
static uint16_t tim3_capture1 = 0;
static uint16_t tim3_capture2 = 0;
/**
 * @fn void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef*)
 * @brief 	This function is called when the rising edge of the PPM signal occurred
 *			The timer counter is picked up and the difference between current
 *			counter and previously stored counter are calculated.
 * @pre
 * @post  the timer must be initialized at 1 MHz and the rising or the falling edge triggered.
 * @param htim handler of timer
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim)
{
	if(htim->Instance == TIM3 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		/**
		 * The timer counts from 0 to 0xFFFF. When the counter register is read,
		 * it may be a number range of 0 to 0xFFFF. The difference between this
		 * to number should be calculated. It's possible first number is greater
		 * than the second number that originates from the essence of the circular
		 * timer counter. this part of the code solves this issue.
		 */
		tim3_capture2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

		if(tim3_capture2 > tim3_capture1)
		{
			tim3_capture_width = (tim3_capture2 - tim3_capture1);
			PpmEngine(tim3_capture_width);
		}
		else if (tim3_capture2 < tim3_capture1)
		{
			tim3_capture_width = ((0xFFFF - tim3_capture1) + tim3_capture2)+1;
			PpmEngine(tim3_capture_width);
		}
		else
		{
			/*An error is occurred */
			PpmTimeOut();
		}
		tim3_capture1 = tim3_capture2;
	}

}
/**
 * @fn void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef*)
 * @brief This function should be implemented similarly to the watch-dog
 * 			this function is called after one second and it calls
 * 			PpmTimeOunt.To avoid this, PpmHeartBeat must be implemented
 * 			to reset the counter of this timer. PpmHeartBeat is called
 * 			when a health packet is received by PpmEngine.
 * @pre
 * @post  the timer must be initialized at GP mode and count down for N
 * 		  N stands for N seconds time out. N is related to application
 * 		  and safe mode policy.
 * @param htim handler of timer
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	if( htim->Instance == TIM6 )
	{
		PpmTimeOut();
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
	}
}
/**
 * @fn void PpmHeartBeat(void)
 * @brief This function is prototyped in the PPM module and is called
 * 			in the PpmEngine function when a healthy packet is received.
 * 			If PpmTimeOut is called, then this function must be implemented.
 * 			this function is prototyped in PPM.h as a weak function
 * @pre
 * @post
 */
void PpmHeartBeat(void)
{
	__HAL_TIM_SET_COUNTER(&htim6,0);

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
