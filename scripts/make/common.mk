MODULES                 := target/$(TARGET) arch/$(ARCH)/$(CORE) common kernel \
                           platform/$(PLATFORM) lib driver utils app srv

.PHONY : load_modules
load_modules : variables
	$(foreach module, $(MODULES),$(eval include $(module)/make.mk))
