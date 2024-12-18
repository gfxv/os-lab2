#ifndef CACHE_H
#define CACHE_H

#include <sys/types.h>

#define BLOCK_SIZE 4096
#define CACHE_SIZE 16

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

extern Cache cache; 

void status();
void evict_block();
void insert_block(CacheBlock*);
CacheBlock *find_cache_block(int fd, off_t offset);
void add_cache_block(int fd, off_t offset, const char *data);

#endif // CACHE_H
