################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MCU_ini.c \
../src/SPI_lib.c \
../src/delay_lib.c \
../src/main.c \
../src/nrf24l01.c \
../src/syscalls.c \
../src/system_stm32f4xx.c \
../src/uart_lib.c 

OBJS += \
./src/MCU_ini.o \
./src/SPI_lib.o \
./src/delay_lib.o \
./src/main.o \
./src/nrf24l01.o \
./src/syscalls.o \
./src/system_stm32f4xx.o \
./src/uart_lib.o 

C_DEPS += \
./src/MCU_ini.d \
./src/SPI_lib.d \
./src/delay_lib.d \
./src/main.d \
./src/nrf24l01.d \
./src/syscalls.d \
./src/system_stm32f4xx.d \
./src/uart_lib.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F407VGTx -DSTM32F407G_DISC1 -DDEBUG -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -I"/home/arashamf/workspace/NRF_F4/StdPeriph_Driver/inc" -I"/home/arashamf/workspace/NRF_F4/inc" -I"/home/arashamf/workspace/NRF_F4/CMSIS/device" -I"/home/arashamf/workspace/NRF_F4/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


