#include "../libs/dedicatedServer.h"

DServer *DSERVER_init(
    int id,
    int fd,
    int state,
    pthread_t thread,
    struct sockaddr_in addr,
    void *server,
    void *list_node,
    int (*remove)(void *))
{
    DServer *ds = (DServer *)malloc(sizeof(DServer));

    if (ds != NULL)
    {
        ds->id = id;
        ds->fd = fd;
        ds->state = state;
        ds->name = NULL;
        ds->thread = (pthread_t)thread;
        ds->addr = addr;
        ds->server = server;
        ds->list_node = list_node;
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
   
    DServer *ds = (DServer *)data;
    IO_write(1, CONNECTED, strlen(CONNECTED));
    Packet p = PACKET_read(ds->fd);
        char buff[128];
        if (p.type == T_CONNECT)
        {
            if (strcmp(p.header,H_NAME))
            {
                IO_write(1, CONNECTED, strlen(CONNECTED));
                Packet s = PACKET_create(T_CONNECT, (int)strlen(H_CONOK), H_CONOK, (int)strlen("Alex"), "Alex");
                PACKET_write(s, ds->fd);
                
                
            }
            
            if (strcmp(p.header,H_CONOK))
            {
                //START CONNECTION TO HAVE FD THE OTHER WAY
                char buff[100];
                //int bytes = sprintf(buff, USERCONNECTED,p.data);
                //write(1,buff, sizeof(buff));
            }
        }

    

    ds->state = 1;
    
    

    pthread_exit(0);
}
