#ifndef PACKET_H
#define PACKET_H

#define HEADER_LENGTH 2

typedef struct {
  char type;
  char* header;
  int lenght;
  char* data;
} Packet;


int PACKET_write(char* user, char* message);
char *PACKET_read(int fd, char delimiter);



#endif 