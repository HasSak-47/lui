# Output binary and static library
OUT := test
LIB := lib/libtui.a

# Directories
SRC_DIR := src
BUILD_DIR := build
INCLUDE_DIR := include

# Tools and flags
C := ccache g++
AR := ar
CFLAGS := -g -I"$(INCLUDE_DIR)" -std=c++23 -MMD -MP -c
LDFLAGS := -llua
LDOUT := -o "$(OUT)"

# Source file collection
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
MAIN_SRC := $(SRC_DIR)/main.cpp
LIB_SRCS := $(filter-out $(MAIN_SRC), $(SRCS))

# Object file mappings
MAIN_OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(MAIN_SRC))
LIB_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(LIB_SRCS))

# Dependency includes
-include $(LIB_OBJS:.o=.d) $(MAIN_OBJ:.o=.d)

# Phony targets
.PHONY: all build compile clean run valgrind libonly

# Default build
all: $(OUT) $(LIB)

build: all
compile: all

# Final binary links main.o + library objects
$(OUT): $(LIB_OBJS) $(MAIN_OBJ)
	$(C) $(LIB_OBJS) $(MAIN_OBJ) $(LDOUT) $(LDFLAGS)

# Static library: only non-main objects
$(LIB): $(LIB_OBJS)
	@mkdir -p "$(dir $@)"
	$(AR) rcs "$@" $^

# Compile rule for object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p "$(dir $@)"
	$(C) $(CFLAGS) $< -o $@

# Run the binary
run: $(OUT)
	./$(OUT)

# Run with Valgrind
valgrind: $(OUT)
	valgrind ./$(OUT)

# Build only the library (no binary)
libonly: $(LIB)

# Cleanup
clean:
	@rm -rf "$(BUILD_DIR)" "$(OUT)" "$(LIB)"
