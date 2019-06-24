MODULE      := platform

MODULE_SRC  := $(PLATFORM)/src/*.c $(PLATFORM)/$(CONTROLLER)/*.c

GLOBAL_INC  += -I$(MODULE)/$(PLATFORM)/$(CONTROLLER)/ -I$(MODULE)/$(PLATFORM)/

include scripts/make/module.mk
