OUT := test
LIB := lib/libtui.a

SRC_DIR := src
OBJ_DIR := build
INCLUDE_DIR := include

C := g++
AR := ar
CFLAGS := -g -I$(INCLUDE_DIR) -std=c++23 -MMD -MP -c
LDFLAGS := -llua
LDOUT := -o $(OUT)

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o,$(SRCS))

-include $(OBJS:.o=.d)

all: $(OUT) $(LIB)

compile: all

$(OUT): $(OBJS)
	$(C) $(OBJS) $(LDOUT) $(LDFLAGS)

$(LIB): $(OBJS)
	@mkdir -p $(dir $@)
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(C) $(CFLAGS) $< -o $@

run: $(OUT)
	./$(OUT)

clean:
	@rm -rf $(OBJ_DIR) $(OUT) $(LIB)

valgrind: $(OUT)
	valgrind ./$(OUT)

.PHONY: all compile clean run valgrind
