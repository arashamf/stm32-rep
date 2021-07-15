/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "task.h"
#include "queue.h"
#include "semphr.h"
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
ADC_HandleTypeDef hadc2;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart3;

osThreadId defaultTaskHandle;
osThreadId task_ADCHandle;
osThreadId task_UART_ADCHandle;
osThreadId task_UART4_ISRHandle;
osThreadId TaskTestUART4TXHandle;
osMessageQId Queue_UART4_RXHandle;
osSemaphoreId Sem01_ADCHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC2_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_UART4_Init(void);
void StartDefaultTask(void const * argument);
void start_task_ADC(void const * argument);
void start_task_UART_ADC(void const * argument);
void task_UART4_ISR_Handler(void const * argument);
void TestUART4TX_Handler(void const * argument);

/* USER CODE BEGIN PFP */
uint32_t ADC_result = 0;
char message_UART3_tx [24]; //буффер для отправки данных по UART3
char message_UART4_rx [15];
char message_UART4_rx [15]; //буффер для приёма данных по UART4
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
  MX_ADC2_Init();
  MX_USART3_UART_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of Sem01_ADC */
  osSemaphoreDef(Sem01_ADC);
  Sem01_ADCHandle = osSemaphoreCreate(osSemaphore(Sem01_ADC), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of Queue_UART4_RX */
  osMessageQDef(Queue_UART4_RX, 24, uint8_t);
  Queue_UART4_RXHandle = osMessageCreate(osMessageQ(Queue_UART4_RX), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 512);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of task_ADC */
  osThreadDef(task_ADC, start_task_ADC, osPriorityLow, 0, 256);
  task_ADCHandle = osThreadCreate(osThread(task_ADC), NULL);

  /* definition and creation of task_UART_ADC */
  osThreadDef(task_UART_ADC, start_task_UART_ADC, osPriorityLow, 0, 512);
  task_UART_ADCHandle = osThreadCreate(osThread(task_UART_ADC), NULL);

  /* definition and creation of task_UART4_ISR */
  osThreadDef(task_UART4_ISR, task_UART4_ISR_Handler, osPriorityLow, 0, 128);
  task_UART4_ISRHandle = osThreadCreate(osThread(task_UART4_ISR), NULL);

  /* definition and creation of TaskTestUART4TX */
  osThreadDef(TaskTestUART4TX, TestUART4TX_Handler, osPriorityLow, 0, 128);
  TaskTestUART4TXHandle = osThreadCreate(osThread(TaskTestUART4TX), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
 
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
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 38400;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

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
  huart3.Init.BaudRate = 57600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
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

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin : PD12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//---------------------------------------------------------------
void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart)
{
  if(huart==&huart4)
  {
	osMessagePut (Queue_UART4_RXHandle, message_UART4_rx, 200);
    HAL_UART_Receive_IT(&huart4, (uint8_t*)message_UART4_rx, 1);
//    osMessageQueuePut (Queue_UART4_RXHandle, message_UART4_rx, NULL, 10);
//    xQueueSend( Queue_UART4_RXHandle, (uint8_t*)message_UART4_rx, 10);
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
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	 //osEvent event;
	 HAL_UART_Receive_IT(&huart4, (uint8_t*)message_UART4_rx, 1); //возвращает символ, полученный по UART4
	 osDelay(1);
  }
  /* USER CODE END 5 */ 
}

/* USER CODE BEGIN Header_start_task_ADC */
/**
* @brief Function implementing the task_ADC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_task_ADC */
void start_task_ADC(void const * argument)
{
  /* USER CODE BEGIN start_task_ADC */
  /* Infinite loop */
  for(;;)
  {
	  TickType_t xLastWakeTime; //аргумент хранит значение времени, соответствующее моменту, когда задача была разблокирована в предыдущий раз
	  const TickType_t xFrequency = 300 / portTICK_PERIOD_MS; //аргумент задаёт нужный период (в данном случае 200 мс)
	  xLastWakeTime = xTaskGetTickCount();
	  /* Infinite loop */
	  for(;;)
	  	{
		HAL_ADC_Start(&hadc2); //запустим аналогово-цифровое преобразование
		HAL_ADC_PollForConversion(&hadc2, 100); //дождёмся окончания преобразований
		ADC_result = (uint32_t) HAL_ADC_GetValue(&hadc2);
		HAL_ADC_Stop(&hadc2); //остановим преобразования
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12); //зажгём светодиод
		xSemaphoreGive (Sem01_ADCHandle); //выдадим семафор
//		vTaskDelayUntil (&xLastWakeTime, xFrequency); //функция vTaskDelayUntil() заблокирует задачу до момента времени T = xLastWakeTime + xFrequency
		vTaskDelay(xFrequency);
	  	}
  	}
  /* USER CODE END start_task_ADC */
}

