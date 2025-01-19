# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -Wall -Wextra

# Libraries
LIBS = -lSDL2

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.cc)
OBJS = $(patsubst $(SRC_DIR)/%.cc, $(BUILD_DIR)/%.o, $(SRCS))

# Target executable
TARGET = ray-casting

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LIBS)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cc | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Ensure the build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run
