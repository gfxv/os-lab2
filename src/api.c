#include "cache.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>

int c_open(const char *path) {
  // NOTE: F_NOCACHE for macos, O_DIRECT for linux
  return open(path, O_RDWR | O_SYNC | F_NOCACHE);
}

