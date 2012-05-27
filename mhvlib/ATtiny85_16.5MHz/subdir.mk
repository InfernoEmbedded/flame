################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AD.cpp \
../ADC.cpp \
../DAC.cpp \
../Debounce.cpp \
../Device_TX.cpp \
../EEPROM.cpp \
../EPP.cpp \
../GammaCorrect.cpp \
../Lock.cpp \
../NotePlayer.cpp \
../PID.cpp \
../PinChangeManager.cpp \
../RTC.cpp \
../ServoControl.cpp \
../Shifter.cpp \
../SoftwareHBridge.cpp \
../StepperMotor.cpp \
../Timer.cpp \
../WaveGenerator.cpp 

OBJS += \
./AD.o \
./ADC.o \
./DAC.o \
./Debounce.o \
./Device_TX.o \
./EEPROM.o \
./EPP.o \
./GammaCorrect.o \
./Lock.o \
./NotePlayer.o \
./PID.o \
./PinChangeManager.o \
./RTC.o \
./ServoControl.o \
./Shifter.o \
./SoftwareHBridge.o \
./StepperMotor.o \
./Timer.o \
./WaveGenerator.o 

CPP_DEPS += \
./AD.d \
./ADC.d \
./DAC.d \
./Debounce.d \
./Device_TX.d \
./EEPROM.d \
./EPP.d \
./GammaCorrect.d \
./Lock.d \
./NotePlayer.d \
./PID.d \
./PinChangeManager.d \
./RTC.d \
./ServoControl.d \
./Shifter.d \
./SoftwareHBridge.d \
./StepperMotor.d \
./Timer.d \
./WaveGenerator.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib" -DMHVLIB_CORE=1 -Wall -Os -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -fno-caller-saves -fno-dse -funsigned-char -funsigned-bitfields -fno-exceptions -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wextra  -Wno-non-virtual-dtor -std=c++11 -mmcu=attiny85 -DF_CPU=16500000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


