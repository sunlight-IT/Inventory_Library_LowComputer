################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Usr/module_apply/app_communicate.c \
../Usr/module_apply/apply_motor.c 

OBJS += \
./Usr/module_apply/app_communicate.o \
./Usr/module_apply/apply_motor.o 

C_DEPS += \
./Usr/module_apply/app_communicate.d \
./Usr/module_apply/apply_motor.d 


# Each subdirectory must supply rules for building sources it contributes
Usr/module_apply/%.o Usr/module_apply/%.su Usr/module_apply/%.cyclo: ../Usr/module_apply/%.c Usr/module_apply/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"S:/WorkeSpace/SY_Workplace/WorkSpace/Inventory_equipment/Inventory_equipment/Usr" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Usr-2f-module_apply

clean-Usr-2f-module_apply:
	-$(RM) ./Usr/module_apply/app_communicate.cyclo ./Usr/module_apply/app_communicate.d ./Usr/module_apply/app_communicate.o ./Usr/module_apply/app_communicate.su ./Usr/module_apply/apply_motor.cyclo ./Usr/module_apply/apply_motor.d ./Usr/module_apply/apply_motor.o ./Usr/module_apply/apply_motor.su

.PHONY: clean-Usr-2f-module_apply

