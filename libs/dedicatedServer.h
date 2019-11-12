#ifndef DSERVER_H
#define DSERVER_H

#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <signal.h>


#define KEYPHRASE   "Bye"
#define CLIENT_SAYS "[Server] Client says: \n"
#define CONNECTED   "[Server] Client Connected\n"
#define BYE         "[Server] Bye Client\n"

typedef struct
{
    int                 id;
    int                 fd;
    int                 state;
    char*               name;
    pthread_t           thread;
    struct sockaddr_in  addr;
    void*               server;
    void*               list_node;
    void*               (*operate)  (void*);
    int                 (*remove)   (void*);
    File                file;
} DServer;

DServer* DSERVER_init
(
    int                 id,
    int                 fd,
    int                 state,
    pthread_t           thread,
    struct sockaddr_in  addr,
    void*               server,
    void*               list_node,
    void*               (*operate) (void*),
    int                 (*remove)  (void*)
);

int DSERVER_close(DServer* ds);

int DSERVER_getFd(DServer* ds);
int DSERVER_getState(DServer* ds);
int DSERVER_setState(DServer *ds, int state);
char* DSERVER_setName(DServer *ds, char* name, size_t size);
pthread_t* DSERVER_getThread(DServer *ds);
void* DSERVER_getListNode(DServer* ds);
void* DSERVER_setListNode(DServer* ds, void* list_node);

void* DSERVER_threadFunc(void* data);

#endif //DSERVER_H