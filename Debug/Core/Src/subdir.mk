################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/BLDCMotor.c \
../Core/Src/Inventory_App.c \
../Core/Src/MainLogic.c \
../Core/Src/Reader15693.c \
../Core/Src/ServoMotor.c \
../Core/Src/SystemTasks.c \
../Core/Src/Template.c \
../Core/Src/WirelessModule.c \
../Core/Src/adc.c \
../Core/Src/dma.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/main.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/BLDCMotor.o \
./Core/Src/Inventory_App.o \
./Core/Src/MainLogic.o \
./Core/Src/Reader15693.o \
./Core/Src/ServoMotor.o \
./Core/Src/SystemTasks.o \
./Core/Src/Template.o \
./Core/Src/WirelessModule.o \
./Core/Src/adc.o \
./Core/Src/dma.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/main.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/BLDCMotor.d \
./Core/Src/Inventory_App.d \
./Core/Src/MainLogic.d \
./Core/Src/Reader15693.d \
./Core/Src/ServoMotor.d \
./Core/Src/SystemTasks.d \
./Core/Src/Template.d \
./Core/Src/WirelessModule.d \
./Core/Src/adc.d \
./Core/Src/dma.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/main.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"S:/WorkeSpace/SY_Workplace/WorkSpace/Inventory_equipment/Inventory_equipment/Usr" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/BLDCMotor.cyclo ./Core/Src/BLDCMotor.d ./Core/Src/BLDCMotor.o ./Core/Src/BLDCMotor.su ./Core/Src/Inventory_App.cyclo ./Core/Src/Inventory_App.d ./Core/Src/Inventory_App.o ./Core/Src/Inventory_App.su ./Core/Src/MainLogic.cyclo ./Core/Src/MainLogic.d ./Core/Src/MainLogic.o ./Core/Src/MainLogic.su ./Core/Src/Reader15693.cyclo ./Core/Src/Reader15693.d ./Core/Src/Reader15693.o ./Core/Src/Reader15693.su ./Core/Src/ServoMotor.cyclo ./Core/Src/ServoMotor.d ./Core/Src/ServoMotor.o ./Core/Src/ServoMotor.su ./Core/Src/SystemTasks.cyclo ./Core/Src/SystemTasks.d ./Core/Src/SystemTasks.o ./Core/Src/SystemTasks.su ./Core/Src/Template.cyclo ./Core/Src/Template.d ./Core/Src/Template.o ./Core/Src/Template.su ./Core/Src/WirelessModule.cyclo ./Core/Src/WirelessModule.d ./Core/Src/WirelessModule.o ./Core/Src/WirelessModule.su ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/dma.cyclo ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

