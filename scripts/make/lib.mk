LIBS += $(BUILD_DIR)/$(MODULE)/lib$(LIB_NAME).so

LIB_MAP       := $(BUILD_DIR)/$(MODULE)/lib$(LIB_NAME).map

LIB_LD_SCRIPT := scripts/ld/lib$(LIB_NAME).ld

LIB_LD_FLAGS  := -Wl,-T,$(LIB_LD_SCRIPT),--cref,-Map,$(LIB_MAP)

$(BUILD_DIR)/$(MODULE)/lib$(LIB_NAME).so: $(LIB_SRC)
	@mkdir -p $(dir $@)
	@echo Composing $(notdir $@)
	@$(CC) $(CC_FLAGS) $(LIB_CC_FLAGS) $(LIB_LD_FLAGS) $(GLOBAL_INC) $^ -o $@