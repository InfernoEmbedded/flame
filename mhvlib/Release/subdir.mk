################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MHV_HardwareSerial.cpp \
../MHV_RingBuffer.cpp \
../MHV_ServoControl.cpp \
../MHV_Timer16.cpp \
../MHV_Timer8.cpp 

OBJS += \
./MHV_HardwareSerial.o \
./MHV_RingBuffer.o \
./MHV_ServoControl.o \
./MHV_Timer16.o \
./MHV_Timer8.o 

CPP_DEPS += \
./MHV_HardwareSerial.d \
./MHV_RingBuffer.d \
./MHV_ServoControl.d \
./MHV_Timer16.d \
./MHV_Timer8.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib" -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=atmega168 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


