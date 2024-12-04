CC = gcc
CFLAGS = -Wall -g
LIB_DIR = lib
LDFLAGS = -L$(LIB_DIR) -lcache

SRC_NO_CACHE = src/ema_search_no_cache.c
SRC_WITH_CACHE = src/ema_search_with_cache.c

OBJ_NO_CACHE = $(SRC_NO_CACHE:.c=.o)
OBJ_WITH_CACHE = $(SRC_WITH_CACHE:.c=.o)

EXE_NO_CACHE = ema_search_no_cache
EXE_WITH_CACHE = ema_search_with_cache

all: $(EXE_NO_CACHE) $(EXE_WITH_CACHE)

$(EXE_NO_CACHE): $(OBJ_NO_CACHE)
	$(CC) $(CFLAGS) -o $@ $^

$(EXE_WITH_CACHE): $(OBJ_WITH_CACHE)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_NO_CACHE): src/ema_search_no_cache.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_WITH_CACHE): src/ema_search_with_cache.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_NO_CACHE) $(OBJ_WITH_CACHE) $(EXE_NO_CACHE) $(EXE_WITH_CACHE)

.PHONY: all clean

