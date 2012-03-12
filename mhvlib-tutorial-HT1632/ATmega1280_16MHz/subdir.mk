################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../HT1632.cpp 

OBJS += \
./HT1632.o 

CPP_DEPS += \
./HT1632.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib" -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -fno-caller-saves -fno-dse -funsigned-char -funsigned-bitfields -fno-exceptions -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wextra -mmcu=atmega1280 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


