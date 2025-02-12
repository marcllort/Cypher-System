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
#include <pthread.h>

#include "io.h"
#include "list.h"
#include "listds.h"
#include "dedicatedServer.h"
#include "packet.h"

#define ERR_SOCKET "Error creating socket\n"
#define ERR_IP "Error IP\n"
#define ERR_BIND "Error binding socket\n"
#define ERR_LISTEN "Error listening socket\n"
#define ERR_ACCEPT "Error accepting client\n"
#define GOODBYE "\nDisconnecting Trinity...\n"
#define ERR_MUTEX "Error creating mutex\n"
#define MAX_CONN 10

typedef struct
{
    char *name;
    char *ip;
    char *audios;
    int port;
    int fd;
    int fdserver;
    int state;
    pthread_t thread;
    void *(*threadFunc)(void *);
    Llistads dss;
    pthread_mutex_t mutex;
} Server;

Server SERVER_init(char *ip, int port, char *name, char *audios);
int SERVER_start(Server *server);
int SERVER_operate(Server *server);
void SERVER_close(Server *server);
pthread_t *SERVER_getThread(Server *server);
void SERVER_setFunc(Server *server, void *(*threadFunc)(void *));
int SERVER_startDS(Server *server, int fd, int fdserver, struct sockaddr_in addr, char *user);
int SERVER_removeAllDS(Server *server);
void *SERVER_threadFunc(void *ds);

#endif