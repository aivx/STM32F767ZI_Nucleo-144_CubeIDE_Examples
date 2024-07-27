/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @author			: Jabed-Akhtar (GitHub)
  * @date			: 11.10.2022
  ******************************************************************************
  * Description:
  * 	- a source within this script: https://youtube.com/playlist?list=PLfIJKC1ud8gj1t2y36sabPT4YcKzmN_5D
  * 	- Related doc/file can be found at location '<project-dir>/zz_docs/...'
  *
  * 	- Difference between Mutex and Binary Semaphore
  * 	- Difference between Priority Inversion and Priority Inheritance
  *
  * @attention
  * Very imp note:
  * 	- comment out #include "cmsis_os.h" , as it is not used
  * 	- comment or remote by-default created task related stuffs
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "FreeRTOS.h"
#include "task.h"
//#include "timers.h"
#include "queue.h"
#include "semphr.h"
//#include "event_groups.h"

#include <stdio.h>
#include "stdlib.h"
#include <string.h>

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

UART_HandleTypeDef huart3;

//osThreadId normalTaskHandle;
/* USER CODE BEGIN PV */

/****** Mutex Handler *****/
SemaphoreHandle_t simpleMutex;
/****** Semaphore Handler *****/
SemaphoreHandle_t binSemaphore;

// defining Task Handlers **********
TaskHandle_t HPT_Task_Handler; // High Priority Task
TaskHandle_t MPT_Task_Handler; // Medium Priority Task
TaskHandle_t LPT_Task_Handler; // Low Priority Task

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
//void start_normalTask(void const * argument);

/* USER CODE BEGIN PFP */

void HPT_Task(void *argument); // High Priority Task Handler function
void MPT_Task(void *argument); // Medium Priority Task Handler function
void LPT_Task(void *argument); // Low Priority Task Handler function
void Send_Uart(char *str); // Function to send data via UART , acquire Mutex and release Mutex

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
  /* USER CODE BEGIN 2 */

  // sending data through USART at the beginning of Programm
  char stri[] = "\n---------- Programm started!!! ----------\n\n\r";
  HAL_UART_Transmit(&huart3, (uint8_t*)stri, strlen(stri), HAL_MAX_DELAY); //HAL_MAX_DELAY -> 0xFFFFFFFFU

  /***** creating Mutex ****/
  simpleMutex = xSemaphoreCreateMutex();
  if (simpleMutex != NULL) // if Mutex is created
  {
	  char *str1 = "Mutex Created.\n\r";
	  HAL_UART_Transmit(&huart3, (uint8_t*)str1, strlen(str1), HAL_MAX_DELAY);
  }

  /***** creating Binary Semaphore ****/
  binSemaphore = xSemaphoreCreateBinary();
  if(binSemaphore != NULL)
  {
	  char *str2 = "Bin-Semaphore Created.\n\r";
	  HAL_UART_Transmit(&huart3, (uint8_t*)str2, strlen(str2), HAL_MAX_DELAY);
  }
  // giving binary Semaphore (!needed before taking it)
  xSemaphoreGive(binSemaphore);

  /***** create Tasks *****/
  xTaskCreate(HPT_Task, "HPT", 128, NULL, 3, &HPT_Task_Handler);
  xTaskCreate(MPT_Task, "MPT", 128, NULL, 2, &MPT_Task_Handler);
  xTaskCreate(LPT_Task, "LPT", 128, NULL, 1, &LPT_Task_Handler); // Task for receiving data from Queue

  // start the Schedular
  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of normalTask */
//  osThreadDef(normalTask, start_normalTask, osPriorityNormal, 0, 128);
//  normalTaskHandle = osThreadCreate(osThread(normalTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
//  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB0 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// Medium Priority Task Handler function
/*
 * Medium/Normal Priority Task to send data via UART
 */
void HPT_Task(void *argument)
{
	char *strtosend = "In HPT =============================\n";

	while(1)
	{
//		char *str = "Entered HPT_Task and about to take the Bin-Semaphore.\n\n";
		char *str = "Entered HPT_Task and about to take the Mutex.\n\n";
		HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);

		Send_Uart(strtosend);

		char *str2 = "Leaving HPT_Task.\n\n";
		HAL_UART_Transmit(&huart3, (uint8_t *)str2, strlen(str2), HAL_MAX_DELAY);

		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Toggling LED-0 before Leaving HPT_Task

		vTaskDelay(750);
	}
}

// Medium Priority Task Handler function
/*
 * Medium/Normal Priority Task to send data via UART
 */
void MPT_Task(void *argument)
{
	while(1)
	{
		char *str = "In MPT =============================.\n\n";
		HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);

		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7); // Toggling LED-1 before Leaving MPT_Task

		vTaskDelay(2000);
	}
}

// Low Priority Task Handler function
/*
 * Low Priority Task to send data via UART
 */
void LPT_Task(void *argument)
{
	char *strtosend = "In LPT =============================\n";

	while(1)
	{
//		char *str = "Entered LPT_Task and about to take the Bin-Semaphore.\n\n";
		char *str = "Entered LPT_Task and about to take the Mutex.\n\n";
		HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);

		Send_Uart(strtosend);

		char *str2 = "Leaving LPT_Task.\n\n";
		HAL_UART_Transmit(&huart3, (uint8_t *)str2, strlen(str2), HAL_MAX_DELAY);

		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14); // Toggling LED-2 before Leaving HPT_Task

		vTaskDelay(1000);
	}
}

// Function to send data via UART
void Send_Uart(char *str)
{
	// for Binary-Semaphore
//	xSemaphoreTake(binSemaphore, portMAX_DELAY);
//	HAL_Delay(5000); // Delay of 5 sec for testing
//	HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
//	xSemaphoreGive(binSemaphore);

	// For Mutex
	xSemaphoreTake(simpleMutex, portMAX_DELAY);
	HAL_Delay(5000); // Delay of 5 sec for testing
	HAL_UART_Transmit(&huart3, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
	xSemaphoreGive(simpleMutex);
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_start_normalTask */
/**
  * @brief  Function implementing the normalTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_start_normalTask */
//void start_normalTask(void const * argument)
//{
//  /* USER CODE BEGIN 5 */
//  /* Infinite loop */
//  for(;;)
//  {
//    osDelay(1);
//  }
//  /* USER CODE END 5 */
//}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
