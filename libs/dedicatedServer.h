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
#include<pthread.h>


#include "io.h"
#include "utils.h"
#include "packet.h"
#include "client.h"
#include "config.h"
#include "listds.h"

#define CLIENT_SAYS "\n[%s]: "
#define USER_CONN "\n[%s]: Connected\n"
#define USER_DISCON "\n[%s]: Disconnected\n"
#define EMPTY_FOLDER "Empty folder!\n"
#define SENDING_FILE "\n[Thread]: Sending audio file...\n"
#define SENT_FILE "\n[Thread]: Audio file sent!\n"




typedef struct
{
    int fd;
    int fdserver;
    int state;
    char *name;
    pthread_t thread;
    struct sockaddr_in addr;
    void *server;
    void *list_node;
    char *user;
    char *audios;
    Llistads llistaServers;
    pthread_mutex_t mutex;
} DServer;

DServer *DSERVER_init(
    int fd,
    int fdserver,
    int state,
    pthread_t thread,
    struct sockaddr_in addr,
    void *server,
    char *name,
    char *user,
    char *audios,
    Llistads llistaServers,
    pthread_mutex_t mutex);

int DSERVER_close(DServer *ds,int removeall);
pthread_t *DSERVER_getThread(DServer *ds);
int DSERVER_getFd(DServer *ds);
void *DSERVER_threadFunc(void *data);
char *DSERVER_showFiles(char *audios);

#endif //DSERVER_H