#include "../libs/packet.h"

Packet PACKET_read(int fd)
{
    // Funcio encarregada de la lectura de un paquet
    Packet pd;

    int error = read(fd, &pd.type, 1);
    if (pd.type != 0x01 && pd.type != 0x02 && pd.type != 0x03 && pd.type != 0x04 && pd.type != 0x05 && pd.type != 0x06)
    {
        pd.headerLength = -1;
        return pd;
    }

    if (error <= 0)
    {
        pd.headerLength = -1;
        return pd;
    }

    pd.headerLength = 0;
    pd.header = (char *)malloc(sizeof(char));
    do
    {
        pd.header = (char *)realloc((void *)pd.header, ++pd.headerLength * sizeof(char));
        if (read(fd, &pd.header[pd.headerLength - 1], 1) <= 0)
            return PACKET_destroy(&pd);
    } while (pd.header[pd.headerLength - 1] != ']');

    pd.header[pd.headerLength] = '\0';


    if (read(fd, &pd.length, sizeof(uint16_t)) <= 0)
    {
        return PACKET_destroy(&pd);
    }
    if (pd.length != 0)
    {
        pd.data = (char *)malloc(sizeof(char) * pd.length);
        if (read(fd, pd.data, pd.length) <= 0)
        {
            return PACKET_destroy(&pd);
        }
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
        IO_write(1, " ", strlen(" "));
    }
    if (pd.length != 0)
    {
        IO_write(fd, pd.data, strlen(pd.data));
    }

    return 0;
}

Packet PACKET_destroy(Packet *p)
{
    // Funcio per destruir la memoria dinamica de un paquet

    p->type = 0;
    p->length = 0;
    p->headerLength = 0;

    if (p->header != NULL)
    {
        strcpy(p->header, " ");
        free(p->header);
        p->header = NULL;
    }

    if (p->data != NULL)
    {
        strcpy(p->data, " ");
        free(p->data);
        p->data = NULL;
    }

    return *p;
}

Packet PACKET_create(char type, int headerLength, char *header, unsigned short dataLength, char *data)
{
    // Creacio de un paquet a partir dels parametres
    Packet pd;

    pd.type = type;
    pd.headerLength = headerLength;
    pd.length = dataLength;

    if ((pd.header = (char *)malloc(sizeof(char) * headerLength)) == NULL)
    {
        return pd;
    }
    strcpy(pd.header, header);

    if ((pd.data = (char *)malloc(sizeof(char) * dataLength)) == NULL)
    {
        free(pd.header);
        return pd;
    }
    if(pd.length!=0){
        strcpy(pd.data, data);
    }   

    return pd;
}