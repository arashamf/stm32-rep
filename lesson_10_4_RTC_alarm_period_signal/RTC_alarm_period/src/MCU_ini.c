
#define stm32f4xx
#include "stm32f4xx.h"
#include "MCU_ini.h"

//-----------------функция инициализации пинов МК для светодиодов---------------------//
void LEDS_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOD, ENABLE); //затактируем порт D

	GPIO_InitTypeDef 	GPIO_Init_LED;
	GPIO_Init_LED.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init_LED.GPIO_Mode = GPIO_Mode_OUT; //порт D 12, 13, 14, 15 пин на обычный выход
	GPIO_Init_LED.GPIO_Speed = GPIO_Low_Speed; //скорость порта самая низкая
	GPIO_Init_LED.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_LED.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOD, &GPIO_Init_LED);
}

//-------------------------функция инициализации UART2-------------------------------//
void UART2_ini ()

	{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_UART2;
	GPIO_Init_UART2.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //PA2 - Tx, PA3 - Rx
	GPIO_Init_UART2.GPIO_Mode = GPIO_Mode_AF; //порт А пин 2, 3 на альт. выход
	GPIO_Init_UART2.GPIO_Speed = GPIO_Fast_Speed; //скорость порта высокая
	GPIO_Init_UART2.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_UART2.GPIO_PuPd = GPIO_PuPd_UP; //вход с подтяжкой вверх

	GPIO_Init (GPIOA, &GPIO_Init_UART2);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource2), GPIO_AF_USART2); //установим альт. функцию UART2-TX
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource3), GPIO_AF_USART2); //установим альт. функцию UART2-RX

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE); //затактируем USART2

	USART_InitTypeDef ini_USART2;
	ini_USART2.USART_BaudRate = BAUDRATE; //пропускная способность канала UART
	ini_USART2.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART2.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART2.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART2
	ini_USART2.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART2, &ini_USART2); //запишем настройки USART2

//	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистим флаг

	NVIC_EnableIRQ(USART2_IRQn); //разрешим прерывание от USART2
	__enable_irq ();  //разрешим прерывания глобально
	USART_Cmd(USART2, ENABLE);  //включим USART2
	}

//-------------------------функция инициализации генератора PLL------------------------------------------//
uint8_t PLL_ini (uint8_t CPU_WS, uint8_t RCC_PLLM, uint32_t RCC_PLLN, uint32_t RCC_PLLP)
{
	SET_BIT (RCC->CR, RCC_CR_HSEON); // включаем генератор HSE
	while(!(RCC->CR & RCC_CR_HSERDY)) {}; // ожидаем готовности генератор HSE к работе
	FLASH->ACR |= CPU_WS; // настройка задержки для работы с Flash памятью (RM0090 стр. 81)
	READ_REG(FLASH->ACR); //прочитаем регистр FLASH->ACR
	SET_BIT (RCC->CFGR, RCC_CFGR_SW_1); //10: системная частота с PLL
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE1_1 );// установим делитель шины APB1 - 8 (42 МГц макс. для APB1)
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE2_2 | RCC_CFGR_PPRE2_1 );// установим делитель шины APB2 - 8 (84 МГц макс. для APB2)
	RCC->PLLCFGR = RCC_PLLM + (RCC_PLLN << 6) + (RCC_PLLP << 16);//сдвигаем биты RCC_PLLN и RCC_PLLP, в данном случае: 8МГц*192/8/2=96МГц
	SET_BIT (RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_3); //USB OTG FS clock frequency = VCO frequency / PLLQ, где 2 ≤ PLLQ ≤ 15 (48 МГц макс.)
	SET_BIT (RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE); //установим внешний генератор (HSE) в качестве входного сигнала для PLL
	SET_BIT (RCC->CR, RCC_CR_PLLON); // включаем PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)) {}; // ожидаем готовности PLL к работе
	if (READ_BIT (RCC->CFGR, RCC_CFGR_SWS) == 0x8) //считаем флаг статуса источника тактирования. Если 0x8, то источник PLL
			return 1;
		else
			return 0;
}
/*при тактирование от внешнего генератора HSE необходимо в файле stm32f4xx.h изменить величину #define HSE_VALUE    ((uint32_t)ХХХХХХХ),
где ХХХХХХХ - величина тактовой частоты внешнего генератора (кварца)*/

//------------------------ф-я инициализации ЦАП1 си формирования на его выходе синусоиды-----------------------------//
void DAC_ini ()
{
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_DAC, ENABLE); //затактируем ЦАП

	DAC_InitTypeDef ini_DAC1;
	ini_DAC1.DAC_Trigger = DAC_Trigger_Software; //програмный триггер запуска ЦАП1
	ini_DAC1.DAC_WaveGeneration = DAC_WaveGeneration_None;  //без формирования шума/треугольника
	ini_DAC1.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1; //выбор амплитуды/маски треугольника/шума. В данном случае неважно
	ini_DAC1.DAC_OutputBuffer = DAC_OutputBuffer_Enable; //включим выходной буффер ЦАП1

	DAC_Init(DAC_Channel_1, &ini_DAC1);

	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_DAC; //настроим выходы для ЦАП1
	GPIO_Init_DAC.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; //PA4 - DAC_OUT1, PA5 - DAC_OUT2
	GPIO_Init_DAC.GPIO_Mode =  GPIO_Mode_AN; //порт А пин 4, 5 в аналоговый режим
	GPIO_Init_DAC.GPIO_Speed = GPIO_Low_Speed; //скорость порта низкая
	GPIO_Init_DAC.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull) (неважно)
	GPIO_Init_DAC.GPIO_PuPd =  GPIO_PuPd_NOPULL; //вход без подтяжки (неважно)

	GPIO_Init (GPIOA, &GPIO_Init_DAC);

	DAC_Cmd (DAC_Channel_1, ENABLE); //запустим ЦАП1
}
