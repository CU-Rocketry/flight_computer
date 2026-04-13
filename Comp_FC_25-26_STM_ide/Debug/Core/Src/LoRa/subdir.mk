################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/LoRa/lr_fhss_mac.c \
../Core/Src/LoRa/sx126x.c \
../Core/Src/LoRa/sx126x_bpsk.c \
../Core/Src/LoRa/sx126x_driver_version.c \
../Core/Src/LoRa/sx126x_lr_fhss.c 

OBJS += \
./Core/Src/LoRa/lr_fhss_mac.o \
./Core/Src/LoRa/sx126x.o \
./Core/Src/LoRa/sx126x_bpsk.o \
./Core/Src/LoRa/sx126x_driver_version.o \
./Core/Src/LoRa/sx126x_lr_fhss.o 

C_DEPS += \
./Core/Src/LoRa/lr_fhss_mac.d \
./Core/Src/LoRa/sx126x.d \
./Core/Src/LoRa/sx126x_bpsk.d \
./Core/Src/LoRa/sx126x_driver_version.d \
./Core/Src/LoRa/sx126x_lr_fhss.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/LoRa/%.o Core/Src/LoRa/%.su Core/Src/LoRa/%.cyclo: ../Core/Src/LoRa/%.c Core/Src/LoRa/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H523xx -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-LoRa

clean-Core-2f-Src-2f-LoRa:
	-$(RM) ./Core/Src/LoRa/lr_fhss_mac.cyclo ./Core/Src/LoRa/lr_fhss_mac.d ./Core/Src/LoRa/lr_fhss_mac.o ./Core/Src/LoRa/lr_fhss_mac.su ./Core/Src/LoRa/sx126x.cyclo ./Core/Src/LoRa/sx126x.d ./Core/Src/LoRa/sx126x.o ./Core/Src/LoRa/sx126x.su ./Core/Src/LoRa/sx126x_bpsk.cyclo ./Core/Src/LoRa/sx126x_bpsk.d ./Core/Src/LoRa/sx126x_bpsk.o ./Core/Src/LoRa/sx126x_bpsk.su ./Core/Src/LoRa/sx126x_driver_version.cyclo ./Core/Src/LoRa/sx126x_driver_version.d ./Core/Src/LoRa/sx126x_driver_version.o ./Core/Src/LoRa/sx126x_driver_version.su ./Core/Src/LoRa/sx126x_lr_fhss.cyclo ./Core/Src/LoRa/sx126x_lr_fhss.d ./Core/Src/LoRa/sx126x_lr_fhss.o ./Core/Src/LoRa/sx126x_lr_fhss.su

.PHONY: clean-Core-2f-Src-2f-LoRa

