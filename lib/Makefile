# Compiler and linker settings
CC = gcc
CFLAGS = -D_GNU_SOURCE -Wall -Wextra -fPIC
LDFLAGS = -shared

# File names
LIB_NAME = libcache.so
SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)   # Get all .c files in the src directory
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)  # Object files corresponding to the source files

# Include directories
INCLUDES = -I$(INCLUDE_DIR)

# Targets
all: $(LIB_NAME)

# Rule to create the shared library (.so)
$(LIB_NAME): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# Rule to compile .c files into .o object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# Clean the build
clean:
	rm -f $(OBJ_DIR)/*.o $(LIB_NAME)

# Rebuild everything
rebuild: clean all

.PHONY: all clean rebuild

