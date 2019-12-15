#include "../libs/packet.h"

Packet PACKET_read(int fd)
{
    // Funcio encarregada de la lectura de un paquet
    Packet pd;
    int headerLength;

    // En cas de que sigui un fitxer d'un tipus diferent als que tractem, detectem error
    int error = read(fd, &pd.type, 1);
    if (pd.type != 0x01 && pd.type != 0x02 && pd.type != 0x03 && pd.type != 0x04 && pd.type != 0x05 && pd.type != 0x06)
    {
        headerLength = -1;
        return pd;
    }

    if (error <= 0)
    {
        headerLength = -1;
        return pd;
    }

    headerLength = 0;
    pd.header = (char *)malloc(sizeof(char));

    // Fem un bucle per anar llegint bit a bit el header, ja que la seva longitud es variable
    do
    {
        pd.header = (char *)realloc((void *)pd.header, ++headerLength * sizeof(char));
        if (read(fd, &pd.header[headerLength - 1], 1) <= 0)
            return PACKET_destroy(&pd);
    } while (pd.header[headerLength - 1] != ']');

    pd.header[headerLength] = '\0';

    // Llegim la longitud de el camp data
    if (read(fd, &pd.length, sizeof(uint16_t)) <= 0)
    {
        pd.length = 0;
    }
    if (pd.length != 0)
    {
        // En cas de que no sigui 0, llegim el camp data amb el lenght llegit anteriorment
        pd.data = (char *)malloc(sizeof(char) * pd.length);
        if (read(fd, pd.data, pd.length) <= 0)
        {
            pd.data = NULL;
        }
        pd.data[pd.length] = 0;
    }
    else
    {
        pd.data = NULL;
    }

    return pd;
}

int PACKET_write(Packet pd, int fd)
{
    // Funcio per enviar un paquet per parts

    IO_write(fd, &pd.type, 1);
    IO_write(fd, pd.header, UTILS_sizeOf(pd.header));

    int error = write(fd, &pd.length, sizeof(uint16_t));
    if (error < 0)
    {
        return -1;
    }
    if (pd.length != 0)
    {
        IO_write(fd, pd.data, pd.length);
    }

    return 0;
}

Packet PACKET_destroy(Packet *p)
{
    // Funcio per destruir la memoria dinamica de un paquet

    p->type = 0;
    p->length = 0;

    if (p->header != NULL)
    {
        free(p->header);
        p->header = NULL;
    }

    if (p->data != NULL)
    {
        free(p->data);
        p->data = NULL;
    }

    return *p;
}

Packet PACKET_create(char type, char *header, unsigned short dataLength, char *data)
{
    // Creacio de un paquet a partir dels parametres
    Packet pd;

    pd.type = type;
    pd.length = dataLength;

    if ((pd.header = (char *)malloc(sizeof(char) * strlen(header))) == NULL)
    {
        return pd;
    }
    strcpy(pd.header, header);

    if (pd.length != 0)
    {
        if ((pd.data = (char *)malloc(sizeof(char) * dataLength)) == NULL)
        {
            free(pd.header);
            return pd;
        }
        strcpy(pd.data, data);
    }
    else
    {
        pd.data = NULL;
    }

    return pd;
}

int PACKET_sendFile(Packet pd, int fd, char *data)
{

    // Funcio per enviar el fitxer, sense el strcpy que fem servir per enviar la resta de paquets
    // Donaria error amb strcpy perque mp3 es un fitxer binari

    IO_write(fd, &pd.type, 1);
    IO_write(fd, pd.header, UTILS_sizeOf(pd.header));

    int error = write(fd, &pd.length, sizeof(uint16_t));
    if (error < 0)
    {
        return -1;
    }
    if (pd.length != 0)
    {
        IO_write(fd, data, pd.length);
    }

    return 0;
}