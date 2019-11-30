#ifndef DSERVER_H
#define DSERVER_H

#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <signal.h>

#include "io.h"
#include "utils.h"
#include "packet.h"

#define KEYPHRASE "Bye"
#define CLIENT_SAYS "\n[%s]: "
#define CONNECTED "Client Connected\n"
#define BYE "ByeBye Client\n"

typedef struct
{
    int id;
    int fd;
    int state;
    char *name;
    pthread_t thread;
    struct sockaddr_in addr;
    void *server;
    void *list_node;
    void *(*operate)(void *);
    int (*remove)(void *);
    char *user;
} DServer;

DServer *DSERVER_init(
    int id,
    int fd,
    int state,
    pthread_t thread,
    struct sockaddr_in addr,
    void *server,
    char *name,
    int (*remove)(void *),
    char *user);

int DSERVER_close(DServer *ds);
pthread_t *DSERVER_getThread(DServer *ds);
int DSERVER_getFd(DServer *ds);
void *DSERVER_threadFunc(void *data);

#endif //DSERVER_H