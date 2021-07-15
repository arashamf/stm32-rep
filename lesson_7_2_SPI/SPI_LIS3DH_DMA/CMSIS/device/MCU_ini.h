#ifndef MCU_ini
#define MCU_ini
void LEDS_ini ();
void UART2_ini ();
uint8_t PLL_ini (uint8_t , uint8_t , uint32_t , uint32_t );
#define BAUDRATE 57600
#define APB_CLOCK 42000000L // частота шины APB

#endif
