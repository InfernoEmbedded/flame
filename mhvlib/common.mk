# Makefile definitions common to building the library (makefile) and building target projects (project.mk)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

CPPFLAGS += -Wall -Wextra -g2 -gstabs -Os -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -fno-caller-saves -fno-dse -funsigned-char -funsigned-bitfields -fno-exceptions -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn  -Wno-non-virtual-dtor -std=c++11 -mmcu=$(MCU) -DF_CPU=$(MHZ)000000UL


# Tools

RM := rm -rf

%.o: %.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I"$(LIBDIR)" -I"$(LIBDIR)/mhvlib" $(CPPFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
