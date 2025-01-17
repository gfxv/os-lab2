#include "../include/api.h"
#include "../include/cache.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int c_open(const char *path) {
  int flags = O_RDWR | O_SYNC;

  // NOTE: F_NOCACHE for macos, O_DIRECT for linux
#if defined(__APPLE__)
  flags |= F_NOCACHE;

#elif defined(__linux__)
  flags |= O_DIRECT;
#endif

  return open(path, flags);
}

int c_close(int fd) {
  // remove fd from cache before closing it
  CacheBlock *block = cache.head;
  while (block != NULL) {
    if (block->fd != fd) {
      continue;
    }

    // if the block is at the head of the cache
    if (block == cache.head) {
      cache.head = block->next;
      if (cache.head)
        cache.head->prev = NULL;
    }

    // if the block is at the tail of the cache
    if (block == cache.tail) {
      cache.tail = block->prev;
      if (cache.tail)
        cache.tail->next = NULL;
    }

    // if the block is somewhere in the middle
    if (block->prev != NULL) {
      block->prev->next = block->next;
    }
    if (block->next != NULL) {
      block->next->prev = block->prev;
    }

    free(block);
    cache.current_size--;
    block = block->next;
  }
  return close(fd);
}

// align to block boundary
off_t align_offset(off_t offset) {
  // ensures that the offset is rounded
  // down to the nearest multiple of the BLOCK_SIZE
  //
  // guarantees that the data we read starts from the
  // beginning of a block and avoids partial block reads
  return offset & ~(BLOCK_SIZE - 1);
}

ssize_t c_read(int fd, void *buf, size_t count) {
  size_t remaining = count;
  size_t copied = 0;
  off_t f_offset = lseek(fd, 0, SEEK_CUR);
  if (f_offset == (off_t)-1) {
    return -1;
  }

  while (remaining > 0) {
    off_t offset = align_offset(f_offset);
    size_t copy_size = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
    CacheBlock *found = find_cache_block(fd, offset);
    if (found) {
      // cache hit
      off_t inner_padding = f_offset - offset;
      int left = BLOCK_SIZE - f_offset;
      copy_size = left < remaining ? left : remaining;
      memcpy(buf + copied, &(found->data[inner_padding]), copy_size);
    } else {
      // cache miss
      char data[BLOCK_SIZE];
      if (pread(fd, data, BLOCK_SIZE, offset) == -1) {
        return -1;
      }
      add_cache_block(fd, offset, data);
      memcpy(buf + copied, data, copy_size);
    }
    copied += copy_size;
    remaining -= copy_size;
    f_offset += copy_size;
  }

  return copied;
}

ssize_t c_write(int fd, const void *buf, size_t count) {
  size_t remaining = count;
  size_t copied = 0;
  off_t f_offset = lseek(fd, 0, SEEK_CUR);
  if (f_offset == (off_t)-1) {
    return -1;
  }

  while (remaining > 0) {
    off_t offset = align_offset(f_offset);
    size_t copy_size = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
    CacheBlock *found = find_cache_block(fd, offset);
    if (found) {
      // cache hit
      off_t inner_padding = f_offset - offset;
      int left = BLOCK_SIZE - f_offset;
      copy_size = left < remaining ? left : remaining;
      memcpy(&(found->data[inner_padding]), buf + copied, copy_size);
    } else {
      // cache miss
      char data[BLOCK_SIZE];
      if (pwrite(fd, buf, count, f_offset) == -1) {
        return -1;
      }
      memcpy(data, buf + copied, copy_size);
      add_cache_block(fd, offset, data);
    }
    copied += copy_size;
    remaining -= copy_size;
    f_offset += copy_size;
  }

  return copied;
}

int c_fsync(int fd) {
  // write all blocks back to disk
  CacheBlock *block = cache.head;
  while (block != NULL) {
    pwrite(block->fd, block->data, BLOCK_SIZE, block->offset);
    block = block->next;
  }
  return fsync(fd);
}

// ???
off_t c_lseek(int fd, off_t offset, int whence) {
  return lseek(fd, offset, whence);
}
