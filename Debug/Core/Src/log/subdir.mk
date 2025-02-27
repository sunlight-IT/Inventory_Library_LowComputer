################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/log/my_log.c 

OBJS += \
./Core/Src/log/my_log.o 

C_DEPS += \
./Core/Src/log/my_log.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/log/%.o Core/Src/log/%.su Core/Src/log/%.cyclo: ../Core/Src/log/%.c Core/Src/log/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"S:/WorkeSpace/SY_Workplace/Libary_data/Inventory_equipment/Inventory_equipment/Core/Src/log" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-log

clean-Core-2f-Src-2f-log:
	-$(RM) ./Core/Src/log/my_log.cyclo ./Core/Src/log/my_log.d ./Core/Src/log/my_log.o ./Core/Src/log/my_log.su

.PHONY: clean-Core-2f-Src-2f-log

