# Compiler
CC = gcc

# Flags
CFLAGS = -Wall -g
LDFLAGS = -lncurses

# Directories
SRC_DIR = .
BIN_DIR = bin
OBJ_DIR = obj

# Target executable name
TARGET = $(BIN_DIR)/main

# Source files
SRCS = main.c process.c ui.c lecture_fichier.c
# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(BIN_DIR) $(TARGET)

# Create binary directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Linking target
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean target
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets
.PHONY: all clean

