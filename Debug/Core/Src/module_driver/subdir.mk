################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/module_driver/driver_wireless.c 

OBJS += \
./Core/Src/module_driver/driver_wireless.o 

C_DEPS += \
./Core/Src/module_driver/driver_wireless.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/module_driver/%.o Core/Src/module_driver/%.su Core/Src/module_driver/%.cyclo: ../Core/Src/module_driver/%.c Core/Src/module_driver/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"S:/WorkeSpace/SY_Workplace/Libary_data/Inventory_equipment/Inventory_equipment/Core/Src/log" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-module_driver

clean-Core-2f-Src-2f-module_driver:
	-$(RM) ./Core/Src/module_driver/driver_wireless.cyclo ./Core/Src/module_driver/driver_wireless.d ./Core/Src/module_driver/driver_wireless.o ./Core/Src/module_driver/driver_wireless.su

.PHONY: clean-Core-2f-Src-2f-module_driver

