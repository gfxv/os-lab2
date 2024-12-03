CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -Llib -lcache

SRC_NO_CACHE = src/ema_search_no_cache.c
SRC_WITH_CACHE = src/ema_search_with_cache.c

OBJ_NO_CACHE = $(SRC_NO_CACHE:.c=.o)
OBJ_WITH_CACHE = $(SRC_WITH_CACHE:.c=.o)

EXE_NO_CACHE = ema_search_no_cache
EXE_WITH_CACHE = ema_search_with_cache

LIB_DIR = lib

all: $(EXE_NO_CACHE) $(EXE_WITH_CACHE)

# Build the executable for ema_search_no_cache.c (without cache)
$(EXE_NO_CACHE): $(OBJ_NO_CACHE)
	$(CC) $(CFLAGS) $(OBJ_NO_CACHE) -o $@

# Build the executable for ema_search_with_cache.c (with cache)
$(EXE_WITH_CACHE): $(OBJ_WITH_CACHE)
	$(CC) $(CFLAGS) $(OBJ_WITH_CACHE) -o $@ $(LDFLAGS)

# Rule to compile ema_search_no_cache.c without cache
$(OBJ_NO_CACHE): src/ema_search_no_cache.c
	$(CC) $(CFLAGS) -c src/ema_search_no_cache.c -o $(OBJ_NO_CACHE)

# Rule to compile ema_search_with_cache.c with cache
$(OBJ_WITH_CACHE): src/ema_search_with_cache.c
	$(CC) $(CFLAGS) -c src/ema_search_with_cache.c -o $(OBJ_WITH_CACHE)

clean:
	rm -f $(OBJ_NO_CACHE) $(OBJ_WITH_CACHE) $(EXE_NO_CACHE) $(EXE_WITH_CACHE)

.PHONY: all clean

