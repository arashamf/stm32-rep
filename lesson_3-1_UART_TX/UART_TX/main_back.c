// Работа с UART
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // библиотека стандартного ввода-вывода си
#define GPIO_AFRH_AF7_UART3 0x77

/* функция передачи байта данных по UART*/
void UART3_TX_byte(char byte)
{
while(!(USART3->SR & USART_SR_TC)) {}; // USART_SR - регистр статуса usart, 6 бит устанавливается, если передача кадра завершена и если TXE установлен.
//0: Transmission is not complete; 1: Transmission is complete;
//надо попробывать через if и break
USART3->DR = byte; // передаем байт
}

/*функция передачи строк*/
void UART3_TX_string(char *str)
{
while((*str) != 0x00) // проверяем на окончание строки (символ '\0')
	{
	UART3_TX_byte(*str); // выводим текущий байт
	str++; //переходим на следующий символ
	}
}

void UART3_TX_byte(char byte)

char msg[100];
char symbol = 0x1; // переменная - символ

int main(void)
{
unsigned int i;
// настройка порта GPIOD
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // запускаем тактовый генератор порта D
GPIOD->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1; //пины 8 и 9 порта D сконфигурируем на альтернативный режим
/*для конфигурирования пина8 в режим USART3_TX и пина9 в режим USART3_RX нужно в регистре GPIOD->AFR[1] выбрать комбинацию AF7 (стр.64 datasheet STM32F04xx)
для этого в регистр GPIOD->AFR[1] необходимо записать 0b01110111 или 0х77*/
GPIOD->AFR[1] = GPIO_AFRH_AF7_UART3; // настраиваем вывод на альтернативную функцию

// настройка UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // запускаем тактовый генератор USART3
USART3->CR1 |= USART_CR1_TE; // этот бит включает только передатчик
// при сбросе параметры установлены (8N1 - 8 бит, без паритета, 1 стоп бит):
USART3->CR1 &= ~USART_CR1_M; // 8 бит данные, 1 старт бит
USART3->CR1 &= ~USART_CR1_PCE; // без проверки чётности
USART3->CR2 &= ~USART_CR2_STOP; // 1 стоп-бит
/* расчет коэффициентов для требуемой скорости: тактовая частота 16 МГц. требуемая скорость 57600 бод/с.
USART3->CR1 |= USART_CR1_OVER8; // включён oversampling by 16, это не понятно
USARTDIV = тактовая частота/(16 * требуемая скорость) = 16000000/(16*57600) = 17.36
"DIV_Mantissa" = 17 = 0x21; "DIV_Fraction" = 16*0.36 = 5.76; округляем до ближайшего целого - 6 = 0x06*/
USART3->BRR = (17 << 4) + 6;  //USART_BRR - Baud rate register; 4-15 бит - мантисса, 0-3 - дробная
USART3->CR1 |= USART_CR1_UE; // включаем USART

UART3_TX_string("Hello, Programmer!\r\n");

while (1)
{
sprintf(msg,"symbol = '%c', Ascii code = %d\r\n",symbol, symbol); // записать в массив msg строку
UART3_TX_string(msg);
symbol++;
if (symbol == 27)
	symbol = 29;
if (symbol == 128)
	symbol = 0x1;
for(i = 0; i < 0xFFFFF; i++) {} // задержка
}
}
