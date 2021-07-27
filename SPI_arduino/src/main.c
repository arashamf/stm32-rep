/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f4xx.h"
#include "stdio.h"
#include "MCU_ini.h"
#include "SPI_lib.h"
#include "UART_lib.h"
#include "delay_lib.h"

#define msg_SIZE 50 //размер массива дл¤ формировани¤ сообщений дл¤ отправки по UART
char msg_UART_Tx [msg_SIZE]; // массив дл¤ формировани¤ сообщений дл¤ отправки по UART

uint8_t SPI_data = 0;
uint8_t point = 0xCA;

int main(void)
{

	// объявления для PLL
	uint8_t RCC_PLLM = 8; // предделитель PLL
	uint32_t  RCC_PLLN = 192 ; // умножитель PLL
	uint32_t  RCC_PLLP = 2; // постделитель PLL
	uint32_t RCC_PLLQ = 7;
	uint8_t CPU_WS = 2; //время задержки для работы с Flash памятью
	uint8_t status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP, RCC_PLLQ); //инициализируем генератор PLL

	LEDS_ini (); //инициализируем LED
	GPIO_SetBits (LED_Port, LED3_Pin);
	GPIO_ResetBits(LED_Port, LED4_Pin);
	GPIO_ResetBits(LED_Port, LED5_Pin);
	GPIO_ResetBits(LED_Port, LED6_Pin);

	UART2_ini_spl (57600); //инициализируем UART1
	if (status_PLL)
		UART2_PutString("init PLL ok\r\n");
	delay_us (100);

	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
	sprintf (msg_UART_Tx, "%lu, %lu, %lu, %lu\r\n", RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency, RCC_Clocks.PCLK2_Frequency);
	UART2_PutString (msg_UART_Tx);

	SPI2_ini ();

	UART2_PutString ("Arduino_connect\r\n");

	while (1)
	{
		GPIO_ToggleBits (LED_Port, LED4_Pin);
		GPIO_ToggleBits (LED_Port, LED3_Pin);
		CS_ON;
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};  //ожидаем пока буфер передачи не пуст
		SPI_I2S_SendData (SPI2, point);
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}; //ожидаем пока буфер приёма пуст
		SPI_data = (uint8_t) SPI_I2S_ReceiveData (SPI2);
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {};  //ожидаем пока буфер передачи не пуст
		SPI_I2S_SendData (SPI2, point);
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}; //ожидаем пока буфер передачи не пуст
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) {}; //ожидаем пока буфер приёма пуст
		(void) SPI_I2S_ReceiveData (SPI2);
		delay_us (10);
		CS_OFF;
		sprintf (msg_UART_Tx, "data = %u\r\n", SPI_data);
		UART2_PutString (msg_UART_Tx);
		delay_ms (1500);
	}

}
