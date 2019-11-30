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
    char* user)
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
        ds->user=user;
    }
    return ds;
}

int DSERVER_close(DServer *ds)
{
    Packet p = PACKET_create(T_EXIT, (int)strlen(H_VOID), H_VOID, (int)strlen(ds->name), ds->name);
    PACKET_write(p,ds->fd);
    p = PACKET_read(ds->fd);
    char buff[124];
    sprintf(buff,"Connexio adeu: %s",p.header);
    IO_write(1, buff, strlen(buff));
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
    ds->state = 1;
    while (ds->state)
    {
        p = PACKET_read(fd);
        if(p.headerLength == -1){
            ds->state = 0;
        }
        if (p.type == T_MSG)
        {
            char buff[128];
            int bytes = sprintf(buff, CLIENT_SAYS, ds->user, p.data);
            IO_write(1, buff, bytes);
            UTILS_printName(ds->name);
        }
        if (p.type == T_EXIT)
        {
            //POSAR ALGO AQUI PER ELIMINAR DS DE LISTDS no puc fer la funcio perk necesito el server.h i no el puc incloure
            //LLISTADS_eliminaAmbNode((&(ds->server)->dss),ds);

            Packet p = PACKET_create(T_EXIT, (int)strlen(H_CONOK), H_CONOK, 0, "");
            PACKET_write(p,ds->fd);
            
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
    
    DSERVER_close(ds);
    pthread_exit(0);
    return 1;
}
