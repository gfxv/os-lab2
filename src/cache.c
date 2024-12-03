#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BLOCK_SIZE 4096 // Size of each block in bytes (4 KB)
#define CACHE_SIZE 8    // Number of blocks that can be cached

typedef struct CacheBlock {
  int fd;
  off_t offset;
  char data[BLOCK_SIZE];
  struct CacheBlock *prev;
  struct CacheBlock *next;
} CacheBlock;

typedef struct {
  CacheBlock *head; // points to the oldest block
  CacheBlock *tail; // points to the most recently added block
  int current_size;
} Cache;

Cache cache = {NULL, NULL, 0}; // initialize empty cache

