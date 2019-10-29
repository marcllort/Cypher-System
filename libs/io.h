#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

int IO_openFile(const char* filename);
int IO_createFile(const char* filename);
long IO_read(int fd, char **data, long size);
long IO_readUntil(int fd, char **data, char delimiter);
long IO_write(int fd, char* data, long size);
int IO_close(int fd);
int IO_deleteFile(const char* filename);

#endif
