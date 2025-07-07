# Compiler and flags
CC = gcc
CFLAGS =  -Iinclude -MD -MP
LDFLAGS = -lm -mconsole

# Directories
SRC_DIR = src
TEST_DIR = test
DEMO_DIR = demo
BUILD_DIR = build

# Source files
SRC_SOURCES = $(wildcard $(SRC_DIR)/*.c)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
DEMO_SOURCES = $(wildcard $(DEMO_DIR)/*_main.c)

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRC_SOURCES))
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.o, $(TEST_SOURCES))
DEMO_OBJECTS = $(patsubst $(DEMO_DIR)/%.c, $(BUILD_DIR)/%.o, $(DEMO_SOURCES))

# Executables
TEST_EXECUTABLES = $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.exe, $(TEST_SOURCES))
DEMO_EXECUTABLES = $(patsubst $(DEMO_DIR)/%_main.c, $(BUILD_DIR)/%_demo.exe, $(DEMO_SOURCES))

# Phony targets
.PHONY: all clean

# Default target
all: $(BUILD_DIR) $(DEMO_EXECUTABLES) $(TEST_EXECUTABLES)

# Create build directory
$(BUILD_DIR):
	mkdir $(BUILD_DIR)

# Link demo executables
$(BUILD_DIR)/%_demo.exe: $(BUILD_DIR)/%_main.o $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

# Link test executables
$(BUILD_DIR)/%.exe: $(BUILD_DIR)/%.o $(OBJECTS)
	$(CC) $^ $(LDFLAGS) -o $@

# Compile source files in src/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile main files in demo/
$(BUILD_DIR)/%.o: $(DEMO_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test files in test/
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rmdir /s /q $(BUILD_DIR)

# Include dependency files
-include $(OBJECTS:.o=.d)
-include $(TEST_OBJECTS:.o=.d)
-include $(DEMO_OBJECTS:.o=.d)
