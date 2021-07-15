################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32.s 

OBJS += \
./startup/startup_stm32.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m3 -mthumb -mfloat-abi=soft -I"/home/arashamf/projects/stm32/stm32f103/at24cp_i2c/StdPeriph_Driver/inc" -I"/home/arashamf/projects/stm32/stm32f103/at24cp_i2c/inc" -I"/home/arashamf/projects/stm32/stm32f103/at24cp_i2c/CMSIS/device" -I"/home/arashamf/projects/stm32/stm32f103/at24cp_i2c/CMSIS/core" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


