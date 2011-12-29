################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../WS2801.cpp 

OBJS += \
./WS2801.o 

CPP_DEPS += \
./WS2801.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib" -Wall -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fno-caller-saves -fno-dse -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega1280 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


