#ifndef SERVER_H
#define SERVER_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include "config.h"
#include "utils.h"
#include "list.h"
#include "packet.h"

typedef struct {
  int port;
  char* name;
  int socketfd;
} Server;           // Caldran més atributs, aquest es un copy paste de el de client




int SERVER_initServer();
int SERVER_closeServer();

//getters i setters de classe server


#endif //_SERVER_H