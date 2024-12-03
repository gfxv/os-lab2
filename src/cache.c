#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BLOCK_SIZE 4096
#define CACHE_SIZE 8

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

CacheBlock *find_cache_block(int fd, off_t offset) {
  CacheBlock *block = cache.head;
  while (block != NULL) {
    if (block->fd == fd && block->offset == offset) {
      return block;
    }
    block = block->next;
  }
  return NULL;
}
