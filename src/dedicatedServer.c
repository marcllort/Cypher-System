#include "../libs/dedicatedServer.h"

DServer *DSERVER_init(
    int id,
    int fd,
    int fdserver,
    int state,
    pthread_t thread,
    struct sockaddr_in addr,
    void *server,
    char *name,
    int (*remove)(void *),
    char *user,
    char *audios)
{
    // Inicialitzacio de la variable DServer

    DServer *ds = (DServer *)malloc(sizeof(DServer));

    if (ds != NULL)
    {
        ds->id = id;
        ds->fd = fd;
        ds->fdserver = fdserver;
        ds->state = state;
        ds->name = name;
        ds->thread = (pthread_t)thread;
        ds->addr = addr;
        ds->server = server;
        ds->list_node = NULL;
        ds->remove = remove;
        ds->user = user;
        ds->audios = audios;
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
    Packet p = PACKET_create(T_EXIT, (int)strlen(H_CONOK), H_CONOK, 0, NULL);
    PACKET_write(p, ds->fd);
    PACKET_destroy(&p);
    ds->state = -1;
    IO_write(1,"state 1", sizeof("state 1"));
    return 0;
}

void *DSERVER_threadFunc(void *data)
{
    // Funcio que corre al thread, encarregada de identificar cada paquet rebut i actuar corresponentment
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    DServer *ds = (DServer *)data;
    Packet p;
    int fd = ds->fd;
    ds->state = 1;

    // Ens quedem al bucle metre no canvii el estat del server dedicat
    while (ds->state==1)
    {
        
        p = PACKET_read(fd);
        if (p.headerLength == -1)
        {
            ds->state = -1;
            IO_write(1,"state 1", sizeof("state 1"));
            //PACKET_destroy(&p);
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
            Packet pok = PACKET_create(T_MSG, (int)strlen(H_MSGOK), H_MSGOK, 1, " ");
            PACKET_write(pok, ds->fd);

            // Alliberem memoria
            PACKET_destroy(&pok);
            //PACKET_destroy(&p);
        }
        else if (p.type == T_EXIT)
        {
            // Tanquem el dedicated server en cas de desconnexio
            DSERVER_close(ds);
            //PACKET_destroy(&p);
        }
        else if (p.type == T_SHOWAUDIOS)
        {
            if (!strcmp(p.header, H_LISTAUDIOS))
            {
                IO_write(1, p.data, p.length);
            }
            if (!strcmp(p.header, H_SHOWAUDIOS))
            {
                char *a = DSERVER_showFiles(ds->audios);
                Packet pack = PACKET_create(T_SHOWAUDIOS, (int)strlen(H_LISTAUDIOS), H_LISTAUDIOS, UTILS_sizeOf(a), a);

                PACKET_write(pack, fd);
                // Alliberem memoria
                
                PACKET_destroy(&pack);
                free(a);
            }
            //PACKET_destroy(&p);
        }
        else if (p.type == T_DOWNLOAD)
        {
            if (!strcmp(p.header, H_AUDREQ))
            {
                int sizes = UTILS_sizeOf(ds->audios) + p.length + 3;

                char *audioFolderr = (char *)malloc(sizes);
                sprintf(audioFolderr, "./%s/%s", ds->audios, p.data);

                /*char buffer[124] = "";
                strcpy(buffer, audioFolderr);
                buffer[sizes] = 0;*/
                audioFolderr[sizes]=0;

                if (UTILS_fileExists(audioFolderr) != -1)
                {
                    //IO_write(1, "Existe\n", strlen("Existe\n"));

                    Packet pack = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDRESP), H_AUDRESP, 0, NULL);
                    PACKET_write(pack, fd);
                    PACKET_destroy(&pack);
                    /*do{
                        pack = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDRESP), H_AUDRESP, 0, NULL);
                        PACKET_write(pack, fd);
                    }while(IO_checkEOF(fdfdfd)!=1);
                    */
                    //PACKET_destroy(&pack);
                }
                else
                {
                    //IO_write(1, "No existe\n", strlen("No Existe\n"));
                    Packet pack = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDKO), H_AUDKO, 0, NULL);
                    PACKET_write(pack, fd);
                    PACKET_destroy(&pack);
                }
                free(audioFolderr);
            }
            //free(p.header);
        }

        //free(p.header);
        /*if(p.data != NULL){
            char bufff[128];
            sprintf(bufff, "\nData: %s  Header: %s  Type: %d\n", p.data, p.header, p.type);
            IO_write(1, bufff,UTILS_sizeOf(bufff));
        }
        if(p.type!=6){
            char bufff[128];
            sprintf(bufff, "\nDELETE Data: %s  Header: %s  Type: %d\n", p.data, p.header, p.type);
            IO_write(1, bufff,UTILS_sizeOf(bufff));
            PACKET_destroy(&p);
        }*/
        //if(p.type!=6){
            PACKET_destroy(&p);
        //}
        //IO_write(1,"state 1", sizeof("state 1"));
    }
    IO_write(1,"FORAAA BUCLE, DETACHEAME", sizeof("FORAAA BUCLE, DETACHEAME"));
    pthread_exit(0);
    return (void *)0;
}

char *DSERVER_showFiles(char *audios)
{

    DIR *dir;
    struct dirent *ent;
    char *audiosData = (char *)malloc(sizeof(char));
    char *audioFolder = (char *)malloc(UTILS_sizeOf(audios));

    sprintf(audioFolder, "./%s", audios);

    if ((dir = opendir(audioFolder)) != NULL)
    {
        int i = 0;
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type != DT_DIR)
            {
                if (i == 0)
                {
                    audiosData = (char *)realloc((void *)audiosData, UTILS_sizeOf(ent->d_name) + 1);
                    strcpy(audiosData, ent->d_name);
                    i++;
                }
                else
                {
                    audiosData = (char *)realloc((void *)audiosData, UTILS_sizeOf(audiosData) + UTILS_sizeOf(ent->d_name) + 1);
                    sprintf(audiosData, "%s\n%s", audiosData, ent->d_name);
                }
            }
        }

        closedir(dir);
        free(audioFolder);

        return audiosData;
    }
    else
    {
        return "No audios found\n";
    }
}
