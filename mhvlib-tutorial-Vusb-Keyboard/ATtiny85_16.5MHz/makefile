################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
mhvlib-tutorial-Vusb-Keyboard.lss \

FLASH_IMAGE += \
mhvlib-tutorial-Vusb-Keyboard.hex \

EEPROM_IMAGE += \
mhvlib-tutorial-Vusb-Keyboard.eep \

SIZEDUMMY += \
sizedummy \

AVRDUDEDUMMY += \
avrdudedummy \


# All Target
all: mhvlib-tutorial-Vusb-Keyboard.elf secondary-outputs

dependents:
	-cd A:/eclipse/mhvlib/ATtiny85_16.5MHz && $(MAKE) all
	-cd A:/eclipse/mhvlib-Vusb-Keyboard/ATtiny85_16.5MHz && $(MAKE) all

# Tool invocations
mhvlib-tutorial-Vusb-Keyboard.elf: $(OBJS) $(USER_OBJS) A:/eclipse/mhvlib/ATtiny85_16.5MHz/libmhvlib.a A:/eclipse/mhvlib-Vusb-Keyboard/ATtiny85_16.5MHz/libmhvlib-vusb-keyboard.a
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,mhvlib-tutorial-Vusb-Keyboard.map,--cref -Wl,--gc-sections -L"A:\eclipse\mhvlib/ATtiny85_16.5MHz" -L"A:\eclipse\mhvlib-Vusb-Keyboard/ATtiny85_16.5MHz" -mmcu=attiny85 -o"mhvlib-tutorial-Vusb-Keyboard.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

mhvlib-tutorial-Vusb-Keyboard.lss: mhvlib-tutorial-Vusb-Keyboard.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S mhvlib-tutorial-Vusb-Keyboard.elf  >"mhvlib-tutorial-Vusb-Keyboard.lss"
	@echo 'Finished building: $@'
	@echo ' '

mhvlib-tutorial-Vusb-Keyboard.hex: mhvlib-tutorial-Vusb-Keyboard.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex mhvlib-tutorial-Vusb-Keyboard.elf  "mhvlib-tutorial-Vusb-Keyboard.hex"
	@echo 'Finished building: $@'
	@echo ' '

mhvlib-tutorial-Vusb-Keyboard.eep: mhvlib-tutorial-Vusb-Keyboard.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex mhvlib-tutorial-Vusb-Keyboard.elf  "mhvlib-tutorial-Vusb-Keyboard.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: mhvlib-tutorial-Vusb-Keyboard.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=attiny85 mhvlib-tutorial-Vusb-Keyboard.elf
	@echo 'Finished building: $@'
	@echo ' '

avrdudedummy: mhvlib-tutorial-Vusb-Keyboard.elf
	@echo 'Invoking: AVRDude'
	C:\Progra~2\MHV AVR Tools\bin\avrdude -pt85 -cusbasp -Uflash:w:mhvlib-tutorial-Vusb-Keyboard.hex:a
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EEPROM_IMAGE)$(ELFS)$(FLASH_IMAGE)$(LSS)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(AVRDUDEDUMMY)$(CPP_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) mhvlib-tutorial-Vusb-Keyboard.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY) $(AVRDUDEDUMMY)

.PHONY: all clean dependents
.SECONDARY:
A:/eclipse/mhvlib/ATtiny85_16.5MHz/libmhvlib.a:
A:/eclipse/mhvlib-Vusb-Keyboard/ATtiny85_16.5MHz/libmhvlib-vusb-keyboard.a:

-include ../makefile.targets
