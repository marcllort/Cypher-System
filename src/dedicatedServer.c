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
    char *user,
    void *config)
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
        ds->config = config;
    }
    return ds;
}

int DSERVER_close(DServer *ds)          // Al tancar enviem el missatge de OK respecte hem rebut missatge desconnexio
{
    Packet p;

    p.type = 0x06;
    p.header = H_CONOK;
    p.length = 0;
    p.data = "";
    PACKET_write(p, ds->fd);
    //CLIENT_borraUser(ds->fd);
    ds->state = 0;
    //free(ds);
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

void *DSERVER_threadFunc(void *data)                // Funcio que corre al thread, encarregada de identificar cada paquet rebut i actuar corresponentment
{
    DServer *ds = (DServer *)data;
    Packet p;
    int fd = ds->fd;
    ds->state = 1;
    while (ds->state)
    {
        p = PACKET_read(fd);
        write(1,"CC\n",3);
        write(1,p.header, strlen(p.header));
        if (p.headerLength == -1)
        {
            ds->state = 0;
        }
        if (p.type == T_MSG)                // En cas de rebre correcatmen un missatge, responem indicant que hem rebut
        {
            char buff[128];
            int bytes = sprintf(buff, CLIENT_SAYS, ds->user);
            IO_write(1, buff, bytes);
            IO_write(1, p.data, p.length);
            bytes = sprintf(buff, "\n");
            IO_write(1, buff, bytes);
            UTILS_printName(ds->name);
            Packet pok = PACKET_create(T_MSG, (int)strlen(H_MSGOK), H_MSGOK, 1, " ");
            PACKET_write(pok, ds->fd);
        }
        if (p.type == T_EXIT)               // Tanquem el dedicated server en cas de desconnexio
        {
            DSERVER_close(ds);
        }
        if (p.type == T_SHOWAUDIOS)
                {
                    IO_write(1, "aaaa", 4);
                    if (!strcmp(p.header,H_LISTAUDIOS) )
                    {
                        IO_write(1, "aaaa", 4);
                        IO_write(1, p.data, p.length);
                    }
                    if (!strcmp(p.header,H_SHOWAUDIOS) )
                    {
                        char* a = DSERVER_showFiles((Config*)ds->config);
                        IO_write(1, a, strlen(a));
                        Packet pack = PACKET_create(T_SHOWAUDIOS, (int)strlen(H_LISTAUDIOS), H_LISTAUDIOS, 1, " ");
                        PACKET_write(pack, fd);
                    }
                    
                    
                }
        PACKET_destroy(&p);
    }

    return (void *)0;
}

char* DSERVER_showFiles(Config* config){
    char *audios = CONFIG_getAudioFolder(*config);
    DIR *dir;
    struct dirent *ent;
    char buff2[128];
    char buff[128];
    int bytes2 = sprintf(buff2, "./%s \n", audios);
    IO_write(1, buff2, bytes2);
    bytes2 = 0;
    if ((dir = opendir ("./Audios1")) != NULL) {
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
        
        int bytes = sprintf(buff, "%s \n %s",buff, ent->d_name);
        bytes2 =+bytes;
    }
    
    buff[bytes2++] = ']';
    buff[bytes2++] = '\0';
    closedir (dir);
    audios = &buff[0];
    return audios;
    } else {
    /* could not open directory */
    return EXIT_FAILURE;
    }
}
