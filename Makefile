

include local.mk
include scripts/make/init.mk

.DEFAULT_GOAL := all

.PHONY : all
all : $(PROJECT_BINARY)

$(PROJECT_BINARY) : load_modules
	$(foreach src, $(GLOBAL_SRCS), $(shell $(CC) $(CC_FLAGS) $(GLOBAL_INC) -c $(src) -o $(addprefix $(BUILD_DIR)/$(firstword \
        $(subst /, ,$(src)))/,$(notdir $(src:.c=.o))) || rm -f $(addprefix $(BUILD_DIR)/$(firstword \
            $(subst /, ,$(src)))/,$(notdir $(src:.c=.o)))))
	$(LD) $(GLOBAL_OBJS) $(LD_FLAGS) -o $(PROJECT_ELF)
	$(OC) -O binary $(PROJECT_ELF) $(PROJECT_BINARY)

.PHONY : test
test : flush
	@$(SERIAL_COMMUNICATION) $(SERIAL_DEVICE)

.PHONY : flush
flush : $(PROJECT_BINARY)
	@$(FLASH_TOOL) write $(PROJECT_BINARY) $(FLASH_ADDR) &> /dev/null

.PHONY : clean
clean:
	@rm -rf $(BUILD_DIR)

.PHONY : dump
dump : $(PROJECT_BINARY)
	$(OD) -d $(PROJECT_ELF) > $(PROJECT_DUMP)
	less $(PROJECT_DUMP)

.PHONY : pre-build
pre-build :
	@mkdir -p $(BUILD_DIR)
