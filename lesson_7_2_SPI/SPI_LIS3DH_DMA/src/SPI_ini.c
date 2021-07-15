#include "stm32f4xx.h"
#include "SPI_ini.h"

#define BUFFER_SIZE 8
uint8_t buffer_TX_SPI [BUFFER_SIZE];  //������ ������ ��� �������� �� SPI
uint8_t buffer_RX_SPI [BUFFER_SIZE]; //������ ���������� �� SPI ������

uint8_t SPI_work=0;  //���� ���������/���������� SPI

//---------------������� ������� SPI---------------------------------------//
void Start_SPI(uint8_t number)
{
	DMA_SetCurrDataCounter(DMA2_Stream0, number); /*���������� ���������� ������ ������, ������� ����� �������� � DMAy Streamx.
Counter: ���������� ������ ������, ���������� �������� (16 ���). ���������� ��������� ������ ������� ������ �� ������� ������������ ������.
���� ������ ������������ ������ ����: ���������� ������ ������ ����� ������ ���������� ������������ ����.*/
	DMA_SetCurrDataCounter(DMA2_Stream3, number);

	SPI_work = 1; //��������� ���� ������ SPI
	CS_ON(); //���������� chip select
	DMA_Cmd(DMA2_Stream0, ENABLE); //������� ����� 0 DMA2
	DMA_Cmd(DMA2_Stream3, ENABLE); //������� ����� 3 DMA2
}

//-------������� �������� ��������� �������� SPI---------------------------//
void SPI_wait()
{
	while (SPI_work) {} //�������� ��������� ����� ������ SPI � 1 (����������)
}

//----------------������� ��������, ���������� ������ �� SPI--------------//
uint8_t Get_SPI_In(uint8_t Adr)
{
	return buffer_RX_SPI[Adr]; //���������� ��������� ������� �������, ���������� �� SPI ������
}

//------------------������� �������� ������ � SPI-----------------------//
void Set_SPI_Out(uint8_t Adress, uint8_t Data)
{
	if (Adress < BUFFER_SIZE) //���� ����� �������� ������ ���������
	{
		buffer_TX_SPI [Adress] = Data; //���������� � ��������� ������� ������� ������, ��� �������� �� SPI
	}
}

//-------------------------������� ������������� SPI1-------------------------------//
void SPI1_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //����������� ���� �

	GPIO_InitTypeDef 	GPIO_ini_SPI_0;
	GPIO_ini_SPI_0.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; //������� ��� 5-7 ����� � ��� ���������
	GPIO_ini_SPI_0.GPIO_Mode = GPIO_Mode_AF; //���� � ��� 5-7 �� ����. �����
	GPIO_ini_SPI_0.GPIO_Speed = GPIO_Medium_Speed; //�������� �����
	GPIO_ini_SPI_0.GPIO_OType = GPIO_OType_PP; //����� � ��������� (push-pull)
	GPIO_ini_SPI_0.GPIO_PuPd = GPIO_PuPd_NOPULL; //���� ��� ��������

	GPIO_Init (GPIOA, &GPIO_ini_SPI_0);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource5), GPIO_AF_SPI1); //������� ���������� ����. ����� - SPI1
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource6), GPIO_AF_SPI1);
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource7), GPIO_AF_SPI1);

	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOE, ENABLE); //����������� ���� �

	GPIO_InitTypeDef 	GPIO_ini_SPI_1;
	GPIO_ini_SPI_1.GPIO_Pin = GPIO_Pin_3; //������� ��� 3 ����� E ��� ���������
	GPIO_ini_SPI_1.GPIO_Mode = GPIO_Mode_OUT; //���� E ��� 3 �� �����
	GPIO_ini_SPI_1.GPIO_Speed = GPIO_Medium_Speed; //�������� �����
	GPIO_ini_SPI_1.GPIO_OType = GPIO_OType_PP; //����� � ��������� (push-pull)
	GPIO_ini_SPI_1.GPIO_PuPd = GPIO_PuPd_NOPULL; //���� ��� ��������

	GPIO_Init (GPIOE, &GPIO_ini_SPI_1);

	CS_OFF(); //�������� chip select

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); // ��������� �������� ��������� SPI1

	SPI_InitTypeDef		SPI_Init_user;
	SPI_Init_user.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //�� ���� ������ � ��������������� ������
	SPI_Init_user.SPI_Mode = SPI_Mode_Master; //�� � ������ ������
	SPI_Init_user.SPI_DataSize = SPI_DataSize_8b; //8 ��� ������
	SPI_Init_user.SPI_CPOL = SPI_CPOL_High; //���� ��� CPOL ����������, ����� SCK ����� ��������� �������� ������ ��� �������
	//SPI_Init_user.SPI_CPOL = SPI_CPOL_Low;
	//SPI_Init_user.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_Init_user.SPI_CPHA = SPI_CPHA_2Edge; //���� ���������� ��� CPHA , ������ ���� �� ������ SCK (��������� ����, ���� ��� CPOL �������, ���������� ����, ���� ��� CPOL ����������) �������� ������� ������� MSBit.
	SPI_Init_user.SPI_NSS = SPI_NSS_Soft;  // ����������� ���������� ������� NSS
	SPI_Init_user.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //������� �������� ������� ����
	SPI_Init_user.SPI_FirstBit = SPI_FirstBit_MSB; // ������ ���� ������� ���
	//SPI_Init_user.SPI_FirstBit = SPI_FirstBit_LSB;
	SPI_Init_user.SPI_CRCPolynomial = 7;



	SPI_Init(SPI1, &SPI_Init_user);

	SPI_Cmd(SPI1, ENABLE); //������� SPI1
}

