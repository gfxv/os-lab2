#include "../include/cache.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

Cache cache = {NULL, NULL, 0};

void status() {
  printf("### CACHE INFO START ###\n");
  printf("cache size: %d\n", cache.current_size);

  CacheBlock *block = cache.head;
  while (block != NULL) {
    printf("[fd: %d, offset: %lld]\n", block->fd, block->offset);
    printf("data: %s\n", block->data);
    block = block->next;
  }
  printf("### CACHE INFO END ###\n");
}

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
  if (cache.current_size >= CACHE_SIZE) {
    evict_block();
  }

  // create new cache block
  CacheBlock *new_block = (CacheBlock *)malloc(sizeof(CacheBlock));
  new_block->fd = fd;
  new_block->offset = offset;
  memcpy(new_block->data, data, BLOCK_SIZE);

  insert_block(new_block);
}

void insert_block(CacheBlock *new_block) {
  new_block->next = NULL;
  new_block->prev = cache.tail;
  if (cache.tail)
    cache.tail->next = new_block;
  cache.tail = new_block;
  if (cache.head == NULL)
    cache.head = new_block;
  cache.current_size++;
}

void evict_block() {
  CacheBlock *old_block = cache.head;
  if (!old_block) {
    return;
  }

  // write the data of the oldest block back to disk to persist any changes
  pwrite(old_block->fd, old_block->data, BLOCK_SIZE, old_block->offset);

  if (cache.head == cache.tail) {
    // Only one cache block
    cache.head = NULL;
    cache.tail = NULL;
  } else {
    // More than one cache block
    cache.head = cache.head->next;
    cache.head->prev = NULL;
  }
  free(old_block);
  cache.current_size--;
}
