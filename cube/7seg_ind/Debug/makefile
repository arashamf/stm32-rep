################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
7seg_ind.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
7seg_ind.list \

OBJCOPY_BIN += \
7seg_ind.bin \


# All Target
all: 7seg_ind.elf secondary-outputs

# Tool invocations
7seg_ind.elf: $(OBJS) $(USER_OBJS) /home/arashamf/projects/stm32/cube/7seg_ind/STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "7seg_ind.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"/home/arashamf/projects/stm32/cube/7seg_ind/STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="7seg_ind.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

7seg_ind.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "7seg_ind.list"
	@echo 'Finished building: $@'
	@echo ' '

7seg_ind.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "7seg_ind.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
