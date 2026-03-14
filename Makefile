
CC ?= gcc
CCFLAGS := -g -fdiagnostics-color=always -MMD -MP

SRC_DIR := ./src
BUILD_DIR := ./build

ASSETS_SRC := ./data
ASSETS_DST := $(BUILD_DIR)/data

#LOC_INC_DIRS := ${shell find ./include -type d}
#LOC_INCS := $(foreach dir,$(LOC_INC_DIRS),-I$(dir))
INC := -I ./include -I ./src/util -I ./libs/stb -I ./libs/cJSON
#LDFLAGS := -L./libs/raylib/lib/
#LDLIBS := -lm

SRCS = $(shell find $(SRC_DIR) -iname "*.c")

ifeq ($(MAKECMDGOALS), install)
TARGET_NAME := kscout
SRCS += ./libs/cJSON/cJSON.c 
else ifeq ($(MAKECMDGOALS), test)
TARGET_NAME := test_kscout
SRCS := $(filter-out $(SRC_DIR)/main.c, $(SRCS))
SRCS += $(shell find ./test -iname "*.c")
SRCS += $(shell find ./libs/unity -iname "*.c")
INC += -I ./libs/unity
endif
$(info sources = $(SRCS))
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

install: assets $(TARGET_NAME)
test: $(TARGET_NAME)

$(TARGET_NAME): $(OBJS)
	$(CC) $(INC) $^ -o $(BUILD_DIR)/$(TARGET_NAME)
#$(LDFLAGS) $(LDLIBS)

#VPATH = $(SRC_DIR):./test:./libs/unity

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CCFLAGS) $(INC) -c -o $@ $<

-include $(DEPS)

.PHONY: assets
assets:
	mkdir -p $(ASSETS_DST)
	cp -r $(ASSETS_SRC)/* $(ASSETS_DST)/
	$(info assets)

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)