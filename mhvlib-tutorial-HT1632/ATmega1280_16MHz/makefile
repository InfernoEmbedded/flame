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
mhvlib-tutorial-HT1632.lss \

FLASH_IMAGE += \
mhvlib-tutorial-HT1632.hex \

EEPROM_IMAGE += \
mhvlib-tutorial-HT1632.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: mhvlib-tutorial-HT1632.elf secondary-outputs

# Tool invocations
mhvlib-tutorial-HT1632.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,mhvlib-tutorial-HT1632.map,--cref -Wl,--gc-sections -L"A:\eclipse\mhvlib/ATmega1280_16MHz" -mmcu=atmega1280 -o "mhvlib-tutorial-HT1632.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

mhvlib-tutorial-HT1632.lss: mhvlib-tutorial-HT1632.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S mhvlib-tutorial-HT1632.elf  >"mhvlib-tutorial-HT1632.lss"
	@echo 'Finished building: $@'
	@echo ' '

mhvlib-tutorial-HT1632.hex: mhvlib-tutorial-HT1632.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex mhvlib-tutorial-HT1632.elf  "mhvlib-tutorial-HT1632.hex"
	@echo 'Finished building: $@'
	@echo ' '

mhvlib-tutorial-HT1632.eep: mhvlib-tutorial-HT1632.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex mhvlib-tutorial-HT1632.elf  "mhvlib-tutorial-HT1632.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: mhvlib-tutorial-HT1632.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega1280 mhvlib-tutorial-HT1632.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EEPROM_IMAGE)$(ELFS)$(FLASH_IMAGE)$(LSS)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) mhvlib-tutorial-HT1632.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
