MODULES                 := target/$(TARGET) arch/$(ARCH)/$(CORE) common kernel \
                           platform/$(PLATFORM) lib driver utils app srv etc

$(foreach module, $(MODULES),$(eval include $(module)/make.mk))
