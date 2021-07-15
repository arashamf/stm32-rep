//SPI. �������� � ���� ������ � LIS3DH. DMA
#include "stm32f4xx.h"
#include "MCU_ini.h"
#include "SPI_ini.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define DELAY 300
uint16_t delay_count = 0;

uint8_t SPI_Data;

#define msg_SIZE 60  //������ ������� �� ����������� ��������� UART
static char msg [msg_SIZE]; // ������ �� ����������� ��������� UART

//������ ��������� ��������� �� ������� SysTick
void SysTick_Handler ()
{
	if (delay_count > 0) //���� �������� delay_count ������ 0,
		{
		delay_count--; //��������� �������� delay_count
		}
}

//������ ��������
void delay_ms (uint16_t delay_temp)
	{
	delay_count = delay_temp;
	while (delay_count) {} //���� �������� delay_count �� ����� 0, ���������� �����
	}

//������ �������� ������ �� UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // ��������� ������� ������ ���������������, ���� �� ��������� ������ ����� ������ "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //��� ���������� ����� "transmission complete"
		USART_SendData(USART2, c); // �������� ����
		}
}

int main(void)
{
	// ��������� �� PLL
	uint8_t RCC_PLLM = 16; // ������������ PLL
	uint32_t  RCC_PLLN = 336 ; // ���������� PLL
	uint32_t  RCC_PLLP = 0; // ������������ PLL
	uint8_t CPU_WS = 5; //���� �������� �� ������ � Flash ������
	uint8_t status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //�������������� ��������� PLL
	SystemCoreClockUpdate ();  //������� ��������� �������*/
	LEDS_ini (); //�������������� LEDs
	UART2_ini (); //�������������� UART2
	SysTick_Config (SystemCoreClock/1000);  //��������� ��� � 1 ��
	sprintf(msg,"SPI programm with DMA start\r\n");
	UART2_PutRAMString (msg);

	SPI1_ini();
	delay_ms (300);
	DMA_SPI1_ini ();

	while (1)
	{
		Set_SPI_Out(0, 0x8F); //������� � ������ ������� ������� ����� �������������� ��������
		Set_SPI_Out (1, 0);  //�� ������ �������� ��������� ������� '0'
		Start_SPI(2); //��������� SPI, ��������� 2 �����
		SPI_wait();  //��� ��������� ��������� ������ �� SPI
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
