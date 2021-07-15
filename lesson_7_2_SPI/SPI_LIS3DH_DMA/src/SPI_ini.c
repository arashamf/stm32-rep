#include "stm32f4xx.h"
#include "SPI_ini.h"

#define BUFFER_SIZE 8
uint8_t buffer_TX_SPI [BUFFER_SIZE];  //массив байтов для передачи по SPI
uint8_t buffer_RX_SPI [BUFFER_SIZE]; //массив полученных по SPI данных

uint8_t SPI_work=0;  //флаг включения/отключения SPI

//---------------функция запуска SPI---------------------------------------//
void Start_SPI(uint8_t number)
{
	DMA_SetCurrDataCounter(DMA2_Stream0, number); /*Записывает количество единиц данных, которые будут переданы в DMAy Streamx.
Counter: количество единиц данных, подлежащих передаче (16 бит). Количество элементов данных зависит только от формата периферийных данных.
Если формат периферийных данных байт: количество единиц данных равно общему количеству передаваемых байт.*/
	DMA_SetCurrDataCounter(DMA2_Stream3, number);

	SPI_work = 1; //установим флаг работы SPI
	CS_ON(); //активируем chip select
	DMA_Cmd(DMA2_Stream0, ENABLE); //включим поток 0 DMA2
	DMA_Cmd(DMA2_Stream3, ENABLE); //включим поток 3 DMA2
}

//-------функция ожидания окончания передачи SPI---------------------------//
void SPI_wait()
{
	while (SPI_work) {} //ожидание установки флага работы SPI в 1 (прерывание)
}

//----------------функция возврата, полученных данных по SPI--------------//
uint8_t Get_SPI_In(uint8_t Adr)
{
	return buffer_RX_SPI[Adr]; //возвращаем требуемый элемент массива, полученных по SPI данных
}

//------------------функция передачи данных в SPI-----------------------//
void Set_SPI_Out(uint8_t Adress, uint8_t Data)
{
	if (Adress < BUFFER_SIZE) //если адрес элемента указан корректно
	{
		buffer_TX_SPI [Adress] = Data; //записываем в требуемый элемент массива данные, для передачи по SPI
	}
}

//-------------------------функция инициализации SPI1-------------------------------//
void SPI1_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_ini_SPI_0;
	GPIO_ini_SPI_0.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //выберем пин 5-7 порта А для настройки
	GPIO_ini_SPI_0.GPIO_Mode = GPIO_Mode_AF; //порт А пин 5-7 на альт. выход
	GPIO_ini_SPI_0.GPIO_Speed = GPIO_Medium_Speed; //скорость порта
	GPIO_ini_SPI_0.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_ini_SPI_0.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOA, &GPIO_ini_SPI_0);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource5), GPIO_AF_SPI1); //зададим конкретный альт. режим - SPI1
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource6), GPIO_AF_SPI1);
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource7), GPIO_AF_SPI1);

	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOE, ENABLE); //затактируем порт Е

	GPIO_InitTypeDef 	GPIO_ini_SPI_1;
	GPIO_ini_SPI_1.GPIO_Pin = GPIO_Pin_3; //выберем пин 3 порта E для настройки
	GPIO_ini_SPI_1.GPIO_Mode = GPIO_Mode_OUT; //порт E пин 3 на выход
	GPIO_ini_SPI_1.GPIO_Speed = GPIO_Medium_Speed; //скорость порта
	GPIO_ini_SPI_1.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_ini_SPI_1.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOE, &GPIO_ini_SPI_1);

	CS_OFF(); //отключим chip select

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); // запускаем тактовый генератор SPI1

	SPI_InitTypeDef		SPI_Init_user;
	SPI_Init_user.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //по двум линиям в полнодуплексном режиме
	SPI_Init_user.SPI_Mode = SPI_Mode_Master; //мк в режиме мастер
	SPI_Init_user.SPI_DataSize = SPI_DataSize_8b; //8 бит данные
	SPI_Init_user.SPI_CPOL = SPI_CPOL_High; //Если бит CPOL установлен, вывод SCK имеет состояние высокого уровня при простое
	//SPI_Init_user.SPI_CPOL = SPI_CPOL_Low;
	//SPI_Init_user.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_Init_user.SPI_CPHA = SPI_CPHA_2Edge; //Если установлен бит CPHA , второй край на выводе SCK (спадающий фрон, если бит CPOL сброшен, восходящий край, если бит CPOL установлен) является стробом захвата MSBit.
	SPI_Init_user.SPI_NSS = SPI_NSS_Soft;  // программное управление выводом NSS
	SPI_Init_user.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //зададим делитель частоты шины
	SPI_Init_user.SPI_FirstBit = SPI_FirstBit_MSB; // первый идет старший бит
	//SPI_Init_user.SPI_FirstBit = SPI_FirstBit_LSB;
	SPI_Init_user.SPI_CRCPolynomial = 7;



	SPI_Init(SPI1, &SPI_Init_user);

	SPI_Cmd(SPI1, ENABLE); //включим SPI1
}

