#include "../libs/packet.h"


char *PACKET_read(int fd, char delimiter)
{
    char *msg = (char *)malloc(1);
    char current;
    int i = 0;

    while (read(fd, &current, 1) > 0)
    {

        msg[i] = current;
        msg = (char *)realloc(msg, ++i + 1);

        if (current == delimiter)
            break;
    }

    msg[i] = '\0';

    return msg;
}
int PACKET_write(Packet pd, int fd) {

    int size = 1 + HEADER_LENGTH + 2 + pd.length;
    unsigned char *data = NULL;

    if ((data = (unsigned char*) malloc (sizeof(unsigned char) * size)) == NULL) return -1;

    *(data)++ = pd.type;                       //Assign and increment pointer

    memcpy(data, pd.header, HEADER_LENGTH);  //Assign
    data += HEADER_LENGTH;                   //Increment Pointer

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