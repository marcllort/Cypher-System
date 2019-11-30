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

int DSERVER_close(DServer *ds)
{
    Packet p;

    p.type = 0x06;
    p.header = H_CONOK;
    p.length = 0;
    p.data = "";

    PACKET_write(p, ds->fd);
    ds->state = 0;
    return 0;
}

pthread_t *DSERVER_getThread(DServer *ds)
{
    return &ds->thread;
}

int DSERVER_getFd(DServer *ds)
{
    return ds->fd;
}

int DSERVER_getState(DServer *ds)
{
    return ds->state;
}

int DSERVER_setState(DServer *ds, int state)
{
    return ds->state = state;
}

char *DSERVER_setName(DServer *ds, char *name, size_t size)
{

    if ((ds->name = (char *)malloc(sizeof(char) * (size + 1))) != NULL)
    {
        memcpy(ds->name, name, size);
        ds->name[size] = '\0';
    }
    return ds->name;
}

void *DSERVER_getListNode(DServer *ds)
{
    return ds->list_node;
}

void *DSERVER_setListNode(DServer *ds, void *list_node)
{
    return ds->list_node = list_node;
}

void *DSERVER_threadFunc(void *data)
{
    DServer *ds = (DServer *)data;
    Packet p;
    int fd = ds->fd;
    ds->state = 1;
    while (ds->state)
    {
        p = PACKET_read(fd);
        if (p.headerLength == -1)
        {
            ds->state = 0;
        }
        if (p.type == T_MSG)
        {
            char buff[128];
            int bytes = sprintf(buff, CLIENT_SAYS, ds->user);
            IO_write(1, buff, bytes);
            IO_write(1, p.data, p.length);
            bytes = sprintf(buff, "\n");
            IO_write(1, buff, bytes);
            UTILS_printName(ds->name);
            Packet pok = PACKET_create(T_MSG, (int)strlen(H_MSGOK), H_MSGOK, 0, "");
            PACKET_write(pok, ds->fd);
        }
        if (p.type == T_EXIT)
        {
            DSERVER_close(ds);
        }
        PACKET_destroy(&p);
    }

    return (void *)0;
}
