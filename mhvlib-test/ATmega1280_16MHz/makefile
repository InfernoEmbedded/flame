################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include Release/subdir.mk
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
mhvlib-test.lss \

FLASH_IMAGE += \
mhvlib-test.hex \

EEPROM_IMAGE += \
mhvlib-test.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: mhvlib-test.elf secondary-outputs

dependents:
	-cd A:/eclipse/mhvlib/ATmega1280_16MHz && $(MAKE) all

# Tool invocations
mhvlib-test.elf: $(OBJS) $(USER_OBJS) A:/eclipse/mhvlib/ATmega1280_16MHz/libmhvlib.a
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,mhvlib-test.map,--cref -Wl,--gc-sections -L"A:\eclipse\mhvlib/ATmega1280_16MHz" -mmcu=atmega1280 -o"mhvlib-test.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

mhvlib-test.lss: mhvlib-test.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S mhvlib-test.elf  >"mhvlib-test.lss"
	@echo 'Finished building: $@'
	@echo ' '

mhvlib-test.hex: mhvlib-test.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex mhvlib-test.elf  "mhvlib-test.hex"
	@echo 'Finished building: $@'
	@echo ' '

mhvlib-test.eep: mhvlib-test.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex mhvlib-test.elf  "mhvlib-test.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: mhvlib-test.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega1280 mhvlib-test.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EEPROM_IMAGE)$(ELFS)$(FLASH_IMAGE)$(LSS)$(S_DEPS)$(CXX_DEPS)$(S_UPPER_DEPS)$(C++_DEPS)$(ASM_DEPS)$(CC_DEPS)$(CPP_DEPS)$(C_UPPER_DEPS)$(SIZEDUMMY) mhvlib-test.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:
A:/eclipse/mhvlib/ATmega1280_16MHz/libmhvlib.a:

-include ../makefile.targets
