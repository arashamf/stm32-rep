################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MAX7219.c \
../src/MCU_ini.c \
../src/RTC_ini.c \
../src/SPI.c \
../src/delay_lib.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f4xx.c 

OBJS += \
./src/MAX7219.o \
./src/MCU_ini.o \
./src/RTC_ini.o \
./src/SPI.o \
./src/delay_lib.o \
./src/main.o \
./src/syscalls.o \
./src/system_stm32f4xx.o 

C_DEPS += \
./src/MAX7219.d \
./src/MCU_ini.d \
./src/RTC_ini.d \
./src/SPI.d \
./src/delay_lib.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32 -DSTM32F4 -DSTM32F407VGTx -DSTM32F407G_DISC1 -DDEBUG -DSTM32F40XX -DSTM32F40_41xxx -DUSE_STDPERIPH_DRIVER -I"D:/my docs/projects/stm32/clock_led_matrix/StdPeriph_Driver/inc" -I"D:/my docs/projects/stm32/clock_led_matrix/inc" -I"D:/my docs/projects/stm32/clock_led_matrix/CMSIS/device" -I"D:/my docs/projects/stm32/clock_led_matrix/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


