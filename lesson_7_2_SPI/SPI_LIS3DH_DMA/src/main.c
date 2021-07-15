//SPI. передача и приём данных с LIS3DH. DMA
#include "stm32f4xx.h"
#include "MCU_ini.h"
#include "SPI_ini.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define DELAY 300
uint16_t delay_count = 0;

uint8_t SPI_Data;

#define msg_SIZE 60  //размер массива дл¤ формировани¤ сообщений UART
static char msg [msg_SIZE]; // массив дл¤ формировани¤ сообщений UART

//функци¤ обработки прерывани¤ от таймера SysTick
void SysTick_Handler ()
{
	if (delay_count > 0) //пока значение delay_count больше 0,
		{
		delay_count--; //уменьшаем значение delay_count
		}
}

//функци¤ задержки
void delay_ms (uint16_t delay_temp)
	{
	delay_count = delay_temp;
	while (delay_count) {} //пока значение delay_count не равно 0, продолжаем ждать
	}

//функци¤ передачи данных по UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // отправл¤ем символы строки последовательно, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждЄм выставлени¤ флага "transmission complete"
		USART_SendData(USART2, c); // передаем байт
		}
}

int main(void)
{
	// объ¤влени¤ дл¤ PLL
	uint8_t RCC_PLLM = 16; // предделитель PLL
	uint32_t  RCC_PLLN = 336 ; // умножитель PLL
	uint32_t  RCC_PLLP = 0; // постделитель PLL
	uint8_t CPU_WS = 5; //врем¤ задержки дл¤ работы с Flash пам¤тью
	uint8_t status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //инициализируем генератор PLL
	SystemCoreClockUpdate ();  //обновим системную частоту*/
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	SysTick_Config (SystemCoreClock/1000);  //прерывани¤ раз в 1 мс
	sprintf(msg,"SPI programm with DMA start\r\n");
	UART2_PutRAMString (msg);

	SPI1_ini();
	delay_ms (300);
	DMA_SPI1_ini ();

	while (1)
	{
		Set_SPI_Out(0, 0x8F); //запишем в первый элемент массива адрес запрашиваемого регистра
		Set_SPI_Out (1, 0);  //во вторую половину сообщения запишем '0'
		Start_SPI(2); //запускаем SPI, передадим 2 байта
		SPI_wait();  //ждём окончания получения данных по SPI
		SPI_Data = Get_SPI_In(1);
		if (SPI_Data == 0x3F)
			{
			GPIO_SetBits (GPIOD, GPIO_Pin_15);
			delay_ms (DELAY);
			GPIO_ResetBits (GPIOD, GPIO_Pin_15);
			}
		sprintf (msg, "who i am - %x\r\n",SPI_Data);
		UART2_PutRAMString(msg);
/*		if ((SPI_Data1 == 0x3F) || SPI_Data2 == 0x3F)
			{
			GPIO_SetBits (GPIOD, GPIO_Pin_15);
			delay_ms (DELAY);
			GPIO_ResetBits (GPIOD, GPIO_Pin_15);
			}*/
		GPIO_ResetBits (GPIOD, GPIO_Pin_14);
		GPIO_SetBits (GPIOD, GPIO_Pin_13);
		delay_ms (DELAY);
		GPIO_ResetBits (GPIOD, GPIO_Pin_13);
		GPIO_SetBits (GPIOD, GPIO_Pin_14);
		delay_ms (DELAY);
	}
}
