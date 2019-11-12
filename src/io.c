#include "../libs/io.h"

int IO_openFile(const char* filename)
{
    return open(filename, O_RDWR);
}

int IO_createFile(const char* filename)
{
    return open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
}

long IO_read(int fd, char **data, long size)
{
    *data = (char*) malloc(sizeof(char) * size);
    return read(fd, *data, size);
}

long IO_readUntil(int fd, char **data, char delimiter)
{
    size_t size = 0;
    ssize_t bytes;
    *data = NULL;

    do {
        *data = (char*) realloc((void*) *data, ++size);
        bytes = read(fd, (*data) + size - 1, 1);
    } while ((*data)[size - 1] != delimiter && bytes);

    if (!bytes) {
        *data = (char*)realloc((void*)*data, ++size);
    }
    
    (*data)[size - 2] = '\0';
    
    return size;
}

long IO_readUntilv2(int fd, char **data, char delimiter)
{
    size_t size = 0;
    ssize_t bytes;
    *data = NULL;

    do {
        *data = (char*) realloc((void*) *data, ++size);
        bytes = read(fd, (*data) + size - 1, 1);
    } while ((*data)[size - 1] != delimiter && bytes);

    if (!bytes) {
        *data = (char*)realloc((void*)*data, ++size);
    }
    
    (*data)[size - 1] = '\0';
    
    return size;
}

long IO_write(int fd, char* data, long size)
{
    int curr = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_END);

    long status = write (fd, data, size);
    lseek(fd, curr, SEEK_SET);

    return status;
}

int IO_close(int fd)
{
    return close(fd);
}

int IO_deleteFile(const char* filename)
{
    return unlink(filename);
}

inline int checkEOF(int fd) {
    char c;
    ssize_t n = read(fd, &c, sizeof(char));

    if (n == 0) return 1;

    lseek(fd, -1, SEEK_CUR);
    return 0;

}