//-------------------------функция инициализации DM2 для SPI1-------------------------------//
void DMA_SPI1_ini ()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	//----------------------инициализция DMA2 STREAM3 для SPI1_TX----------------------------------------------------------------------------------//
	DMA_InitTypeDef ini_DMA2_SPI;
	ini_DMA2_SPI.DMA_Channel = DMA_Channel_3; //номер канала
	ini_DMA2_SPI.DMA_PeripheralBaseAddr = (uint32_t) &(SPI1->DR); //указатель на регистр памяти перифирии: буффер SPI1
	ini_DMA2_SPI.DMA_Memory0BaseAddr = (uint32_t) buffer_TX_SPI; //указатель на массив передаваемых данных; указатель на массив есть название массива
	ini_DMA2_SPI.DMA_DIR = DMA_DIR_MemoryToPeripheral; //направление копирования, из памяти в периферию
	ini_DMA2_SPI.DMA_BufferSize = 2; //размер буффера DMA1 в единице данных указанного потока. Единица данных, задается настройкой DMA_PeripheralDataSize или DMA_MemoryDataSize.
	ini_DMA2_SPI.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //без увеличения (инкремента) указателя на регистр памяти перифирии
	ini_DMA2_SPI.DMA_MemoryInc = DMA_MemoryInc_Enable;  //увеличение (инкремент) указателя на массив передаваемых данных
	ini_DMA2_SPI.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //величина на которую увеличивается указатель на регистр данных SPI1 - 1byte
	ini_DMA2_SPI.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //величина на которую увеличивается указатель на массив передаваемых данных - 1byte
	ini_DMA2_SPI.DMA_Mode = DMA_Mode_Normal; //нормальный режим передачи данных DMA, круговой режим выключен
	ini_DMA2_SPI.DMA_Priority = DMA_Priority_High; //приоритет DMA1 высокий
	ini_DMA2_SPI.DMA_FIFOMode = DMA_FIFOMode_Disable; //режим FIFO отключен
	ini_DMA2_SPI.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull; //Выбор порога FIFO, в данном случае не важен
	ini_DMA2_SPI.DMA_MemoryBurst = DMA_MemoryBurst_Single; //конфигурация передачи пакетов памяти: пакетная передача отключена
	ini_DMA2_SPI.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //конфигурация передачи пакетов перефирии: пакетная передача отключена

	DMA_Init (DMA2_Stream3, &ini_DMA2_SPI); //инициализация DMA2 Stream3 (SPI1_TX)
	SPI_I2S_DMACmd (SPI1, SPI_I2S_DMAReq_Tx, ENABLE); //включаем интерфейс DMA SPI1_TX

	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE); //включаем прерывание при полной передачи потока 3 DMA2
	NVIC_EnableIRQ (DMA2_Stream3_IRQn); //разрешим прерывание от потока 3 DMA2

	//-----------------------------инициализция DMA2 STREAM0 для SPI1_RX-------------------------------------------------------------------//
	ini_DMA2_SPI.DMA_Channel = DMA_Channel_3; //номер канала
	ini_DMA2_SPI.DMA_PeripheralBaseAddr = (uint32_t) &(SPI1->DR); //указатель на регистр памяти перифирии: буффер SPI1
	ini_DMA2_SPI.DMA_Memory0BaseAddr = (uint32_t) buffer_RX_SPI; //указатель на массив передаваемых данных; указатель на массив есть название массива
	ini_DMA2_SPI.DMA_DIR = DMA_DIR_PeripheralToMemory; //направление копирования, из периферии в память
	ini_DMA2_SPI.DMA_BufferSize = 2; //размер буффера DMA1
	ini_DMA2_SPI.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //без увеличения (инкремента) указателя на регистр памяти перифирии
	ini_DMA2_SPI.DMA_MemoryInc = DMA_MemoryInc_Enable;  //увеличение (инкремент) указателя на массив передаваемых данных
	ini_DMA2_SPI.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //величина на которую увеличивается указатель на регистр данных SPI1 - 1byte
	ini_DMA2_SPI.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //величина на которую увеличивается указатель на массив передаваемых данных - 1byte
	ini_DMA2_SPI.DMA_Mode = DMA_Mode_Normal; //нормальный режим передачи данных DMA, круговой режим выключен
	ini_DMA2_SPI.DMA_Priority = DMA_Priority_High; //приоритет DMA1 высокий
	ini_DMA2_SPI.DMA_FIFOMode = DMA_FIFOMode_Disable; //режим FIFO отключен
	ini_DMA2_SPI.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull; //Выбор порога FIFO, в данном случае не важен
	ini_DMA2_SPI.DMA_MemoryBurst = DMA_MemoryBurst_Single; //конфигурация передачи пакетов памяти: пакетная передача отключена
	ini_DMA2_SPI.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //конфигурация передачи пакетов перефирии: пакетная передача отключена

	DMA_Init (DMA2_Stream0, &ini_DMA2_SPI); //инициализация DMA2 Stream0 (SPI1_RX)
	SPI_I2S_DMACmd (SPI1, SPI_I2S_DMAReq_Rx, ENABLE); //включаем интерфейс DMA SPI1_RX

	DMA_ITConfig (DMA2_Stream0, DMA_IT_TC, ENABLE); //включаем прерывание при полной передачи потока 0 DMA2
	NVIC_EnableIRQ (DMA2_Stream0_IRQn); //разрешим прерывание от потока 0 DMA2
}

//----------------функция обработки прерывания от DMA2 потока 0 при получении данных (RX) по SPI1-------------------//
void DMA2_Stream0_IRQHandler(void)
{

	if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) == SET) //если флаг окончания передачи TCIF0 установлен
	{
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0); //сбросим флаг TCIF0
		CS_OFF(); //отключим chip select
		SPI_work = 0; //сбросим флаг работы SPI
	}
}

//--------------функция обработки прерывания от DMA2 потока 0 при передачи данных (TX) по SPI1-------------------//
void DMA2_Stream3_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3) == SET) //если флаг окончания передачи TCIF3 установлен
	{
		DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3); //сбросим флаг TCIF3
	}
}

