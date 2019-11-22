#include "../libs/dedicatedServer.h"

DServer* DSERVER_init (
                 int                 id,
                 int                 fd,
                 int                 state,
                 pthread_t           thread,
                 struct sockaddr_in  addr,
                 void*               server,
                 void*               list_node,
                 void*               (*operate)  (void*),
                 int                 (*remove)   (void*))
{
    DServer* ds = (DServer*) malloc(sizeof(DServer));

    if (ds != NULL) {
        ds->id = id;
        ds->fd = fd;
        ds->state = state;
        ds->name = NULL;
        ds->thread = thread;
        ds->addr = addr;
        ds->server = server;
        ds->list_node = list_node;
        ds->operate = operate;
        ds->remove = remove;
        //ds->file = FILE_init(); NO ens cal cap file en principi
    }
    return ds;
}

int DSERVER_close(DServer* ds) {
    if (ds->name != NULL) free(ds->name);
    free(ds);
    return 0;
}

pthread_t* DSERVER_getThread(DServer *ds) {
    return &ds->thread;
}

int DSERVER_getFd(DServer* ds) {
    return ds->fd;
}

int DSERVER_getState(DServer* ds) {
    return ds->state;
}

int DSERVER_setState(DServer *ds, int state) {
    return ds->state = state;
}

char* DSERVER_setName(DServer *ds, char* name, size_t size) {

    if ((ds->name = (char*) malloc(sizeof(char) * (size + 1))) != NULL) {
        memcpy(ds->name, name, size);
        ds->name[size] = '\0';
    }
    return ds->name;
}

void* DSERVER_getListNode(DServer* ds) {
    return ds->list_node;
}

void* DSERVER_setListNode(DServer* ds, void* list_node) {
    return ds->list_node = list_node;
}

void* DSERVER_threadFunc(void* data) {

    sigset_t set;
    sigemptyset(&set);

    if (sigaddset(&set, SIGUSR2)) pthread_exit(0);

    write(1, CONNECTED, strlen(CONNECTED));

    DServer* ds = (DServer*) data;

    ds->state = 1;
    ds->operate(ds);
    ds->remove(ds);

    pthread_exit(0);
}
