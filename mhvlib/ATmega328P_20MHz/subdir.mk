################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MHV_AD.cpp \
../MHV_ADC.cpp \
../MHV_DAC.cpp \
../MHV_Debounce.cpp \
../MHV_Device_TX.cpp \
../MHV_EEPROM.cpp \
../MHV_EPP.cpp \
../MHV_GammaCorrect.cpp \
../MHV_Lock.cpp \
../MHV_NotePlayer.cpp \
../MHV_PID.cpp \
../MHV_PinChangeManager.cpp \
../MHV_RTC.cpp \
../MHV_ServoControl.cpp \
../MHV_Shifter.cpp \
../MHV_SoftwareHBridge.cpp \
../MHV_SoftwarePWM.cpp \
../MHV_StepperMotor.cpp \
../MHV_Timer.cpp \
../MHV_WaveGenerator.cpp 

OBJS += \
./MHV_AD.o \
./MHV_ADC.o \
./MHV_DAC.o \
./MHV_Debounce.o \
./MHV_Device_TX.o \
./MHV_EEPROM.o \
./MHV_EPP.o \
./MHV_GammaCorrect.o \
./MHV_Lock.o \
./MHV_NotePlayer.o \
./MHV_PID.o \
./MHV_PinChangeManager.o \
./MHV_RTC.o \
./MHV_ServoControl.o \
./MHV_Shifter.o \
./MHV_SoftwareHBridge.o \
./MHV_SoftwarePWM.o \
./MHV_StepperMotor.o \
./MHV_Timer.o \
./MHV_WaveGenerator.o 

CPP_DEPS += \
./MHV_AD.d \
./MHV_ADC.d \
./MHV_DAC.d \
./MHV_Debounce.d \
./MHV_Device_TX.d \
./MHV_EEPROM.d \
./MHV_EPP.d \
./MHV_GammaCorrect.d \
./MHV_Lock.d \
./MHV_NotePlayer.d \
./MHV_PID.d \
./MHV_PinChangeManager.d \
./MHV_RTC.d \
./MHV_ServoControl.d \
./MHV_Shifter.d \
./MHV_SoftwareHBridge.d \
./MHV_SoftwarePWM.d \
./MHV_StepperMotor.d \
./MHV_Timer.d \
./MHV_WaveGenerator.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib" -DMHVLIB_CORE=1 -Wall -g2 -gstabs -Os -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -fno-caller-saves -fno-dse -funsigned-char -funsigned-bitfields -fno-exceptions -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wextra  -Wno-non-virtual-dtor -mmcu=atmega328p -DF_CPU=20000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


