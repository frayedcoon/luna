MODULES                 := target/$(TARGET) arch/$(ARCH)/$(CORE) common kernel \
                           platform/$(PLATFORM) lib driver utils app srv

$(foreach module, $(MODULES),$(eval include $(module)/make.mk))
