
#ifndef SPI_ini
#define SPI_ini

#define CS_ON() GPIO_ResetBits (GPIOE, GPIO_Pin_3);
#define CS_OFF() GPIO_SetBits (GPIOE, GPIO_Pin_3);
void Start_SPI(uint8_t );
void SPI_wait();
uint8_t Get_SPI_In (uint8_t);
void Set_SPI_Out(uint8_t , uint8_t );
void SPI1_ini ();
void DMA_SPI1_ini ();
#endif
