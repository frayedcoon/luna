include local.mk
include scripts/make/init.mk

.DEFAULT_GOAL := all

.PHONY : all
all : $(PROJECT_BINARY)

$(PROJECT_BINARY) : $(GLOBAL_OBJS) $(LIBS)
	@echo Linking $(PROJECT_BINARY)
	@$(LD) $(GLOBAL_OBJS) $(LD_FLAGS) -o $(PROJECT_ELF)
	@$(OC) -O binary $(PROJECT_ELF) $(PROJECT_BINARY)

.PHONY : test
test : flush
	@$(SERIAL_COMMUNICATION) $(SERIAL_DEVICE)

.PHONY : flush
flush : $(PROJECT_BINARY)
	@$(FLASH_TOOL) $(PROJECT_BINARY) $(FLASH_ADDR) &> /dev/null

.PHONY : clean
clean:
	@rm -rf $(BUILD_DIR)

.PHONY : dump
dump : $(PROJECT_BINARY)
	$(OD) -d $(PROJECT_ELF) > $(PROJECT_DUMP)
	less $(PROJECT_DUMP)

