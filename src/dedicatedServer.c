#include "../libs/dedicatedServer.h"

DServer *DSERVER_init(
    int id,
    int fd,
    int state,
    pthread_t thread,
    struct sockaddr_in addr,
    void *server,
    char *name,
    int (*remove)(void *))
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
    }
    return ds;
}

int DSERVER_close(DServer *ds)
{
    if (ds->name != NULL)
        free(ds->name);
    free(ds);
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
    //Aqui caldria posar d'alguna manera una funcio semblant a la de server operate pero sense el accept i aixo que nomes facir read
    DServer *ds = (DServer *)data;
    //IO_write(1, CONNECTED, strlen(CONNECTED));
    Packet p;
    int fd = ds->fd;
    while (1)
    {
        p = PACKET_read(fd);
        if (p.type == T_MSG)
        {
            char buff[128];
            int bytes = sprintf(buff, CLIENT_SAYS, ds->name, p.data);
            IO_write(1, buff, bytes);
            
        }
        //free(p);
        //free(&p.data);
        //free(&p.header);
        //char buff[128];
        //IO_write(1, p.data, p.length);
        //IO_write(1, "p.data", sizeof("p.data"));
        //wait(100);
        PACKET_destroy(&p);
    }
    

    pthread_exit(0);
    return 1;
}
