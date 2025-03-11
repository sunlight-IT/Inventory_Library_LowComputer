################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Usr/component/book_database/database.c 

OBJS += \
./Usr/component/book_database/database.o 

C_DEPS += \
./Usr/component/book_database/database.d 


# Each subdirectory must supply rules for building sources it contributes
Usr/component/book_database/%.o Usr/component/book_database/%.su Usr/component/book_database/%.cyclo: ../Usr/component/book_database/%.c Usr/component/book_database/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"S:/WorkeSpace/SY_Workplace/WorkSpace/Inventory_equipment/Inventory_equipment/Usr" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Usr-2f-component-2f-book_database

clean-Usr-2f-component-2f-book_database:
	-$(RM) ./Usr/component/book_database/database.cyclo ./Usr/component/book_database/database.d ./Usr/component/book_database/database.o ./Usr/component/book_database/database.su

.PHONY: clean-Usr-2f-component-2f-book_database

