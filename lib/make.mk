MODULE      := lib

MODULE_SRC  := src/*.c

LIB_SRC     := $(MODULE)/src/syscall.c $(MODULE)/src/list.c $(MODULE)/src/parser.c

LIB_NAME    := $(PROJECT)

include scripts/make/module.mk
include scripts/make/lib.mk

