################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Usr/log/my_log.c 

OBJS += \
./Usr/log/my_log.o 

C_DEPS += \
./Usr/log/my_log.d 


# Each subdirectory must supply rules for building sources it contributes
Usr/log/%.o Usr/log/%.su Usr/log/%.cyclo: ../Usr/log/%.c Usr/log/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"S:/WorkeSpace/SY_Workplace/WorkSpace/Inventory_equipment/Inventory_equipment/Usr" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Usr-2f-log

clean-Usr-2f-log:
	-$(RM) ./Usr/log/my_log.cyclo ./Usr/log/my_log.d ./Usr/log/my_log.o ./Usr/log/my_log.su

.PHONY: clean-Usr-2f-log

