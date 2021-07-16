/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f10x.h"
#include "ini_MCU.h"
#include "stdio.h"
#include "delay.h"

#define msg_SIZE 60
static char msg_UART_tx [msg_SIZE]; // массив для формирования сообщений для вывода по UART

RCC_ClocksTypeDef RCC_freq;

//---------------------------------функция передачи данных по UART2----------------------------------------//
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставления флага "transmission complete"
		USART_SendData(USART2, c); // передаем байт
		}
}

int main(void)
{
	uint8_t RCC_PLLMUL = 6; // умножитель частоты PLL
	uint8_t CPU_WS = 1; //врем¤ задержки для работы с Flash памятью (для 32 МГц равно 1)
	uint8_t status_PLL = 0; //флаг готовности PLL
	status_PLL = PLL_ini (CPU_WS, RCC_PLLMUL); //настройка генератора PLL (тактирование от HSE = 8MHz, SYSCLK = 32MHz, APB1=APB2=8MHz)
	SystemCoreClockUpdate ();  //обновим системную частоту
	LEDS_ini ();
	UART2_ini ();
	UART2_PutRAMString("start\r\n");
	if (status_PLL)
		UART2_PutRAMString("PLL ok!\r\n");
	else
		UART2_PutRAMString("PLL error!\r\n");
	RCC_GetClocksFreq(&RCC_freq);
	sprintf (msg_UART_tx, "SYSCLK=%ld, HCLK=%ld, PCLK1=%ld, PCLK2=%ld\r\n", RCC_freq.SYSCLK_Frequency, RCC_freq.HCLK_Frequency,
				RCC_freq.PCLK1_Frequency, RCC_freq.PCLK2_Frequency);
	UART2_PutRAMString(msg_UART_tx);

	while (1)
	{
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET);
	delay_ms(100);
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
	UART2_PutRAMString("ok\r\n");
	delay_ms(100);
	}
}
