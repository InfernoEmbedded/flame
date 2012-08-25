# Include this file in your target project Makefile that uses mhvlib
#
# Look at the makefiles in the tutorial directories for examples of how to include it.
#
LIBDIR ?= "../mhvlib"
include $(LIBDIR)/common.mk

SRCS = $(wildcard *.cpp) $(EXTRA_SRCS)

# Outputs

ELF = $(PROJECT).elf
LSS = $(PROJECT).lss
HEX = $(PROJECT).hex
EEPROM_IMAGE = $(PROJECT).eep
MAP = $(PROJECT).map

OUTPUTS = $(ELF) $(LSS) $(HEX) $(EEPROM_IMAGE) $(HEX)

all: $(OUTPUTS) printsize

$(ELF): $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,$(MAP),--cref -Wl,--gc-sections -L"$(LIBDIR)" -mmcu=$(MCU) -o "$(ELF)" $(OBJS) $(LIBS) -lmhvlib
	@echo 'Finished building target: $@'
	@echo ' '

$(LSS): $(ELF)
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S "$(ELF)"  >"$(LSS)"
	@echo 'Finished building: $@'
	@echo ' '

$(HEX): $(ELF)
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex $(ELF)  "$(HEX)"
	@echo 'Finished building: $@'
	@echo ' '

$(EEPROM_IMAGE): $(ELF)
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex $(ELF)  $(EEPROM_IMAGE)
	@echo 'Finished building: $@'
	@echo ' '

printsize: $(ELF)
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=$(MCU) $(ELF)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS) $(DEPS) $(OUTPUTS)
	-@echo ' '

.PHONY: all clean
.SECONDARY:
