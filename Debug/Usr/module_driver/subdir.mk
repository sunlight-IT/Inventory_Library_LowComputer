################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Usr/module_driver/driver_motor.c \
../Usr/module_driver/driver_upp_uart.c \
../Usr/module_driver/driver_wireless.c 

OBJS += \
./Usr/module_driver/driver_motor.o \
./Usr/module_driver/driver_upp_uart.o \
./Usr/module_driver/driver_wireless.o 

C_DEPS += \
./Usr/module_driver/driver_motor.d \
./Usr/module_driver/driver_upp_uart.d \
./Usr/module_driver/driver_wireless.d 


# Each subdirectory must supply rules for building sources it contributes
Usr/module_driver/%.o Usr/module_driver/%.su Usr/module_driver/%.cyclo: ../Usr/module_driver/%.c Usr/module_driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"S:/WorkeSpace/SY_Workplace/WorkSpace/Inventory_equipment/Inventory_equipment/Usr" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Usr-2f-module_driver

clean-Usr-2f-module_driver:
	-$(RM) ./Usr/module_driver/driver_motor.cyclo ./Usr/module_driver/driver_motor.d ./Usr/module_driver/driver_motor.o ./Usr/module_driver/driver_motor.su ./Usr/module_driver/driver_upp_uart.cyclo ./Usr/module_driver/driver_upp_uart.d ./Usr/module_driver/driver_upp_uart.o ./Usr/module_driver/driver_upp_uart.su ./Usr/module_driver/driver_wireless.cyclo ./Usr/module_driver/driver_wireless.d ./Usr/module_driver/driver_wireless.o ./Usr/module_driver/driver_wireless.su

.PHONY: clean-Usr-2f-module_driver

