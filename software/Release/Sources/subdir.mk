################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/dmx_sender.c 

OBJS += \
./Sources/dmx_sender.o 

C_DEPS += \
./Sources/dmx_sender.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/usr/local/CrossPack-AVR/avr/include -I/usr/local/CrossPack-AVR/lib/gcc/avr/4.3.3/include -I/usr/local/CrossPack-AVR/lib/gcc/avr/4.3.3/include-fixed -Wall -O3 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega8 -DF_CPU=20000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


