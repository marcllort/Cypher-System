#ifndef _SERVER_H
#define _SERVER_H

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>

#include "io.h"
#include "list.h"
#include "listds.h"
#include "dedicatedServer.h"
#include "packet.h"


#define ERR_SOCKET "[Server] Error creating socket\n"
#define ERR_IP "[Server] Error resolving IP\n"
#define ERR_BIND "[Server] Error binding socket\n"
#define ERR_LISTEN "[Server] Error listening socket\n"
#define ERR_ACCEPT "[Server] Error accepting client\n"

#define ERR_SERVER "Error, no se pudo inicar el servidor\n"
#define WAITING "[Server] Waiting client...\n"
#define USERCONNECTED "[Server] Connected to user : %s\n"
#define GOODBYE "[Server] Disconnecting Server\n"
#define BYE "[Server] Bye Client\n"

//#define PORT 60001
//#define IP "127.0.0.1"
#define MAX_CONN 10

typedef struct
{
    char *name;
    char *ip;
    int port;
    int fd;
    int state;
    pthread_t thread;
    void *(*threadFunc)(void *);
    Llistads dss;
    int ids;
    pthread_mutex_t mutex;

} Server;

Server SERVER_init(char *ip, int port,char *name);
int SERVER_start(Server *server);
int SERVER_operate(Server *server);
void SERVER_close(Server *server);

pthread_t *SERVER_getThread(Server *server);

void SERVER_setMT(Server *server,
                  void *(*threadFunc)(void *));
int SERVER_startDS(Server *server, int fd, struct sockaddr_in addr, char* user);
int SERVER_addDS(void *server, DServer *ds);
int SERVER_removeDS(void *data);
int SERVER_removeDSS(Server *server);

void *SERVER_threadFunc(void *ds);

#endif