//-------------------------������� ������������� DM2 ��� SPI1-------------------------------//
void DMA_SPI1_ini ()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	//----------------------������������ DMA2 STREAM3 ��� SPI1_TX----------------------------------------------------------------------------------//
	DMA_InitTypeDef ini_DMA2_SPI;
	ini_DMA2_SPI.DMA_Channel = DMA_Channel_3; //����� ������
	ini_DMA2_SPI.DMA_PeripheralBaseAddr = (uint32_t) &(SPI1->DR); //��������� �� ������� ������ ���������: ������ SPI1
	ini_DMA2_SPI.DMA_Memory0BaseAddr = (uint32_t) buffer_TX_SPI; //��������� �� ������ ������������ ������; ��������� �� ������ ���� �������� �������
	ini_DMA2_SPI.DMA_DIR = DMA_DIR_MemoryToPeripheral; //����������� �����������, �� ������ � ���������
	ini_DMA2_SPI.DMA_BufferSize = 2; //������ ������� DMA1 � ������� ������ ���������� ������. ������� ������, �������� ���������� DMA_PeripheralDataSize ��� DMA_MemoryDataSize.
	ini_DMA2_SPI.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //��� ���������� (����������) ��������� �� ������� ������ ���������
	ini_DMA2_SPI.DMA_MemoryInc = DMA_MemoryInc_Enable;  //���������� (���������) ��������� �� ������ ������������ ������
	ini_DMA2_SPI.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�������� �� ������� ������������� ��������� �� ������� ������ SPI1 - 1byte
	ini_DMA2_SPI.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //�������� �� ������� ������������� ��������� �� ������ ������������ ������ - 1byte
	ini_DMA2_SPI.DMA_Mode = DMA_Mode_Normal; //���������� ����� �������� ������ DMA, �������� ����� ��������
	ini_DMA2_SPI.DMA_Priority = DMA_Priority_High; //��������� DMA1 �������
	ini_DMA2_SPI.DMA_FIFOMode = DMA_FIFOMode_Disable; //����� FIFO ��������
	ini_DMA2_SPI.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull; //����� ������ FIFO, � ������ ������ �� �����
	ini_DMA2_SPI.DMA_MemoryBurst = DMA_MemoryBurst_Single; //������������ �������� ������� ������: �������� �������� ���������
	ini_DMA2_SPI.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //������������ �������� ������� ���������: �������� �������� ���������

	DMA_Init (DMA2_Stream3, &ini_DMA2_SPI); //������������� DMA2 Stream3 (SPI1_TX)
	SPI_I2S_DMACmd (SPI1, SPI_I2S_DMAReq_Tx, ENABLE); //�������� ��������� DMA SPI1_TX

	DMA_ITConfig(DMA2_Stream3, DMA_IT_TC, ENABLE); //�������� ���������� ��� ������ �������� ������ 3 DMA2
	NVIC_EnableIRQ (DMA2_Stream3_IRQn); //�������� ���������� �� ������ 3 DMA2

	//-----------------------------������������ DMA2 STREAM0 ��� SPI1_RX-------------------------------------------------------------------//
	ini_DMA2_SPI.DMA_Channel = DMA_Channel_3; //����� ������
	ini_DMA2_SPI.DMA_PeripheralBaseAddr = (uint32_t) &(SPI1->DR); //��������� �� ������� ������ ���������: ������ SPI1
	ini_DMA2_SPI.DMA_Memory0BaseAddr = (uint32_t) buffer_RX_SPI; //��������� �� ������ ������������ ������; ��������� �� ������ ���� �������� �������
	ini_DMA2_SPI.DMA_DIR = DMA_DIR_PeripheralToMemory; //����������� �����������, �� ��������� � ������
	ini_DMA2_SPI.DMA_BufferSize = 2; //������ ������� DMA1
	ini_DMA2_SPI.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //��� ���������� (����������) ��������� �� ������� ������ ���������
	ini_DMA2_SPI.DMA_MemoryInc = DMA_MemoryInc_Enable;  //���������� (���������) ��������� �� ������ ������������ ������
	ini_DMA2_SPI.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�������� �� ������� ������������� ��������� �� ������� ������ SPI1 - 1byte
	ini_DMA2_SPI.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //�������� �� ������� ������������� ��������� �� ������ ������������ ������ - 1byte
	ini_DMA2_SPI.DMA_Mode = DMA_Mode_Normal; //���������� ����� �������� ������ DMA, �������� ����� ��������
	ini_DMA2_SPI.DMA_Priority = DMA_Priority_High; //��������� DMA1 �������
	ini_DMA2_SPI.DMA_FIFOMode = DMA_FIFOMode_Disable; //����� FIFO ��������
	ini_DMA2_SPI.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull; //����� ������ FIFO, � ������ ������ �� �����
	ini_DMA2_SPI.DMA_MemoryBurst = DMA_MemoryBurst_Single; //������������ �������� ������� ������: �������� �������� ���������
	ini_DMA2_SPI.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //������������ �������� ������� ���������: �������� �������� ���������

	DMA_Init (DMA2_Stream0, &ini_DMA2_SPI); //������������� DMA2 Stream0 (SPI1_RX)
	SPI_I2S_DMACmd (SPI1, SPI_I2S_DMAReq_Rx, ENABLE); //�������� ��������� DMA SPI1_RX

	DMA_ITConfig (DMA2_Stream0, DMA_IT_TC, ENABLE); //�������� ���������� ��� ������ �������� ������ 0 DMA2
	NVIC_EnableIRQ (DMA2_Stream0_IRQn); //�������� ���������� �� ������ 0 DMA2
}

//----------------������� ��������� ���������� �� DMA2 ������ 0 ��� ��������� ������ (RX) �� SPI1-------------------//
void DMA2_Stream0_IRQHandler(void)
{

	if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0) == SET) //���� ���� ��������� �������� TCIF0 ����������
	{
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0); //������� ���� TCIF0
		CS_OFF(); //�������� chip select
		SPI_work = 0; //������� ���� ������ SPI
	}
}

//--------------������� ��������� ���������� �� DMA2 ������ 0 ��� �������� ������ (TX) �� SPI1-------------------//
void DMA2_Stream3_IRQHandler(void)
{
	if (DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3) == SET) //���� ���� ��������� �������� TCIF3 ����������
	{
		DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3); //������� ���� TCIF3
	}
}

