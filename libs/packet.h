#ifndef PACKET_H
#define PACKET_H

typedef struct {
  char type;
  char* header;
  int lenght;
  char* data;
} Packet;



#endif 