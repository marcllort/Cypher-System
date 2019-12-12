#ifndef PACKET_H
#define PACKET_H

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#include "io.h"
#include "utils.h"

#define T_CONNECT 0x01
#define T_MSG 0x02
#define T_BROADCAST 0x03
#define T_SHOWAUDIOS 0x04
#define T_DOWNLOAD 0x05
#define T_EXIT 0x06

#define H_DEF "[]"
#define H_CONOK "[CONOK]"
#define H_CONKO "[CONKO]"
#define H_NAME "[TR_NAME]"
#define H_AUDREQ "[AUDIO_RQST]"
#define H_AUDKO "[AUDIO_KO]"
#define H_AUDRESP "[AUDIO_RSPNS]"
#define H_AUDEOF "[EOF]"
#define H_MD5OK "[MD5OK]"
#define H_MD5KO "[MD5KO]"
#define H_MSG "[MSG]"
#define H_MSGOK "[MSGOK]"
#define H_SHOWAUDIOS "[SHOW_AUDIOS]"
#define H_LISTAUDIOS "[LIST_AUDIOS]"
#define H_VOID "[]"

typedef struct
{
  char type;
  char *header;
  uint16_t length;
  char *data;
} Packet;

int PACKET_write(Packet pd, int fd);
Packet PACKET_read(int fd);
Packet PACKET_destroy(Packet *p);
Packet PACKET_create(char type, char *header, unsigned short dataLength, char *data);
int PACKET_sendFile(Packet pd, int fd,char* data);

#endif