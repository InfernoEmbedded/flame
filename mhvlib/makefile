LIBDIR=.
-include $(CURDIR)/common.mk
# Source files

SRCS = $(wildcard *.cpp)

# Output

LSS := libmhvlib.lss
LIB := libmhvlib.a


# Compiler options

CPPFLAGS += -DMHVLIB_CORE=1

all: $(LIB) printsize $(LSS)

$(LIB): $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR Archiver'
	avr-ar -r "$(LIB)" $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

$(LSS): $(LIB)
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S "$(LIB)" > "$(LSS)"
	@echo 'Finished building: $@'
	@echo ' '

printsize: $(LIB)
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=$(MCU) $(LIB)
	@echo 'Finished building: $@'
	@echo ' '

clean:
	-$(RM) $(OBJS) $(DEPS) $(LSS) $(LIB)
	-@echo ' '

.PHONY: all clean
.SECONDARY:

-include $(DEPS)
