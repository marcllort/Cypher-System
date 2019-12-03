#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

int IO_openFile(const char *filename);
long IO_readUntil(int fd, char **data, char delimiter);
long IO_readUntilv2(int fd, char **data, char delimiter);
long IO_write(int fd, char *data, long size);
int IO_checkEOF(int fd);

#endif
