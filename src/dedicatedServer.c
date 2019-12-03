#include "../libs/dedicatedServer.h"

DServer *DSERVER_init(
    int id,
    int fd,
    int state,
    pthread_t thread,
    struct sockaddr_in addr,
    void *server,
    char *name,
    int (*remove)(void *),
    char *user)
{
    // Inicialitzacio de la variable DServer

    DServer *ds = (DServer *)malloc(sizeof(DServer));

    if (ds != NULL)
    {
        ds->id = id;
        ds->fd = fd;
        ds->state = state;
        ds->name = name;
        ds->thread = (pthread_t)thread;
        ds->addr = addr;
        ds->server = server;
        ds->list_node = NULL;
        ds->remove = remove;
        ds->user = user;
    }
    return ds;
}

// Getters

pthread_t *DSERVER_getThread(DServer *ds)
{
    return &ds->thread;
}

int DSERVER_getFd(DServer *ds)
{
    return ds->fd;
}

int DSERVER_close(DServer *ds) 
{
    // Al tancar enviem el missatge de OK respecte hem rebut missatge desconnexio
    Packet p = PACKET_create(T_EXIT, (int)strlen(H_CONOK), H_CONOK, 0, "");
    PACKET_write(p, ds->fd);
    PACKET_destroy(&p);
    ds->state = 0;

    return 0;
}

void *DSERVER_threadFunc(void *data) 
{
    // Funcio que corre al thread, encarregada de identificar cada paquet rebut i actuar corresponentment

    DServer *ds = (DServer *)data;
    Packet p;
    int fd = ds->fd;
    ds->state = 1;

    // Ens quedem al bucle metre no canvii el estat del server dedicat
    while (ds->state)
    {
        p = PACKET_read(fd);
        if (p.headerLength == -1)
        {
            ds->state = 0;
        }
        if (p.type == T_MSG) 
        {
            // En cas de rebre correcatmen un missatge, responem indicant que hem rebut

            // Printem el nom del usuari que ens envia missatge
            char buff[128];
            int bytes = sprintf(buff, CLIENT_SAYS, ds->user);
            IO_write(1, buff, bytes);

            // Printem missatge rebut
            IO_write(1, p.data, p.length);

            // Printem un \n
            bytes = sprintf(buff, "\n");
            IO_write(1, buff, bytes);

            // Printem el nom de la consola
            UTILS_printName(ds->name);

            // Enviem al usuari que ens ha contactat el OK respecte hem rebut el missatge
            Packet pok = PACKET_create(T_MSG, (int)strlen(H_MSGOK), H_MSGOK, 0, "");
            PACKET_write(pok, ds->fd);

            // Alliberem memoria
            PACKET_destroy(&pok);
        }
        if (p.type == T_EXIT) 
        {
            // Tanquem el dedicated server en cas de desconnexio
            DSERVER_close(ds);
        }
        PACKET_destroy(&p);
    }

    return (void *)0;
}
