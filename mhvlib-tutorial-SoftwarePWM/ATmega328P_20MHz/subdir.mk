################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../SoftwarePWM.cpp 

OBJS += \
./SoftwarePWM.o 

CPP_DEPS += \
./SoftwarePWM.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib" -Wall -g1 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -flto -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega328p -DF_CPU=2000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

