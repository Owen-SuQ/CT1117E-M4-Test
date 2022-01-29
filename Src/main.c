/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "USERLIB.h"
#include <stdio.h>

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
uint8_t ledODR = 7;
uint8_t direction = 0;
uint8_t ucLcd[21];                        /* LCD閺佺増宓? */
//uint16_t usLcd;                         /* LCD閸掗攱鏌婇弮鍫曟？ */
uint8_t ucSec = 100;
uint8_t ucCnt = 10;
uint8_t cmd = 0;
uint32_t ADC_Value = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim8)
    {
        ucSec = ucSec + 1;

        LCD_Proc();

        LED_Process(&ledODR, direction);

        HAL_ADC_Start_IT(&hadc1);

    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    HAL_ADC_Stop_IT(&hadc1);     //启动ADC转换
    //HAL_ADC_PollForConversion(&hadc1, 50);   //等待转换完成，50为最大等待时间，单位为ms


//    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
//    {
    ADC_Value = HAL_ADC_GetValue(&hadc1);
    sprintf((char *) ucLcd, " ADC1:%03u", ADC_Value);
    LCD_DisplayStringLine(Line2, ucLcd);
    //HAL_UART_Transmit(&huart1, ADC_Value, sizeof(ADC_Value), 1000);
//    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (cmd == 0xA1)
        {
            direction = 1;
            HAL_UART_Transmit(&huart1, "Right!\n", sizeof("Right!\n"), 10000);
        }
        if (cmd == 0xA2)
        {
            direction = 0;
            HAL_UART_Transmit(&huart1, "left!\n", sizeof("left!\n"), 10000);
        }
        HAL_UART_Receive_IT(&huart1, &cmd, 1);
    }
}
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
    MX_TIM8_Init();
    MX_USART1_UART_Init();
    MX_ADC1_Init();
    /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start_IT(&htim8);
    //HAL_Delay(500);
    LCD_Init();                                   /* LCD閸掓繂顫愰崠? */
    LCD_Clear(Black);                       /* LCD濞撳懎鐫? */
    LCD_SetTextColor(White);                /* 鐠佸墽鐤嗙?涙顑? */
    LCD_SetBackColor(Black);                /* 鐠佸墽鐤嗛懗灞炬珯 */

    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);    //AD校准

    HAL_UART_Receive_IT(&huart1, &cmd, 1);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {

        //HAL_Delay(500);
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

    /** Configure the main internal regulator output voltage
    */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
    RCC_OscInitStruct.PLL.PLLN = 85;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the peripherals clocks
    */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_ADC12;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void LCD_Proc(void)                /* LCD婢跺嫮鎮? */
{
    sprintf((char *) ucLcd, " SEC:%03u   CNT:%03u ", ucSec, ucCnt);
    LCD_DisplayStringLine(Line1, ucLcd);
//    LCD_DisplayStringLine(Line2, ucLcd);
//    LCD_DisplayStringLine(Line3, ucLcd);
//    LCD_DisplayStringLine(Line4, ucLcd);
//    LCD_DisplayStringLine(Line5, ucLcd);
//    LCD_DisplayStringLine(Line6, ucLcd);
//    LCD_DisplayStringLine(Line7, ucLcd);
//    LCD_DisplayStringLine(Line8, ucLcd);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
