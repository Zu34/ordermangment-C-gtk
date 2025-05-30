# Project settings
CC = gcc
CFLAGS = -Wall -g -Iinclude `pkg-config --cflags gtk+-3.0 sqlite3`
LDFLAGS = `pkg-config --libs gtk+-3.0 sqlite3`
SRC_DIR = src
BUILD_DIR = build
TARGET = $(BUILD_DIR)/oms

# All .c files in src/
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(BUILD_DIR) $(TARGET)

# Link object files to create executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Create build directory if not exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
