# include this makefile from a project that uses the VUSB Keyboard driver
#
# For an example, see flame-tutorial-Vusb-Keyboard/makefile

EXTRA_SRCS += $(wildcard $(VUSBDIR)/*.cpp) $(wildcard $(VUSBDIR)/vusb/*.c) $(wildcard $(VUSBDIR)/vusb/*.S)
CPPFLAGS += "-I$(VUSBDIR)"
