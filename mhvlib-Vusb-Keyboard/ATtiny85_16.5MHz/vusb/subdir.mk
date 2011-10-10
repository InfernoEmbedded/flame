################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../vusb/oddebug.c \
../vusb/usbdrv.c 

S_UPPER_SRCS += \
../vusb/usbdrvasm.S 

ASM_SRCS += \
../vusb/usbdrvasm.asm 

OBJS += \
./vusb/oddebug.o \
./vusb/usbdrv.o \
./vusb/usbdrvasm.o 

C_DEPS += \
./vusb/oddebug.d \
./vusb/usbdrv.d 

S_UPPER_DEPS += \
./vusb/usbdrvasm.d 

ASM_DEPS += \
./vusb/usbdrvasm.d 


# Each subdirectory must supply rules for building sources it contributes
vusb/%.o: ../vusb/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"A:\eclipse\mhvlib" -I"A:\eclipse\mhvlib-Vusb-Keyboard" -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=attiny85 -DF_CPU=16500000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

vusb/%.o: ../vusb/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -DF_CPU=16500000 -I"A:\eclipse\mhvlib" -mmcu=attiny85 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

vusb/%.o: ../vusb/%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -DF_CPU=16500000 -I"A:\eclipse\mhvlib" -mmcu=attiny85 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


