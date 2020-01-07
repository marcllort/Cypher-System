#include "../libs/io.h"

pthread_mutex_t mutex;

// Funcions relacionades amb IO

int IO_openFile(const char *filename)
{
    //Obrim fitxer
    return open(filename, O_RDWR);
}

long IO_readUntil(int fd, char **data, char delimiter)
{
    //Llegim de un fd fins un cert delimitador, posem /0 a size-2
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
{
    //Llegim de un fd fins un cert delimitador, posem /0 a size-1
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

long IO_read(int fd, char **data, long size)
{
    //Llegim size bits de un fd
    *data = (char *)malloc(sizeof(char) * size);
    return read(fd, *data, size);
}

//Funcio per inicialitzar el semàfor que controla l'esciptura per pantalla
int IO_initMutex()
{
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        IO_write(1, ERR_MUTEX, strlen(ERR_MUTEX));
        return 1;
    }
    return 0;
}

long IO_write(int fd, char *data, long size)
{
    if (fd == 1)
    {
        pthread_mutex_lock(&mutex);
    }
    // Escrivim  a un fd
    int curr = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_END);

    long status = write(fd, data, size);
    lseek(fd, curr, SEEK_SET);
    if (fd == 1)
    {
        pthread_mutex_unlock(&mutex);
    }
    return status;
}

int IO_close(int fd)
{
    return close(fd);
}

inline int IO_checkEOF(int fd)
{
    // Mirem si hem arribat al final de un fitxer
    char c;
    ssize_t n = read(fd, &c, sizeof(char));

    if (n == 0)
        return 1;

    lseek(fd, -1, SEEK_CUR);
    return 0;
}
