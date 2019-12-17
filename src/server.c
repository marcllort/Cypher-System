#include "../libs/server.h"

Server SERVER_init(char *ip, int port, char *name, char *audios)
{ // Inicialitzacio de les vars principals del tipus server

    Server server;
    server.name = name;
    server.audios = audios;
    server.ip = ip;
    server.port = port;
    server.fd = -1;
    server.state = -1;
    server.thread = (pthread_t)0;
    server.dss = LLISTADS_crea();

    return server;
}

pthread_t *SERVER_getThread(Server *server)
{
    // Getter thread
    return &(server->thread);
}

void SERVER_setFunc(Server *server, void *(*threadFunc)(void *))
{
    // Setter funcio thread
    server->threadFunc = threadFunc;
}

int SERVER_start(Server *server)
{
    // Inicializacio server principal, encarregat rebre peticions de connexio

    if ((server->fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        IO_write(1, ERR_SOCKET, strlen(ERR_SOCKET));
        return server->state = -1;
    }

    struct sockaddr_in s_addr;
    memset(&s_addr, 0, sizeof(s_addr));

    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons((uint16_t)server->port);
    s_addr.sin_addr.s_addr = inet_addr(server->ip);

    if (inet_aton(server->ip, &s_addr.sin_addr) == 0)
    {
        struct hostent *host = gethostbyname(server->ip);

        if (host == NULL)
        {
            IO_write(1, ERR_IP, strlen(ERR_IP));
            return server->state = -2;
        }

        memcpy(&s_addr.sin_addr.s_addr, host->h_addr, (size_t)host->h_length);
    }

    // En cas de error al bind del socket o listen, mostrem error
    if (bind(server->fd, (void *)&s_addr, sizeof(s_addr)) < 0)
    {
        IO_write(1, ERR_BIND, strlen(ERR_BIND));
        return server->state = -3;
    }
    if (listen(server->fd, MAX_CONN) < 0)
    {
        IO_write(1, ERR_LISTEN, strlen(ERR_LISTEN));
        return server->state = -4;
    }

    return server->state = 0;
}

int SERVER_startDS(Server *server, int fd, int fdserver, struct sockaddr_in addr, char *user)
{
    // Inicialitzacio de server dedicat

    DServer *ds = DSERVER_init(server->ids++, fd, fdserver, 0, 0, addr, server, server->name, SERVER_removeDS, user, server->audios);

    // Afegim el ds a la llista de servers dedicats
    SERVER_addDS(server, ds);

    // Creem el thread on ha de correr el dedicated server
    if (pthread_create(DSERVER_getThread(ds), NULL, DSERVER_threadFunc, ds) != 0)
    {
        // Detach perquè s'alliberi la memoria al tancar el thread
        pthread_detach(*DSERVER_getThread(ds));
    }

    return 0;
}

int SERVER_operate(Server *server)
{
    // Funcio que va llegint paquets i segons el tipus crea un nou dedicated server o respon a la peticio
    server->state = 1;

    // Ens quedem al bucle metre no canvii el estat del server principal
    while (server->state)
    {
        struct sockaddr_in s_addr;
        socklen_t len = sizeof(s_addr);

        // Esperem una connexio
        if ((server->fdserver = accept(server->fd, (struct sockaddr *)&s_addr, &len)) <= 0)
        {
            IO_write(1, ERR_ACCEPT, strlen(ERR_ACCEPT));
        }

        // Un cop tenim una connexio, llegim el paquet que ens envia
        if (server->state == 1)
        {
            SERVER_startDS(server, server->fdserver, server->fd, s_addr, NULL);
        }
    }

    return 1;
}

int SERVER_removeDS(void *data)
{
    // Funcio per borrar el dedicated server passat per parametres

    DServer *ds = (DServer *)data;
    Server *server = (Server *)ds->server;

    pthread_mutex_lock(&server->mutex);

    close(DSERVER_getFd(ds));
    IO_write(1, server->name, sizeof(server->name));

    DSERVER_close(ds);

    pthread_mutex_unlock(&server->mutex);

    return 0;
}

int SERVER_addDS(void *server, DServer *ds)
{
    // Funcio per afegir server dedicat a la llista

    Server *s = (Server *)server;
    int i = LLISTADS_inserirDavant(&s->dss, ds);

    ds->state = 1;

    return i;
}

int SERVER_removeAllDS(Server *server)
{
    // Funcio per borrar tots els dedicated servers

    LLISTADS_vesInici(&server->dss);

    while (!LLISTADS_final(server->dss))
    {
        DServer *ds = LLISTADS_consulta(server->dss);

        close(DSERVER_getFd(ds));
        DSERVER_close(ds);
        pthread_cancel(*DSERVER_getThread(ds));
        pthread_join(*DSERVER_getThread(ds), NULL);

        free(ds);
        LLISTADS_avanca(&server->dss);
    }

    LLISTADS_destrueix(&server->dss);

    return 0;
}

void SERVER_close(Server *server)
{
    // Funcio per tancar server principal

    SERVER_removeAllDS(server);
    server->state = -1;
    close(server->fdserver);
    close(server->fd);
    LLISTADS_destrueix(&server->dss);
    IO_write(1, GOODBYE, strlen(GOODBYE));
}

void *SERVER_threadFunc(void *data)
{
    // Funcio executada per el thread del server principal

    Server *server = (Server *)data;
    if (SERVER_start(server) == 0)
        SERVER_operate(server);
    pthread_exit(0);

    return (void *)0;
}