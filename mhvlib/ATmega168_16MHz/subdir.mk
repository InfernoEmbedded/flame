################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AD.cpp \
../ADCManager.cpp \
../Accelerometer.cpp \
../BestSphereGaussNewtonCalibrator.cpp \
../DAC.cpp \
../Device_TX.cpp \
../EEPROM.cpp \
../EPP.cpp \
../GammaCorrect.cpp \
../Lock.cpp \
../NotePlayer.cpp \
../PID.cpp \
../PinChangeManager.cpp \
../PureVirtual.cpp \
../RTC.cpp \
../Shifter.cpp \
../SoftwareHBridge.cpp \
../StepperMotor.cpp \
../Timer.cpp \
../TripleAxisCalibrator.cpp \
../TripleAxisSensor.cpp \
../WaveGenerator.cpp 

OBJS += \
./AD.o \
./ADCManager.o \
./Accelerometer.o \
./BestSphereGaussNewtonCalibrator.o \
./DAC.o \
./Device_TX.o \
./EEPROM.o \
./EPP.o \
./GammaCorrect.o \
./Lock.o \
./NotePlayer.o \
./PID.o \
./PinChangeManager.o \
./PureVirtual.o \
./RTC.o \
./Shifter.o \
./SoftwareHBridge.o \
./StepperMotor.o \
./Timer.o \
./TripleAxisCalibrator.o \
./TripleAxisSensor.o \
./WaveGenerator.o 

CPP_DEPS += \
./AD.d \
./ADCManager.d \
./Accelerometer.d \
./BestSphereGaussNewtonCalibrator.d \
./DAC.d \
./Device_TX.d \
./EEPROM.d \
./EPP.d \
./GammaCorrect.d \
./Lock.d \
./NotePlayer.d \
./PID.d \
./PinChangeManager.d \
./PureVirtual.d \
./RTC.d \
./Shifter.d \
./SoftwareHBridge.d \
./StepperMotor.d \
./Timer.d \
./TripleAxisCalibrator.d \
./TripleAxisSensor.d \
./WaveGenerator.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"A:\eclipse\mhvlib\mhvlib" -DMHVLIB_CORE=1 -Wall -Os -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -flto -fno-inline -funsigned-char -funsigned-bitfields -fno-exceptions -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wextra  -Wno-non-virtual-dtor -std=c++11 -mmcu=atmega168 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


