################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Usr/module_middle/middle_process.c 

OBJS += \
./Usr/module_middle/middle_process.o 

C_DEPS += \
./Usr/module_middle/middle_process.d 


# Each subdirectory must supply rules for building sources it contributes
Usr/module_middle/%.o Usr/module_middle/%.su Usr/module_middle/%.cyclo: ../Usr/module_middle/%.c Usr/module_middle/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"S:/WorkeSpace/SY_Workplace/WorkSpace/Inventory_equipment/Inventory_equipment/Usr" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Usr-2f-module_middle

clean-Usr-2f-module_middle:
	-$(RM) ./Usr/module_middle/middle_process.cyclo ./Usr/module_middle/middle_process.d ./Usr/module_middle/middle_process.o ./Usr/module_middle/middle_process.su

.PHONY: clean-Usr-2f-module_middle

