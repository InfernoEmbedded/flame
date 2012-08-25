# include this makefile from a project that uses the VUSB Console driver
#
# For an example, see mhvlib-tutorial-Vusb-Console/makefile

EXTRA_SRCS += $(wildcard $(VUSBDIR)/*.cpp) $(wildcard $(VUSBDIR)/vusb/*.c) $(wildcard $(VUSBDIR)/vusb/*.S)
CPPFLAGS += -I$(VUSBDIR)
