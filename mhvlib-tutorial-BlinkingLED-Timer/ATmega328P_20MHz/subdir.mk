################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../BlinkingLED-Timer.cpp 

OBJS += \
./BlinkingLED-Timer.o 

CPP_DEPS += \
./BlinkingLED-Timer.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib\mhvlib" -Wall -g2 -gstabs -Os -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -funsigned-char -funsigned-bitfields -fno-exceptions -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wextra  -Wno-non-virtual-dtor -std=c++11 -mmcu=atmega328p -DF_CPU=20000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


