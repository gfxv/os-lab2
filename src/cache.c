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

void add_cache_block(int fd, off_t offset, const char *data) {
  // check if cache is full
  if (cache.current_size >= CACHE_SIZE) {
    CacheBlock *old_block = cache.head;
    if (old_block) {

      // write the data of the oldest block back to disk to persist any changes
      pwrite(old_block->fd, old_block->data, BLOCK_SIZE, old_block->offset);

      // remove the oldest block
      cache.head = old_block->next;
      if (cache.head)
        cache.head->prev = NULL;
      if (cache.tail == old_block)
        cache.tail = NULL;
      free(old_block);
      cache.current_size--;
    }
  }

  // create and add the new block to the tail (most recent)
  CacheBlock *new_block = (CacheBlock *)malloc(sizeof(CacheBlock));
  new_block->fd = fd;
  new_block->offset = offset;
  memcpy(new_block->data, data, BLOCK_SIZE);

  new_block->next = NULL;
  new_block->prev = cache.tail;
  if (cache.tail)
    cache.tail->next = new_block;
  cache.tail = new_block;
  if (cache.head == NULL)
    cache.head = new_block;
  cache.current_size++;
}
