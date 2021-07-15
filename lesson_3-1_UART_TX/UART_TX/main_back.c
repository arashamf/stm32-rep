// ������ � UART
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // ���������� ������������ �����-������ ��
#define GPIO_AFRH_AF7_UART3 0x77

/* ������� �������� ����� ������ �� UART*/
void UART3_TX_byte(char byte)
{
while(!(USART3->SR & USART_SR_TC)) {}; // USART_SR - ������� ������� usart, 6 ��� ���������������, ���� �������� ����� ��������� � ���� TXE ����������.
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

void UART3_TX_byte(char byte)

char msg[100];
char symbol = 0x1; // ���������� - ������

int main(void)
{
unsigned int i;
// ��������� ����� GPIOD
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� ����� D
GPIOD->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1; //���� 8 � 9 ����� D �������������� �� �������������� �����
/*��� ���������������� ����8 � ����� USART3_TX � ����9 � ����� USART3_RX ����� � �������� GPIOD->AFR[1] ������� ���������� AF7 (���.64 datasheet STM32F04xx)
��� ����� � ������� GPIOD->AFR[1] ���������� �������� 0b01110111 ��� 0�77*/
GPIOD->AFR[1] = GPIO_AFRH_AF7_UART3; // ����������� ����� �� �������������� �������

// ��������� UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // ��������� �������� ��������� USART3
USART3->CR1 |= USART_CR1_TE; // ���� ��� �������� ������ ����������
// ��� ������ ��������� ����������� (8N1 - 8 ���, ��� ��������, 1 ���� ���):
USART3->CR1 &= ~USART_CR1_M; // 8 ��� ������, 1 ����� ���
USART3->CR1 &= ~USART_CR1_PCE; // ��� �������� ��������
USART3->CR2 &= ~USART_CR2_STOP; // 1 ����-���
/* ������ ������������� ��� ��������� ��������: �������� ������� 16 ���. ��������� �������� 57600 ���/�.
USART3->CR1 |= USART_CR1_OVER8; // ������� oversampling by 16, ��� �� �������
USARTDIV = �������� �������/(16 * ��������� ��������) = 16000000/(16*57600) = 17.36
"DIV_Mantissa" = 17 = 0x21; "DIV_Fraction" = 16*0.36 = 5.76; ��������� �� ���������� ������ - 6 = 0x06*/
USART3->BRR = (17 << 4) + 6;  //USART_BRR - Baud rate register; 4-15 ��� - ��������, 0-3 - �������
USART3->CR1 |= USART_CR1_UE; // �������� USART

UART3_TX_string("Hello, Programmer!\r\n");

while (1)
{
sprintf(msg,"symbol = '%c', Ascii code = %d\r\n",symbol, symbol); // �������� � ������ msg ������
UART3_TX_string(msg);
symbol++;
if (symbol == 27)
	symbol = 29;
if (symbol == 128)
	symbol = 0x1;
for(i = 0; i < 0xFFFFF; i++) {} // ��������
}
}
