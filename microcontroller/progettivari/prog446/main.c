/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "mialibstm32.h"
#include "stm32f4xx_HC-SR04.h"
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_on(adc) { adc->CR2 |= 1; }
#define ADC_off(adc) { adc->CR2 &= ~(uint32_t)1; }

#define ADC_completed(adc)	((adc->SR & 2) != 0)
#define ADC_read(adc)	adc->DR
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int measure_done, sample;
uint16_t start_time, us;
float hcsr04_read (GPIO_TypeDef* GPIOx_trig, uint16_t GPIO_Pin_trig, GPIO_TypeDef* GPIOx_echo , uint16_t GPIO_Pin_echo){
	uint32_t local_time;
	local_time=0;
	HAL_GPIO_WritePin(GPIOx_trig, GPIO_Pin_trig, GPIO_PIN_RESET);  // pull the TRIG pin HIGH
	HAL_Delay(2);  // wait for 2 us


	HAL_GPIO_WritePin(GPIOx_trig, GPIO_Pin_trig, GPIO_PIN_SET);  // pull the TRIG pin HIGH
	HAL_Delay(10);  // wait for 10 us
	HAL_GPIO_WritePin(GPIOx_trig, GPIO_Pin_trig, GPIO_PIN_RESET);  // pull the TRIG pin low

	// read the time for which the pin is high

	while (!(HAL_GPIO_ReadPin(GPIOx_echo, GPIO_Pin_echo)));  // wait for the ECHO pin to go high
	while (HAL_GPIO_ReadPin(GPIOx_echo, GPIO_Pin_echo))    // while the pin is high
	 {
		local_time++;   // measure time for which the pin is high
		HAL_Delay(1);
	 }
	return local_time* .034/2;
}

int true_dist_read(){
	while (!sample) {}
			sample = 0;
			measure_done = 0;
			TIM_set(TIM2,0);
			GPIO_write(GPIOA, 1, 0);
			HAL_Delay(1);
			GPIO_write(GPIOA, 1, 1);
			HAL_Delay(1);
			GPIO_write(GPIOA, 1, 0);
			GPIOA->OTYPER;
			while (!measure_done && !sample) { } // wait for capture or sample timeout

			if (measure_done) {
				// capture OK!
				float distance = (us*68)/100;
				return  (int) distance;
			}
			return 0;
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
  /* Initialize distance sensor1 on pins; ECHO: PB9, TRIGGER: PB8 */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  GPIO_config_input(GPIOB,9);		//echo
  GPIO_config_output(GPIOB,8);		//trig
  ADC_on(ADC1);
  ADC_sample_channel(ADC1, 10);
  char str[10]="";
    GPIO_config_output(GPIOA, 1);
	GPIO_config_alternate(GPIOA, 0, 1); // AF1 --> TIM2,CH1

	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 84, 50000); // prescaler at 1us timeout of 50 msecs
	TIM_config_IC(TIM2, 1, IC_BOTH_EDGES); // capture on both edges
	TIM_set(TIM2, 0);
	TIM_on(TIM2);
	TIM_enable_irq(TIM2, IRQ_UPDATE | IRQ_CC_1); // IRQ su UPDATE e su Capture canale 1
	GPIO_write(GPIOB,8,1);
	GPIO_write(GPIOA, 1, 0);
  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  int Distance = true_dist_read();
	  sprintf(str,"%4i\r",Distance);
	  HAL_UART_Transmit(&huart2,(uint8_t*)str,10,100);
	  HAL_Delay(1000);
	  ADC_start(ADC1);
	  while (!ADC_completed(ADC1)) {}
	  int value = ADC_read(ADC1);
	  if(value>=0 && value< 100){
		  //HAL_GPIO_WritePin(Wled_GPIO_Port,Wled_Pin,SET);
//		  Wled_GPIO_Port->ODR&=~(1<<Wled_Pin);
//		  Wled_GPIO_Port->ODR|=1<<Wled_Pin;
//		  Wled_GPIO_Port->ODR = (Wled_GPIO_Port->ODR & ~(int32_t)(1<<Wled_Pin))| (1 << Wled_Pin);
		  Wled_GPIO_Port->BSRR=Wled_Pin;
		  //Wled_GPIO_Port->BSRR=SET<<Wled_Pin;
		  //HAL_GPIO_WritePin(Gled_GPIO_Port,Gled_Pin,RESET);
//		  Gled_GPIO_Port->ODR&=~(1<<Gled_Pin);
//		  Gled_GPIO_Port->ODR|=0<<Gled_Pin;
//		  Gled_GPIO_Port->ODR = (Gled_GPIO_Port->ODR & ~(int32_t)(1<<Gled_Pin))| (0 << Gled_Pin);
		  Gled_GPIO_Port->BSRR = (uint32_t)Gled_Pin << 16U;
		  //Wled_GPIO_Port->BSRR=RESET<<Wled_Pin;
	  }
	  else{
		  //HAL_GPIO_WritePin(Wled_GPIO_Port,Wled_Pin,RESET);
		  Wled_GPIO_Port->ODR &= (~(uint32_t)(Wled_Pin));
		  //HAL_GPIO_WritePin(Gled_GPIO_Port,Gled_Pin,SET);
		  Gled_GPIO_Port->ODR = (Gled_GPIO_Port->ODR & (~(uint32_t)(Gled_Pin)))| (Gled_Pin);
		  //Gled_GPIO_Port->ODR|=Gled_Pin;
	  }
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

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_8B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
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

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Bled_Pin|Gled_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Wled_GPIO_Port, Wled_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Bled_Pin Gled_Pin */
  GPIO_InitStruct.Pin = Bled_Pin|Gled_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Wled_Pin */
  GPIO_InitStruct.Pin = Wled_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Wled_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : X_button_Pin Y_button_Pin Z_button_Pin T_button_Pin */
  GPIO_InitStruct.Pin = X_button_Pin|Y_button_Pin|Z_button_Pin|T_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch(GPIO_Pin){
	case X_button_Pin:
		HAL_GPIO_TogglePin(Wled_GPIO_Port,Wled_Pin);
		break;
	case Y_button_Pin:
		HAL_GPIO_TogglePin(Bled_GPIO_Port,Bled_Pin);
		break;
	case Z_button_Pin:
		HAL_GPIO_TogglePin(Gled_GPIO_Port,Gled_Pin);
		break;
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {
		// period match, sample timeout
		sample = 1;
		TIM_update_clear(TIM2);
	}
	if (TIM_cc_check(TIM2,1)) {
		// capture
		if (GPIO_read(GPIOA, 0) == 1) { // fronte di salita
			start_time = TIM2->CCR1;
		}
		else { // fronte di discesa
			us = TIM2->CCR1 - start_time;
			measure_done = 1;
		}
		TIM_cc_clear(TIM2,1);
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
	char str[10]="errore\n";
	HAL_UART_Transmit(&huart2,(uint8_t*)str,10,0);
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
