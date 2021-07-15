// ������ � UART
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // ���������� ������������ �����-������ ��
#define GPIO_AFRH_AF7_UART3 0x77
#define LEDS 4

char flag = 0x1;

/*��������� ���������� �� TIM4*/
void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF; // ������� ���� ���������� �� ������������ �������4;
	flag = flag << 1;
	if (flag == 0x10)
		flag = 0x1;
}

/* ������� �������� ����� ������ �� UART*/
void UART3_TX_byte(char byte)
{
while(!(USART3->SR & USART_SR_TC)) {}; // USART_SR - ������� ������� usart, 6 ��� ���������������, ���� �������� ����� ��������� � ����� TXE ����������.
//0: Transmission is not complete; 1: Transmission is complete;
//���� ����������� ����� if � break
USART3->DR = byte; // �������� ����
}

/*������� �������� �����*/
void UART3_TX_string(char *str)
{
while((*str) != 0x00) // ��������� �� ��������� ������ (������ '\0')
	{
	UART3_TX_byte(*str); // ������� ������� ����
	str++; //��������� �� ��������� ������
	}
}

const char *massiv [LEDS] =	{"led_1","led_2","led_3","led_4"};
char messenge [50];
char symbol = 0x1; // ���������� - ������
char ykaz = 0x1;

int main(void)
{
// ��������� ����� GPIOD
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� ����� D
GPIOD->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1; //���� 8 � 9 ����� D �������������� �� �������������� �����
/*��� ���������������� ����8 � ����� USART3_TX � ����9 � ����� USART3_RX ����� � �������� GPIOD->AFR[1] ������� ���������� AF7 (���.65 datasheet STM32F04xx)
��� ����� � ������� GPIOD->AFR[1] ���������� �������� 0b01110111 ��� 0�77*/
GPIOD->AFR[1] = GPIO_AFRH_AF7_UART3; 
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� D ����� ������

// ��������� UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // ��������� �������� ��������� USART3
USART3->CR1 |= USART_CR1_TE; // ���� ��� �������� ������ ����������
// ��� ������ ��������� ����������� (8N1 - 8 ���, ��� ��������, 1 ���� ���):
USART3->CR1 &= ~USART_CR1_M; // 8 ��� ������, 1 ����� ���
USART3->CR1 &= ~USART_CR1_PCE; // ��� �������� ��������
USART3->CR2 &= ~USART_CR2_STOP; // 1 ����-���
//USART3->CR1 |= USART_CR1_OVER8; // ������� oversampling by 16, ��� �� �������
/* ������ ������������� ��� ��������� ��������: �������� ������� 16 ���. ��������� �������� 57600 ���/�.
USARTDIV = �������� �������/(16 * ��������� ��������) = 16000000/(16*57600) = 17.36
"DIV_Mantissa" = 17 = 0x21; "DIV_Fraction" = 16*0.36 = 5.76; ��������� �� ���������� ������ - 6 = 0x06*/
USART3->BRR = (17 << 4) + 6;  //USART_BRR - Baud rate register; 4-15 ��� - ��������, 0-3 - �������
USART3->CR1 |= USART_CR1_UE; // �������� USART

UART3_TX_string("START!\r\n");
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //�������� ������ 4
TIM4->PSC = 16000-1; // ��������� ������������ �������4
TIM4->ARR = 1000; // �������� ������������
TIM4->DIER |= TIM_DIER_UIE;  //�������� ���������� ��� ���������� (������������) ������� 4
TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ ������� 4
NVIC_EnableIRQ(TIM4_IRQn); //��������� ���������� � ����������� ����������
__enable_irq(); // �������� ���������� ���������

while (1)
{
if (ykaz != flag)
	{
	switch (flag)
		{
		case 0x1: GPIOD->BSRRH |= GPIO_BSRR_BS_15; //��������� 15 ���, BSRRH - ������� �������� �������� BSRR, ������������ ��� ������ ���� � 0
				GPIOD->BSRRL |= GPIO_BSRR_BS_12;  //�������� 12 ���BSRRL - ������� �������� �������� BSRR, ������������ ��� ��������� ���� � 1
				sprintf (messenge, "%s fire!\n\r", massiv [0]);
				UART3_TX_string (messenge);
				break;
		case 0x2: GPIOD->BSRRH |= GPIO_BSRR_BS_12; //��������� 12 ���, BSRRH - ������� �������� �������� BSRR, ������������ ��� ������ ���� � 0
				GPIOD->BSRRL |= GPIO_BSRR_BS_13;  //�������� 13 ���BSRRL - ������� �������� �������� BSRR, ������������ ��� ��������� ���� � 1
				sprintf (messenge, "%s fire!\n\r", massiv [1]);
				UART3_TX_string (messenge);
				break;
		case 0x4: GPIOD->BSRRH |= GPIO_BSRR_BS_13; //��������� 13 ���, BSRRH - ������� �������� �������� BSRR, ������������ ��� ������ ���� � 0
				GPIOD->BSRRL |= GPIO_BSRR_BS_14;  //�������� 14 ���BSRRL - ������� �������� �������� BSRR, ������������ ��� ��������� ���� � 1
				sprintf (messenge, "%s fire!\n\r", massiv [2]);
				UART3_TX_string (messenge);
				break;
		case 0x8: GPIOD->BSRRH |= GPIO_BSRR_BS_14; //��������� 14 ���, BSRRH - ������� �������� �������� BSRR, ������������ ��� ������ ���� � 0
				GPIOD->BSRRL |= GPIO_BSRR_BS_15;  //�������� 15 ���BSRRL - ������� �������� �������� BSRR, ������������ ��� ��������� ���� � 1
				sprintf (messenge, "%s fire!\n\r", massiv [3]);
				UART3_TX_string (messenge);
				break;
		default: break;
		}
	ykaz = flag;
	}
}
}

