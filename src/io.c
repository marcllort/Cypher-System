#include "../libs/io.h"

// Funcions relacionades amb IO

int IO_openFile(const char *filename)
{ //Obrim fitxer
    return open(filename, O_RDWR);
}

int IO_createFile(const char *filename)
{ // Creem arxiu
    return open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
}

long IO_read(int fd, char **data, long size)
{ //Llegim de un fd
    *data = (char *)malloc(sizeof(char) * size);
    return read(fd, *data, size);
}

long IO_readUntil(int fd, char **data, char delimiter)
{ //Llegim de un fd fins un cert delimitador
    size_t size = 0;
    ssize_t bytes;
    *data = NULL;

    do
    {
        *data = (char *)realloc((void *)*data, ++size);
        bytes = read(fd, (*data) + size - 1, 1);
    } while ((*data)[size - 1] != delimiter && bytes);

    if (!bytes)
    {
        *data = (char *)realloc((void *)*data, ++size);
    }

    (*data)[size - 2] = '\0';

    return size;
}

long IO_readUntilv2(int fd, char **data, char delimiter)
{ //Llegim de un fd fins un cert delimitador
    size_t size = 0;
    ssize_t bytes;
    *data = NULL;

    do
    {
        *data = (char *)realloc((void *)*data, ++size);
        bytes = read(fd, (*data) + size - 1, 1);
    } while ((*data)[size - 1] != delimiter && bytes);

    if (!bytes)
    {
        *data = (char *)realloc((void *)*data, ++size);
    }

    (*data)[size - 1] = '\0';

    return size;
}

long IO_write(int fd, char *data, long size)
{ // Escrivim  a un fd
    int curr = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_END);

    long status = write(fd, data, size);
    lseek(fd, curr, SEEK_SET);

    return status;
}

int IO_close(int fd)
{ // Tanquem arxiu
    return close(fd);
}

int IO_deleteFile(const char *filename)
{ // Borrem fitxer
    return unlink(filename);
}

inline int checkEOF(int fd)
{   // Mirem si hem arribat al final de un fitxer
    char c;
    ssize_t n = read(fd, &c, sizeof(char));

    if (n == 0)
        return 1;

    lseek(fd, -1, SEEK_CUR);
    return 0;
}
