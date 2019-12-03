#ifndef DSERVER_H
#define DSERVER_H

#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <signal.h>

#include "io.h"
#include "utils.h"
#include "packet.h"
#include "client.h"
#include "config.h"


#define CLIENT_SAYS "\n[%s]: "

typedef struct
{
    int id;
    int fd;
    int fdserver;
    int state;
    char *name;
    pthread_t thread;
    struct sockaddr_in addr;
    void *server;
    void *list_node;
    void *(*operate)(void *);
    int (*remove)(void *);
    char *user;
    void* config;
} DServer;

DServer *DSERVER_init(
    int id,
    int fd,
    int fdserver,
    int state,
    pthread_t thread,
    struct sockaddr_in addr,
    void *server,
    char *name,
    int (*remove)(void *),
    char *user,
    void  *config);

int DSERVER_close(DServer *ds);
pthread_t *DSERVER_getThread(DServer *ds);
int DSERVER_getFd(DServer *ds);
void *DSERVER_threadFunc(void *data);
char* DSERVER_showFiles(Config* config);

#endif //DSERVER_H