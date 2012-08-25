# dummy all target (default) we can add to later
all:

# Makefile definitions common to building the library (makefile) and building target projects (project.mk)
OBJS_ = $(SRCS:.cpp=.o)
OBJS__ = $(OBJS_:.c=.o)
OBJS = $(OBJS__:.S=.o)
DEPS_ = $(SRCS:.cpp=.d)
DEPS__ = $(DEPS_:.c=.d)
DEPS = $(DEPS__:.S=.d)

# Check board-specific parameters MCU & clock speed (any of HZ, KHZ or MHZ) before building

ifdef MHZ
	KHZ := $(MHZ)000
endif
ifdef KHZ
	HZ := $(KHZ)000
endif

checkparams:
ifndef MCU
	$(error No MCU (avr model) has been set. You can set one by running make like this: make MCU=atmega328p MHZ=16 )
endif
ifndef HZ
	$(error No HZ, KHZ or MHZ value has been set. You can set one by running make like this: 'make MCU=atmega328p MHZ=16')
endif

CFLAGS += -std=gnu99 -mmcu=$(MCU) -DF_CPU=$(HZ)UL

CPPFLAGS += -I"." -I"$(LIBDIR)" -I"$(LIBDIR)/mhvlib" -Wall -Wextra -g2 -gstabs -Os -fshort-enums -ffunction-sections -fdata-sections -fmerge-constants -fno-caller-saves -fno-dse -funsigned-char -funsigned-bitfields -mmcu=$(MCU) -DF_CPU=$(HZ)UL

CXXFLAGS += -fno-exceptions -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn  -Wno-non-virtual-dtor -std=c++11

DEPFLAGS = -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"

# Tools

RM := rm -rf


# Add MHVAVRTOOLS/bin to the path if it is set and exists as a directory
MHVAVRTOOLS ?=/usr/local/mhvavrtools
ifdef $(findstring $(MHVAVRTOOLS)/bin,$(PATH))
	PATH="$PATH:$MHVAVRTOOLS/bin"
endif

%.o: %.cpp | checkparams
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: %.c | checkparams
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc $(CPPFLAGS) $(CFLAGS) $(DEPFLAGS) -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: %.S | checkparams
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Assembler'
	avr-gcc -x assembler-with-cpp -DF_CPU=$(HZ) -I"." -I"./mhvlib" -mmcu=$(MCU) $(DEPFLAGS) -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

.PHONY: all checkparams
