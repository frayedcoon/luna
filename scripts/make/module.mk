SRC            := $(wildcard $(addprefix $(MODULE)/,$(MODULE_SRC)))

GLOBAL_SRCS    += $(SRC)
GLOBAL_OBJS    += $(foreach src, $(SRC), \
                  $(patsubst %.c,%.o,$(BUILD_DIR)/$(src)))

MODULE_DIR     := $(BUILD_DIR)/$(MODULE)
