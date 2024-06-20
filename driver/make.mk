MODULE      := driver

MODULE_SRC  := src/*.c fs/src/*.c

GLOBAL_INC  += -I$(MODULE)/

include scripts/make/module.mk
