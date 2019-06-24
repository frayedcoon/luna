PROJECT                 := luna

SCRIPT_DIR              := scripts
BUILD_DIR               := build

USER_VARIABLES          += CROSS_TOOL
USER_VARIABLES          += TARGET
USER_VARIABLES          += SERIAL_DEVICE
USER_VARIABLES          += SERIAL_COMMUNICATION

.PHONY : variables
variables : pre-build
	$(foreach var, $(USER_VARIABLES), \
        $(if $(value $(var)), , $(error "Undefined $(var), please provide it in local.mk")))

CC                       := $(CROSS_TOOL)gcc
AS                       := $(CROSS_TOOL)as
LD                       := $(CROSS_TOOL)ld
OC                       := $(CROSS_TOOL)objcopy
OD                       := $(CROSS_TOOL)objdump

PROJECT_BINARY           := $(BUILD_DIR)/$(PROJECT).bin
PROJECT_ELF              := $(BUILD_DIR)/$(PROJECT).elf
PROJECT_MAP              := $(BUILD_DIR)/$(PROJECT).map
PROJECT_DUMP             := $(BUILD_DIR)/$(PROJECT).dump

TARGET_DIR               := target/$(TARGET)

GLOBAL_SRCS              :=
GLOBAL_OBJS              :=

GLOBAL_INC               := -I.

include $(TARGET_DIR)/target.mk

include $(SCRIPT_DIR)/make/common.mk

LD_SCRIPT               := $(SCRIPT_DIR)/ld/$(TARGET).ld

CC_FLAGS                := -mcpu=cortex-$(CORE) -mthumb -g -ffreestanding \
                           -std=gnu99 -fomit-frame-pointer -Werror \
                           -Wall -Wextra -mfloat-abi=hard -mapcs-frame \
                           -mlittle-endian
LD_FLAGS                := -T $(LD_SCRIPT) --cref \
                           -Map $(PROJECT_MAP)
