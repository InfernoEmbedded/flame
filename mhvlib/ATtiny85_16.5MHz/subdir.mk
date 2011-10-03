################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../MHV_AD.cpp \
../MHV_ADC.cpp \
../MHV_Debounce.cpp \
../MHV_Device_RX.cpp \
../MHV_Device_TX.cpp \
../MHV_Display_Character.cpp \
../MHV_Display_HD44780.cpp \
../MHV_Display_HD44780_Direct_Connect.cpp \
../MHV_Display_HD44780_Shift_Register.cpp \
../MHV_Display_Holtek_HT1632.cpp \
../MHV_Display_Monochrome.cpp \
../MHV_Display_Monochrome_Buffered.cpp \
../MHV_EEPROM.cpp \
../MHV_EPP.cpp \
../MHV_GammaCorrect.cpp \
../MHV_HardwareSerial.cpp \
../MHV_Lock.cpp \
../MHV_PID.cpp \
../MHV_PWMMatrix.cpp \
../MHV_PinChangeManager.cpp \
../MHV_RTC.cpp \
../MHV_RingBuffer.cpp \
../MHV_ServoControl.cpp \
../MHV_Shifter.cpp \
../MHV_SoftwareHBridge.cpp \
../MHV_Timer16.cpp \
../MHV_Timer8.cpp \
../MHV_VoltageRegulator.cpp 

OBJS += \
./MHV_AD.o \
./MHV_ADC.o \
./MHV_Debounce.o \
./MHV_Device_RX.o \
./MHV_Device_TX.o \
./MHV_Display_Character.o \
./MHV_Display_HD44780.o \
./MHV_Display_HD44780_Direct_Connect.o \
./MHV_Display_HD44780_Shift_Register.o \
./MHV_Display_Holtek_HT1632.o \
./MHV_Display_Monochrome.o \
./MHV_Display_Monochrome_Buffered.o \
./MHV_EEPROM.o \
./MHV_EPP.o \
./MHV_GammaCorrect.o \
./MHV_HardwareSerial.o \
./MHV_Lock.o \
./MHV_PID.o \
./MHV_PWMMatrix.o \
./MHV_PinChangeManager.o \
./MHV_RTC.o \
./MHV_RingBuffer.o \
./MHV_ServoControl.o \
./MHV_Shifter.o \
./MHV_SoftwareHBridge.o \
./MHV_Timer16.o \
./MHV_Timer8.o \
./MHV_VoltageRegulator.o 

CPP_DEPS += \
./MHV_AD.d \
./MHV_ADC.d \
./MHV_Debounce.d \
./MHV_Device_RX.d \
./MHV_Device_TX.d \
./MHV_Display_Character.d \
./MHV_Display_HD44780.d \
./MHV_Display_HD44780_Direct_Connect.d \
./MHV_Display_HD44780_Shift_Register.d \
./MHV_Display_Holtek_HT1632.d \
./MHV_Display_Monochrome.d \
./MHV_Display_Monochrome_Buffered.d \
./MHV_EEPROM.d \
./MHV_EPP.d \
./MHV_GammaCorrect.d \
./MHV_HardwareSerial.d \
./MHV_Lock.d \
./MHV_PID.d \
./MHV_PWMMatrix.d \
./MHV_PinChangeManager.d \
./MHV_RTC.d \
./MHV_RingBuffer.d \
./MHV_ServoControl.d \
./MHV_Shifter.d \
./MHV_SoftwareHBridge.d \
./MHV_Timer16.d \
./MHV_Timer8.d \
./MHV_VoltageRegulator.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib" -DMHVLIB_CORE -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -mmcu=attiny85 -DF_CPU=16500000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


