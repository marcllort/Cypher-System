#include "../libs/packet.h"

Packet PACKET_read(int fd)
{

    Packet pd = PACKET_create("0", strlen("[TR_caca]"), "[TR_caca]", strlen("test"), "test");

    if (read(fd, &pd.type, 1) <= 0)
        return PACKET_destroy(&pd);

    char buff[128];
    int bytes;
    if(pd.type == 0x01){
    bytes= sprintf(buff, "CHAR RECIVED \n");

    }
    IO_write(1, buff, bytes);

    free(pd.header);
    pd.headerLength = 0;
    do
    {
        pd.header = (char *)realloc((void *)pd.header, ++pd.headerLength * sizeof(char));
        if (read(fd, &pd.header[pd.headerLength - 1], 1) <= 0)
            return PACKET_destroy(&pd);
    } while (pd.header[pd.headerLength - 1] != ']');
    bytes = sprintf(buff, "STRING RECIVED %s \n", pd.header);
    IO_write(1, buff, bytes);
    if (read(fd, &pd.length,sizeof(uint16_t)) <= 0){
        return PACKET_destroy(&pd);
    }

    char buff2[128];
    bytes = sprintf(buff2, "DATA LENGTH char1: %d \n", pd.length);
    IO_write(1, buff2, bytes);

    if (read(fd, pd.data, pd.length) <= 0){
        
        return PACKET_destroy(&pd);
    }
    bytes = sprintf(buff2, "DATA: %s \n", pd.data);
    IO_write(1, buff2, bytes);

    return pd;
}
int PACKET_write(Packet pd, int fd)
{

    
    IO_write(fd, &pd.type, 1);

    
    IO_write(fd, pd.header, strlen(pd.header));
    
    
    write(fd, &pd.length, sizeof(uint16_t));
   
    IO_write(fd, pd.data, strlen(pd.data));
    
    return 0;
}

Packet PACKET_destroy(Packet *p)
{

    p->type = 0;
    p->length = 0;
    p->headerLength = 0;

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

Packet PACKET_create(char type, int headerLength, char *header, unsigned short dataLength, char *data)
{

    Packet pd;

    pd.type = type;
    pd.headerLength = headerLength;
    pd.length = dataLength;

    if ((pd.header = (char *)malloc(sizeof(char) * headerLength)) == NULL)
        return pd;

    strcpy(pd.header, header);

    if ((pd.data = (char *)malloc(sizeof(char) * dataLength)) == NULL)
    {
        free(pd.header);
        return pd;
    }

    strcpy(pd.data, data);
    
    return pd;
}