# Source files

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(SRCS:.cpp=.d)

# Output

LSS := libmhvlib.lss
LIB := libmhvlib.a


# Compiler options

CPPFLAGS := -DMHVLIB_CORE=1 -Wall -Wextra -g2 -gstabs -Os -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -fno-caller-saves -fno-dse -funsigned-char -funsigned-bitfields -fno-exceptions -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn  -Wno-non-virtual-dtor -std=c++11 -mmcu=$(MCU) -DF_CPU=$(MHZ)000000UL


# Tools

RM := rm -rf


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

%.o: %.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"." -I"./mhvlib" $(CPPFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

clean:
	-$(RM) $(OBJS) $(DEPS) $(LSS) $(LIB)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include $(DEPS)
