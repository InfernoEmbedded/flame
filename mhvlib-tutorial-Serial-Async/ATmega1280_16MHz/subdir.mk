################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../SerialAsync.cpp 

OBJS += \
./SerialAsync.o 

CPP_DEPS += \
./SerialAsync.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"C:\workspace\mhvlib\mhvlib" -Wall -Os -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -flto -funsigned-char -funsigned-bitfields -fno-exceptions -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wextra -std=c++11 -mmcu=atmega1280 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


