################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Usr/communicate/middle_lower.c \
../Usr/communicate/middle_upper.c 

OBJS += \
./Usr/communicate/middle_lower.o \
./Usr/communicate/middle_upper.o 

C_DEPS += \
./Usr/communicate/middle_lower.d \
./Usr/communicate/middle_upper.d 


# Each subdirectory must supply rules for building sources it contributes
Usr/communicate/%.o Usr/communicate/%.su Usr/communicate/%.cyclo: ../Usr/communicate/%.c Usr/communicate/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"S:/WorkeSpace/SY_Workplace/WorkSpace/Inventory_equipment/Inventory_equipment/Usr" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Usr-2f-communicate

clean-Usr-2f-communicate:
	-$(RM) ./Usr/communicate/middle_lower.cyclo ./Usr/communicate/middle_lower.d ./Usr/communicate/middle_lower.o ./Usr/communicate/middle_lower.su ./Usr/communicate/middle_upper.cyclo ./Usr/communicate/middle_upper.d ./Usr/communicate/middle_upper.o ./Usr/communicate/middle_upper.su

.PHONY: clean-Usr-2f-communicate

