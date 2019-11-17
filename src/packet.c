#include "../libs/packet.h"


Packet PACKET_read(int fd/*, char delimiter*/)
{
    Packet pd = PACKET_create(0, 0, NULL, 0, NULL);

    if (read(fd, &pd.type, 1) <= 0) return PACKET_destroy(&pd);

    do {
        pd.header = (char*) realloc((void*) pd.header, ++pd.headerLength * sizeof(char));
        if (read(fd, &pd.header[pd.headerLength - 1], 1) <= 0) return PACKET_destroy(&pd);
    } while(pd.header[pd.headerLength - 1] != ']');

    unsigned char tmp[2];

    if (read(fd, tmp, 2) <= 0) return PACKET_destroy(&pd);

    if ((pd.dataLength = (tmp[0] << 8) | tmp[1]) > 0) {
        pd.data = (char*) realloc((void*) pd.data, sizeof(char) * pd.dataLength);
        if (read(fd, pd.data, pd.dataLength) <= 0) return PACKET_destroy(&pd);
    } else {
        return PACKET_destroy(&pd);
    }

    return pd;
}
int PACKET_write(Packet pd, int fd) {

    int size = 1 + pd.headerLength + 2 + pd.length;
    unsigned char *data = NULL;

    if ((data = (unsigned char*) malloc (sizeof(unsigned char) * size)) == NULL) return -1;

    *(data)++ = pd.type;                       //Assign and increment pointer

    memcpy(data, pd.header, pd.headerLength);  //Assign
    data += pd.headerLength;                   //Increment Pointer

    *(data)++ = (unsigned char) (pd.length >> 8);   //Assign and increment pointer
    *(data)++ = (unsigned char) pd.length;          //Assign and increment pointer

    memcpy(data, pd.data, pd.length);      //Assign

    data -= sizeof(char) * (size - pd.length);      //Restart pointer

    write(fd, data, size);

    free(data);

    return 0;
}

Packet PACKET_destroy(Packet *p) {

    p->type = 0;
    p->length = 0;
    p->headerLength = 0;

    

    if (p->header != NULL)  {
        free(p->header);
        p->header = NULL;
    }

    if (p->data != NULL) {
        free(p->data);
        p->data = NULL;
    }

    return *p;
}

Packet PACKET_create(char type, int headerLength, char *header, unsigned short dataLength, char *data) {

    Packet pd;

    pd.type = type;
    pd.headerLength = headerLength;
    pd.length = dataLength;

    if ((pd.header = (char*)malloc(sizeof(char) * headerLength)) == NULL) return pd;

    memcpy(pd.header, header, (size_t) headerLength);

    if ((pd.data = (char*) malloc(sizeof(char) * dataLength)) == NULL) {
        free(pd.header);
        return pd;
    }

    memcpy(pd.data, data, dataLength);


    return pd;
}