/* USER CODE BEGIN Header_start_task_UART_ADC */
/**
* @brief Function implementing the task_UART_ADC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_start_task_UART_ADC */
void start_task_UART_ADC(void const * argument)
{
  /* USER CODE BEGIN start_task_UART_ADC */
  /* Infinite loop */
  for(;;)
  {
	 if( Sem01_ADCHandle != NULL )
	   {
	   if( xSemaphoreTake( Sem01_ADCHandle, (TickType_t) (100 / portTICK_RATE_MS) ) == pdTRUE ) //подождать 50 тиков, пока семафор станет свободным
	     {
	     sprintf (message_UART3_tx, "resultat = %lu\r\n", ADC_result);
	     HAL_UART_Transmit (&huart3, (unsigned char*)message_UART3_tx, strlen(message_UART3_tx), 0xFFFF);
	     memset (message_UART3_tx, '\0', sizeof (message_UART3_tx)); //очищаем буфер передачи
	     }
/*	   else
	   	   {
		   sprintf (message_UART_tx, "no resultat");
		   HAL_UART_Transmit (&huart3, (unsigned char*)message_UART_tx, strlen(message_UART_tx), 0xFFFF);
		   memset(message_UART_tx, '\0', sizeof(message_UART_tx)); //очищаем буфер передачи
	   	   }*/
	   }
	 osDelay(1);
  }
  /* USER CODE END start_task_UART_ADC */
}

/* USER CODE BEGIN Header_task_UART4_ISR_Handler */
/**
* @brief Function implementing the task_UART4_ISR thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_task_UART4_ISR_Handler */
void task_UART4_ISR_Handler(void const * argument)
{
  /* USER CODE BEGIN task_UART4_ISR_Handler */
  /* Infinite loop */
/*	char char_message_RX;
	osEvent event; //объявим структуру
	uint8_t b;
	static char buffer [24];
	uint8_t count = 0;*/
  for(;;)
  {
/*	  event = osMessageGet(Queue_UART4_RXHandle, 50);
	  if (event.status == osEventMessage)
	  {
		  b= event.value.v;
		  buffer [count] = b;
		  if(b==0x0A)
		  {
		  	HAL_UART_Transmit (&huart3, (unsigned char*)buffer, strlen(buffer), 0xFFFF);
		  	count=0;
		  }
		  else count++;*/

/*	  }
	  if (uxQueueMessagesWaiting(Queue_UART4_RXHandle) != 0)
	  {
	  	xQueueReceive(Queue_UART4_RXHandle, &buffer, 0);
//	  	if ((buffer [count-2] == 0x0D) && (buffer [count-1] == 0x0A))
	  	if (uxQueueMessagesWaiting(Queue_UART4_RXHandle) == 0)
	  		{
	  		HAL_UART_Transmit (&huart3, (unsigned char*)buffer, strlen(buffer), 0xFFFF);
	  		count = 0;
	  		}
	  	else
	  		{
	  		count++;
	  		}*/

//	  xQueueReceive (Queue_UART4_RXHandle, &buffer, 0); //xQueueReceive() используется для приема (чтения) элемента из очереди, при этом элемент удаляется из очереди
/*	  event = osMessageGet (Queue_UART4_RXHandle, 100);
	  if (event.status == osEventMessage) //если произошло событие сообщения
	  	  {
		  b = event.value.v;
		  buffer [count] = b;
		  if ((buffer [count-2] == 0x0D) && (buffer [count-1] == 0x0A))
		  	  {
			  HAL_UART_Transmit (&huart3, (unsigned char*)buffer, strlen(buffer), 0xFFFF);
		  	  count = 0;
		  	  }
	  	  else
	  	  	  {
	  		  count++;
	  	  	  }*/
	  	  }
  /* USER CODE END task_UART4_ISR_Handler */
}

/* USER CODE BEGIN Header_TestUART4TX_Handler */
/**
* @brief Function implementing the TaskTestUART4TX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_TestUART4TX_Handler */
void TestUART4TX_Handler(void const * argument)
{
  /* USER CODE BEGIN TestUART4TX_Handler */
	const TickType_t xFrequency = 1000 / portTICK_PERIOD_MS; //аргумент задаёт нужный период (в данном случае 1000 мс)
	const char buffer_UART4 [] = "test\r\n";
  /* Infinite loop */
  for(;;)
  {

	  HAL_UART_Transmit (&huart4, (unsigned char*)buffer_UART4, strlen(buffer_UART4), 0xFFFF);
	  vTaskDelay(xFrequency);
  }
  /* USER CODE END TestUART4TX_Handler */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
