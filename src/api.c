#include "cache.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>

int c_open(const char *path) {
  // NOTE: F_NOCACHE for macos, O_DIRECT for linux
  return open(path, O_RDWR | O_SYNC | F_NOCACHE);
}

int c_close(int fd) {
  // remove fd from cache before closing it
  CacheBlock *block = cache.head;
  while (block != NULL) {
    if (block->fd == fd) {

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
      break;
    }
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
  off_t offset = lseek(fd, 0, SEEK_CUR);
  ssize_t bytes_read = 0;
  size_t remaining = count;

  while (remaining > 0) {
    // align the offset to the block boundary
    off_t block_offset = align_offset(offset);
    char block[BLOCK_SIZE];

    ssize_t bytes_in_block = read_block(fd, block_offset, block);
    if (bytes_in_block <= 0)
      return bytes_in_block;

    // copy data from the block to the buffer (either remaining or block size)
    size_t to_copy = remaining < bytes_in_block ? remaining : bytes_in_block;

    memcpy(buf + bytes_read, block + (offset - block_offset), to_copy);

    bytes_read += to_copy;
    remaining -= to_copy;
    offset += to_copy;
  }

  return bytes_read;
}

ssize_t c_write(int fd, const void *buf, size_t count) {
  off_t offset = lseek(fd, 0, SEEK_CUR);
  ssize_t bytes_written = 0;
  size_t remaining = count;

  while (remaining > 0) {
    // align the offset to the block boundary
    off_t block_offset = align_offset(offset);
    char block[BLOCK_SIZE];

    // copy data into block buffer (either remaining or full block size)
    size_t to_copy = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
    memcpy(block, buf + bytes_written, to_copy);

    ssize_t written = write_block(fd, block_offset, block);
    if (written <= 0)
      return written;

    bytes_written += to_copy;
    remaining -= to_copy;
    offset += to_copy;
  }

  return bytes_written;
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
