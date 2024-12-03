#ifndef CACHE_H
#define CACHE_H

#include <sys/types.h>

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

CacheBlock *find_cache_block(int fd, off_t offset);
void add_cache_block(int fd, off_t offset, const char *data);
ssize_t read_block(int fd, off_t offset, char *buffer);
ssize_t write_block(int fd, off_t offset, const char *data);

#endif // CACHE_H
