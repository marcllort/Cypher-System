#ifndef PACKET_H
#define PACKET_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#include "io.h"

#define T_CONNECT 0x01
#define T_MSG 0x02
#define T_BROADCAST 0x03
#define T_SHOWAUDIOS 0x04
#define T_EXIT 0x06

#define H_DEF "[]"
#define H_CONOK "[CONOK]"
#define H_CONKO "[CONKO]"
#define H_NAME "[TR_NAME]"
#define H_MSG "[MSG]"
#define H_MSGOK "[MSGOK]"
#define H_SHOWAUDIOS "[SHOW_AUDIOS]"
#define H_LISTAUDIOS "[LIST_AUDIOS]"
#define H_VOID "[]"

typedef struct
{
  char type;
  int headerLength;
  char *header;
  uint16_t length;
  char *data;
} Packet;

int PACKET_write(Packet pd, int fd);
Packet PACKET_read(int fd);
Packet PACKET_destroy(Packet *p);
Packet PACKET_create(char type, int headerLength, char *header, unsigned short dataLength, char *data);

#endif