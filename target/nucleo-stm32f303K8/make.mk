MODULE                  := target

GLOBAL_INC  += -I$(MODULE)/$(TARGET)/

include scripts/make/module.mk
