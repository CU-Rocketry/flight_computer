################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/system_stm32h5xx.c 

OBJS += \
./Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/system_stm32h5xx.o 

C_DEPS += \
./Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/system_stm32h5xx.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/%.o Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/%.su Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/%.cyclo: ../Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/%.c Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H523xx -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-CMSIS-2f-Device-2f-ST-2f-STM32H5xx-2f-Source-2f-Templates

clean-Drivers-2f-CMSIS-2f-Device-2f-ST-2f-STM32H5xx-2f-Source-2f-Templates:
	-$(RM) ./Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/system_stm32h5xx.cyclo ./Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/system_stm32h5xx.d ./Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/system_stm32h5xx.o ./Drivers/CMSIS/Device/ST/STM32H5xx/Source/Templates/system_stm32h5xx.su

.PHONY: clean-Drivers-2f-CMSIS-2f-Device-2f-ST-2f-STM32H5xx-2f-Source-2f-Templates

