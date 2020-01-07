#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>

#define FRAGMENT_SIZE 8192
#define ERR_MUTEX "Error creating mutex\n"

int IO_openFile(const char *filename);
long IO_readUntil(int fd, char **data, char delimiter);
long IO_readUntilv2(int fd, char **data, char delimiter);
long IO_read(int fd, char **data, long size);
int IO_initMutex();
long IO_write(int fd, char *data, long size);
int IO_close(int fd);
int IO_checkEOF(int fd);

#endif
