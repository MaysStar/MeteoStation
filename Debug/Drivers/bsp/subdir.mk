################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/bsp/at24c32.c \
../Drivers/bsp/ds1307.c \
../Drivers/bsp/lcd.c 

OBJS += \
./Drivers/bsp/at24c32.o \
./Drivers/bsp/ds1307.o \
./Drivers/bsp/lcd.o 

C_DEPS += \
./Drivers/bsp/at24c32.d \
./Drivers/bsp/ds1307.d \
./Drivers/bsp/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/bsp/%.o Drivers/bsp/%.su Drivers/bsp/%.cyclo: ../Drivers/bsp/%.c Drivers/bsp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../USB_HOST/App -I"C:/programing/Embedded/projects_stm32/MeteoStation/MeteoStation/Drivers/bsp" -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/CDC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-bsp

clean-Drivers-2f-bsp:
	-$(RM) ./Drivers/bsp/at24c32.cyclo ./Drivers/bsp/at24c32.d ./Drivers/bsp/at24c32.o ./Drivers/bsp/at24c32.su ./Drivers/bsp/ds1307.cyclo ./Drivers/bsp/ds1307.d ./Drivers/bsp/ds1307.o ./Drivers/bsp/ds1307.su ./Drivers/bsp/lcd.cyclo ./Drivers/bsp/lcd.d ./Drivers/bsp/lcd.o ./Drivers/bsp/lcd.su

.PHONY: clean-Drivers-2f-bsp

