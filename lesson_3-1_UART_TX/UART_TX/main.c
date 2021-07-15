// Работа с UART
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // библиотека стандартного ввода-вывода си
#define GPIO_AFRH_AF7_UART3 0x77
#define LEDS 4

char flag = 0x1;

/*обработка прерываний от TIM4*/
void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания по переполнению таймера4;
	flag = flag << 1;
	if (flag == 0x10)
		flag = 0x1;
}

/* функция передачи байта данных по UART*/
void UART3_TX_byte(char byte)
{
while(!(USART3->SR & USART_SR_TC)) {}; // USART_SR - регистр статуса usart, 6 бит устанавливается, если передача кадра завершена и режим TXE установлен.
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

const char *massiv [LEDS] =	{"led_1","led_2","led_3","led_4"};
char messenge [50];
char symbol = 0x1; // переменная - символ
char ykaz = 0x1;

int main(void)
{
// настройка порта GPIOD
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // запускаем тактовый генератор порта D
GPIOD->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1; //пины 8 и 9 порта D сконфигурируем на альтернативный режим
/*для конфигурирования пина8 в режим USART3_TX и пина9 в режим USART3_RX нужно в регистре GPIOD->AFR[1] выбрать комбинацию AF7 (стр.65 datasheet STM32F04xx)
для этого в регистр GPIOD->AFR[1] необходимо записать 0b01110111 или 0х77*/
GPIOD->AFR[1] = GPIO_AFRH_AF7_UART3; 
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта D самая низкая

// настройка UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // запускаем тактовый генератор USART3
USART3->CR1 |= USART_CR1_TE; // этот бит включает только передатчик
// при сбросе параметры установлены (8N1 - 8 бит, без паритета, 1 стоп бит):
USART3->CR1 &= ~USART_CR1_M; // 8 бит данные, 1 старт бит
USART3->CR1 &= ~USART_CR1_PCE; // без проверки чётности
USART3->CR2 &= ~USART_CR2_STOP; // 1 стоп-бит
//USART3->CR1 |= USART_CR1_OVER8; // включён oversampling by 16, это не понятно
/* расчет коэффициентов для требуемой скорости: тактовая частота 16 МГц. требуемая скорость 57600 бод/с.
USARTDIV = тактовая частота/(16 * требуемая скорость) = 16000000/(16*57600) = 17.36
"DIV_Mantissa" = 17 = 0x21; "DIV_Fraction" = 16*0.36 = 5.76; округляем до ближайшего целого - 6 = 0x06*/
USART3->BRR = (17 << 4) + 6;  //USART_BRR - Baud rate register; 4-15 бит - мантисса, 0-3 - дробная
USART3->CR1 |= USART_CR1_UE; // включаем USART

UART3_TX_string("START!\r\n");
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //включаем таймер 4
TIM4->PSC = 16000-1; // установка предделителя таймера4
TIM4->ARR = 1000; // значение перезагрузки
TIM4->DIER |= TIM_DIER_UIE;  //разрешим прерывание при обновлении (переполнении) таймера 4
TIM4->CR1 |= TIM_CR1_CEN; // разрешим работу таймера 4
NVIC_EnableIRQ(TIM4_IRQn); //разрешаем прерывание в контроллере прерываний
__enable_irq(); // разрешим прерывания глобально

while (1)
{
if (ykaz != flag)
	{
	switch (flag)
		{
		case 0x1: GPIOD->BSRRH |= GPIO_BSRR_BS_15; //выключаем 15 пин, BSRRH - старшая половина регистра BSRR, импользуется для сброса бита в 0
				GPIOD->BSRRL |= GPIO_BSRR_BS_12;  //включаем 12 пинBSRRL - младшая половина регистра BSRR, используется для установки бита в 1
				sprintf (messenge, "%s fire!\n\r", massiv [0]);
				UART3_TX_string (messenge);
				break;
		case 0x2: GPIOD->BSRRH |= GPIO_BSRR_BS_12; //выключаем 12 пин, BSRRH - старшая половина регистра BSRR, импользуется для сброса бита в 0
				GPIOD->BSRRL |= GPIO_BSRR_BS_13;  //включаем 13 пинBSRRL - младшая половина регистра BSRR, используется для установки бита в 1
				sprintf (messenge, "%s fire!\n\r", massiv [1]);
				UART3_TX_string (messenge);
				break;
		case 0x4: GPIOD->BSRRH |= GPIO_BSRR_BS_13; //выключаем 13 пин, BSRRH - старшая половина регистра BSRR, импользуется для сброса бита в 0
				GPIOD->BSRRL |= GPIO_BSRR_BS_14;  //включаем 14 пинBSRRL - младшая половина регистра BSRR, используется для установки бита в 1
				sprintf (messenge, "%s fire!\n\r", massiv [2]);
				UART3_TX_string (messenge);
				break;
		case 0x8: GPIOD->BSRRH |= GPIO_BSRR_BS_14; //выключаем 14 пин, BSRRH - старшая половина регистра BSRR, импользуется для сброса бита в 0
				GPIOD->BSRRL |= GPIO_BSRR_BS_15;  //включаем 15 пинBSRRL - младшая половина регистра BSRR, используется для установки бита в 1
				sprintf (messenge, "%s fire!\n\r", massiv [3]);
				UART3_TX_string (messenge);
				break;
		default: break;
		}
	ykaz = flag;
	}
}
}

