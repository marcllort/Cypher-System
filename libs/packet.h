#ifndef PACKET_H
#define PACKET_H


#define T_CONNECT       1
#define T_DISCONNECT    2
#define T_TRANSFER      3

#define H_DEF       "[]"
#define H_CONOK     "[CONOK]"
#define H_CONKO     "[CONKO]"
#define H_META      "[METADATA]"

typedef struct {
  char type;
  int headerLength;
  char* header;
  int lenght;
  char* data;
} Packet;


int PACKET_write(Packet pd, int fd);
char *PACKET_read(int fd);
Packet PACKET_destroy(Packet *p);
Packet PACKET_create(char type, int headerLength, char *header, unsigned short dataLength, char *data);


#endif 