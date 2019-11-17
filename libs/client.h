#ifndef CLIENT_H
#define CLIENT_H

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


#define MSG_ERR_CONN "Puerto %d Cerrado\n"
#define MSG_AVAIL_CONN "%d connections available\n"
#define TESTING "Testing...\n"
#define MSG_ERR_SOCKET "Error durante la creacion del socket.\n"
#define MSG_CONNECTED "%d connected: %s\n"
#define UNKNOWN_CONNECTION "Couldn't send message. Unknown connection %s\n"

/*typedef struct {
  int port;
  char* name;
  int socketfd;
} Server;*/

int CLIENT_checkPorts(Config config);

int CLIENT_connectPort(Config config, int connectPort);
int CLIENT_initClient();
int CLIENT_write(char *user, char *message);
int CLIENT_freeMemory();
char *CLIENT_read(int fd, char delimiter);
#endif //_CLIENT_H
