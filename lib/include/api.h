#ifndef API_H
#define API_H

#include <stddef.h>
#include <sys/types.h>

int c_open(const char *path);
int c_close(int fd);
off_t align_offset(off_t offset);
ssize_t c_read(int fd, void *buf, size_t count);
ssize_t c_write(int fd, const void *buf, size_t count);
int c_fsync(int fd);
off_t c_lseek(int fd, off_t offset, int whence);

#endif // API_H
