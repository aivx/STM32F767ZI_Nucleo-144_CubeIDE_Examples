/* USER CODE BEGIN Header */
/**
  *******************************************************************************
  * @file           : main.c													*
  * @brief          : A UDP-Server Project										*
  *******************************************************************************
  * @author			: Jabed-Akhtar (GitHub)										*
  * @date			: 30.10.2022												*
  *******************************************************************************
  * @attention																	*
  * 	-> Tag: WORKING :)														*
  *																				*
  * Imp note:																	*
  * 	- comment out #include "cmsis_os.h" , as it is not used					*
  * 	- comment or remote by-default created task related stuffs				*
  *																				*
  * Description:																*
  * 	- This project sends Hello msg to Client, when Client sends any msg.	*
  * 	- MCU-Board IP Address: 192.168.178.204	(See MX_LWIP_Init(); )			*
  * 	- LED for Info: PB0 (Green LED) toggles after sending out msg to Client *
  * 	- UDP-Server programming is done in task_TCPServer						*
  *		- UDP-Server Programming steps:											*
  *			|- 1. Create UDP Socket												*
  *			|- 2. Bind Socket to the Server Address							    *
  *			|- Rx <-> Tx | recvfrom() <-> sendto()								*
  *			|- 3. Close the Socket											    *
  *	    																		*
  *******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "cmsis_os.h"
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "lwip/api.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SERVADDR "169.254.181.186" //"169.254.81.179" n // Address of the Server to connect this Client to.
#define SERV_PORT 2414			// Server port this Client will be connected to.
#define MAXBUFFLEN 100			// Maximum string length
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;

//osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
//void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
void vAplicationIdleHook(void);					// Task > Idle Hook
void task_UDPClient(void *pvParameters);		// Task for this UDP-Server
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

  // Initialize LWIP
  MX_LWIP_Init();

  char stri[] = "\n---------- Programm started!!! ----------\n\n\r";
  HAL_UART_Transmit(&huart3, (uint8_t*)stri, strlen(stri), HAL_MAX_DELAY);

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
  /* definition and creation of defaultTask */
//  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
//  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  xTaskCreate(task_UDPClient, "task_UDPClient", configMINIMAL_STACK_SIZE*4, NULL, 4, NULL);	// Thread/Task for UDP Client

  /***** Starting Scheduler *****/
  vTaskStartScheduler();

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/*
 * -> Idle Hook Task
 */
void vAplicationIdleHook(void){
	__WFI();
}

/*
 * -> Task for UDP Server
 * description:
 * 		- Makes connection with Client
 * 		- Wait for msg from the Client
 * 		- Sends Hello msg to Client on receiving any msg from the Client
 */
void task_UDPClient(void *pvParameters)
{
	/* --- Variables -----------------------------------------------------------*/
	int					sockfd;					// Socket for this Client
	int					addr_len;				// Size of sock Addr for recvfrom(...);
	struct sockaddr_in 	servaddr_toConn, cliaddr;		// Struct for the UDP-Server and for this Client
	int					numBytes;				// Number of Bytes sent/received
	char 				buff[MAXBUFFLEN];		// Buffer to save Rx/Tx Data

	/* --- 1. Create UDP Socket ------------------------------------------------*/
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		exit(0);
	}

	// Assignments to servaddr
	servaddr_toConn.sin_family = AF_INET;					// Address-Family -> Internet
	servaddr_toConn.sin_addr.s_addr = inet_addr(SERVADDR); 	// Auto-Fill with MCU-Board IP-Address
	servaddr_toConn.sin_port = htons(SERV_PORT);			// The Server port

	/* --- 2. Bind Socket to the Server Address --------------------------------*/
//	int b_ret = bind(sockfd, (struct sockaddr *)&servaddr_toConn, sizeof(struct sockaddr));
//	if(b_ret == -1)
//	{
//		exit(0);
//	}

	/****** Endless-Loop *****/
	while(1)
	{
		// Filling Buffer-Memory with 0 -> Clearing Buffer-Memory
		memset(buff, 0, sizeof(buff));

		/* --- Receiving from Client -------------------------------------------*/
//		addr_len = sizeof(struct sockaddr);
//		numBytes = recvfrom(sockfd, buff, MAXBUFFLEN, 0, (struct sockaddr *)&servaddr_toConn, &addr_len);
//		if(numBytes==-1)
//		{
//			exit(1);
//		}

		/* --- Sending to Client -----------------------------------------------*/
		int n = sendto(sockfd, "Hello from Client!\n", 20, 0, (struct sockaddr *)&servaddr_toConn, sizeof(struct sockaddr));
		if(n == -1)
		{
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		}

		// Toggle Green LED after sending out msg to the Client
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

		HAL_Delay(1000);


		/* --- 3. Close the Socket ---------------------------------------------*/
//		close(sockfd); 	/* Close socket at the end of programm. !Here it could not be a good idea, */
						/* as here the socket will be closed after sending msg to Client once :(   */
	}
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
//void StartDefaultTask(void const * argument)
//{
//  /* init code for LWIP */
//  MX_LWIP_Init();
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
  * @note   This function is called  when TIM10 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM10) {
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
