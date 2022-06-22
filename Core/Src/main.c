/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "stdint.h"

#include "ICM_20648.h"
#include "LED_Driver.h"
#include "Motor_Driver.h"
#include "AS5047P.h"
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
void as5047p_spi_send(uint16_t data){
  //return 0;
  HAL_SPI_Transmit(&hspi3, (uint8_t *)&data, 1, 100);
}
uint16_t as5047p_spi_read(void){
  uint16_t data = 0;
  HAL_SPI_Receive(&hspi3, (uint8_t *)&data, 1, 100);
  return data;
}
void as5047p_spi_select()
{
  switch(spi3_dev_num){
    case 0:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
    break;
    case 1:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    break;
  }
}
void as5047p_spi_deselect(int n)
{
  switch(spi3_dev_num){
    case 0:
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
    break;
    case 1:
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
    break;
  }
}

//SWD Debug
int _write(int file, char *ptr, int len)
{
  int DataIdx;
  for(DataIdx=0; DataIdx<len; DataIdx++)
  {
    ITM_SendChar(*ptr++);
  }
  return len;
}
//USART printf
#ifdef __GNUC__
        #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
        #define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
        #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
        #define GETCHAR_PROTOTYPE int f getc(FILE* f)
#endif /*__GNUC__*/
PUTCHAR_PROTOTYPE {
        HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 0xFFFF);
        return ch;
}
int __io_getchar(void) {
HAL_StatusTypeDef Status = HAL_BUSY;
uint8_t Data;

while(Status != HAL_OK)
{
Status = HAL_UART_Receive(&huart1, &Data, sizeof(Data), 10);
//if(Status == HAL_ERROR)
//{
//      return 0;
//      break;
//}

}

return(Data);
}
//GPIO EXTI Callback
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
  //swtitch IT
  static int i=0;
  //ChangeLED(i);
  i++;
  if(i > 7) i=0;
}
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
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_SPI3_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM10_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  uint16_t adcdata[5]={0};
  HAL_TIM_OC_Start_IT(&htim1,TIM_CHANNEL_1);
  HAL_TIMEx_OCN_Start_IT(&htim1, TIM_CHANNEL_1); // 位相
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcdata, 5);
  for(int i=0; i < 2; i++){
      if(IMU_init()){
      printf("OK\r\n");
    }else{
      printf("nomusan\r\n");
    }
  }
  Motor_PWM_Start();
  Motor_Switch(1000, 1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //ChangeLED(5);
	  for(int i=0; i < 5; i++){
		  printf("%d ", adcdata[i]);
		  HAL_Delay(200);
	  }
	  read_gyro_data();
	  printf(",%d\r\n",zg);
	  HAL_Delay(200);

#if 0
    float angle_degree;
    as5047p_dev_num_select(ENC_LEFT);
    if(as5047p_get_angle(0, &angle_degree) == 0){
      printf("左角度 : %f\r\n", angle_degree);
    }
    else{
      printf("error 左\r\n");
    }
    as5047p_dev_num_select(ENC_RIGHT);
    if(as5047p_get_angle(0, &angle_degree) == 0){
      printf("右角度 : %f\r\n", angle_degree);
    }
    else{
      printf("error 右\r\n");
    }
#else
    //tim3,4でABモード(デフォ)
    printf("%ld, %ld\r\n", TIM3->CNT, TIM4->CNT);
#endif
    HAL_Delay(200);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
