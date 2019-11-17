#include "../libs/server.h"


Server SERVER_init(char *ip, int port) {

    Server server;

    server.ip = ip;
    server.port = port;
    server.fd = -1;
    server.state = -1;
    server.thread = (pthread_t) 0;
    server.threadFunc = NULL;
    server.threadISR = NULL;
    server.threadSig = 0;
    server.dss = LLISTADS_crea();
    server.dsThreadOperate = NULL;
    server.dsThreadISR = NULL;
    server.dsThreadSig = 0;
    server.ids = 0;
    server.mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;

    return server;
}


pthread_t* SERVER_getThread(Server *server) {
    return &(server->thread);
}

int SERVER_getThreadSig(Server *server) {
    return server->threadSig;
}

void SERVER_setMT ( Server  *server,
                    void*   (*threadFunc)   (void*),
                    void    (*threadISR)    (int),
                    unsigned short threadSig,
                    void*   (*dsThreadOperate)  (void*),
                    void    (*dsThreadISR)      (int),
                    unsigned short dsThreadSig)
{
    server->threadFunc = threadFunc;
    server->threadISR = threadISR;
    server->threadSig = threadSig;
    signal(threadSig, threadISR);

    server->dsThreadOperate = dsThreadOperate;
    server->dsThreadISR = dsThreadISR;
    server->dsThreadSig = dsThreadSig;
    signal(dsThreadSig, dsThreadISR);
}

int SERVER_start(Server *server) {

    if ((server->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        write(1, ERR_SOCKET, strlen(ERR_SOCKET));
        return server->state = -1;
    }

    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons((uint16_t) server->port);
    s_addr.sin_addr.s_addr = INADDR_ANY;

    if (inet_aton(server->ip, &s_addr.sin_addr) == 0) {

        struct hostent* host = gethostbyname(server->ip);

        if (host == NULL) {
            write(1, ERR_IP, strlen(ERR_IP));
            return server->state = -2;
        }

        memcpy(&s_addr.sin_addr.s_addr, host->h_addr, (size_t) host->h_length);
    }

    if (bind(server->fd, (struct sockaddr*) &s_addr, sizeof(s_addr)) < 0) {
        write(1, ERR_BIND, strlen(ERR_BIND));
        return server->state = -3;
    }

    if (listen(server->fd, MAX_CONN) < 0) {
        write(1, ERR_LISTEN, strlen(ERR_LISTEN));
        return server->state = -4;
    }

    return server->state = 0;
}

int SERVER_startDS(Server* server, int fd, struct sockaddr_in addr) {

    DServer *ds = DSERVER_init(server->ids++, fd, 0, 0, addr, server, NULL, server->dsThreadOperate, SERVER_removeDS);

    SERVER_addDS(server, ds);

    if (pthread_create(DSERVER_getThread(ds), NULL, DSERVER_threadFunc, ds) != 0) {
        return EXIT_FAILURE;
    }

    pthread_detach(*DSERVER_getThread(ds));

    return 0;
}

int SERVER_operate(Server *server) {

    server->state = 1;

    while (server->state) {

        int fd;
        struct sockaddr_in s_addr;
        socklen_t len = sizeof(s_addr);

        do {
            write(1, WAITING, strlen(WAITING));

            if ((fd = accept(server->fd, (struct sockaddr*) &s_addr, &len)) <= 0) {
                write(1, ERR_ACCEPT, strlen(ERR_ACCEPT));
            }
        } while (fd <= 0);

        SERVER_startDS(server, fd, s_addr);
    }

    return EXIT_SUCCESS;
}

int removeDS(DServer* ds)
{
    close(DSERVER_getFd(ds));
    LLISTADS_eliminaAmbNode(&((Server*)ds->server)->dss, DSERVER_getListNode(ds));
    printf("List size: %d\n", (int) LLISTADS_getMida(((Server*)ds->server)->dss));
    return 0;
}

int SERVER_removeDS(void *data) {

    DServer* ds = (DServer*) data;
    Server* server = (Server*)ds->server;

    pthread_mutex_lock(&server->mutex);

    removeDS(ds);
    DSERVER_close(ds);

    pthread_mutex_unlock(&server->mutex);

    return 0;
}

int SERVER_addDS(void *server, DServer *ds) {

    Server *s = (Server*) server;

    Nodeds * node = LLISTADS_inserirDavant(&s->dss, ds);  //NOSE PERQUE HO VOL AFEGIR 2 ABANS

    DSERVER_setListNode(ds, node);

    ds->state = 1;

    printf("List Size: %d\n", (int) LLISTADS_getMida(s->dss));

    return 0;
}

int SERVER_removeDSS(Server *server) {

    LLISTADS_vesInici(&server->dss);

    while (!LLISTADS_final(server->dss)) {

        DServer* ds = (DServer)LLISTADS_consulta(server->dss);

        removeDS(ds);

        pthread_kill(*DSERVER_getThread(ds), server->dsThreadSig);
        pthread_join(*DSERVER_getThread(ds), NULL);

        DSERVER_close(ds);

        LLISTADS_avanca(&server->dss);
    }

    LLISTADS_destrueix(&server->dss);

    return 0;
}

void SERVER_close(Server *server) {

    SERVER_removeDSS(server);

    server->state = -1;
    close(server->fd);

    write(1, GOODBYE, strlen(GOODBYE));
}

void* SERVER_threadFunc(void* data) {

    Server *server = (Server*) data;
    sigset_t set;
    sigemptyset(&set);

    if (sigaddset(&set, server->threadSig)) pthread_exit(0);

    if (SERVER_start(server) == 0) SERVER_operate(server);

    pthread_exit(0);
}