################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../ServoMotor.cpp 

OBJS += \
./ServoMotor.o 

CPP_DEPS += \
./ServoMotor.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib" -I"A:\eclipse\mhvlib-Vusb-Keyboard" -I"A:\eclipse\mhvlib-Vusb-Console" -Wall -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega1280 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


