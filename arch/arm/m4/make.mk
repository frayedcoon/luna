MODULE      := arch

MODULE_SRC  := $(ARCH)/$(CORE)/src/*.c

GLOBAL_INC  += -I$(MODULE)/$(ARCH)/$(CORE)

include scripts/make/module.mk
