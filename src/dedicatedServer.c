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
        ds->audios[strlen(ds->audios) - 1] = 0;
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
            Packet pok = PACKET_create(T_MSG, (int)strlen(H_MSGOK), H_MSGOK, 1, " ");
            PACKET_write(pok, ds->fd);

            // Alliberem memoria
            PACKET_destroy(&pok);
        }
        else if (p.type == T_EXIT)
        {
            // Tanquem el dedicated server en cas de desconnexio
            DSERVER_close(ds);
        }
        else if (p.type == T_SHOWAUDIOS)
        {

            if (!strcmp(p.header, H_LISTAUDIOS))
            {
                IO_write(1, "aaaa", 4);
                IO_write(1, p.data, p.length);
            }
            if (!strcmp(p.header, H_SHOWAUDIOS))
            {
                char *a = DSERVER_showFiles(ds->audios);
                IO_write(1, a, strlen(a));
                Packet pack = PACKET_create(T_SHOWAUDIOS, (int)strlen(H_LISTAUDIOS), H_LISTAUDIOS, UTILS_sizeOf(a), a);

                char buff[126];
                int bytes= sprintf(buff,"\nVALUE DATA: %d\n", UTILS_sizeOf(a));
                IO_write(1, buff, UTILS_sizeOf(buff));
                
                PACKET_write(pack, fd);
                // Alliberem memoria
                free(a);
                PACKET_destroy(&pack);
            }
        }
        else if (p.type == T_DOWNLOAD)
        {
            IO_write(1, "Dins DOWNLOAD\n", strlen("Dins DOWNLOAD\n"));
            if (!strcmp(p.header, H_AUDREQ))
            {
                IO_write(1, "Dins H_AUDREQ\n", strlen("Dins H_AUDREQ\n"));
                int sizes = UTILS_sizeOf(ds->audios)+p.length +3;

                char buff[126];
                int bytes= sprintf(buff,"\nVALUE DATA: %d\n", sizes);
                IO_write(1, buff, UTILS_sizeOf(buff));

                char *audioFolderr = (char *)malloc(sizes);
                //audioFolder = (char *)realloc((void *)audioFolder, );
                
                int gg = sprintf(audioFolderr, "./%s/%s", ds->audios,p.data);
                /*strtok(audioFolder, "\n");
                audioFolder[strlen(audioFolder) - 1] = 0;
                strcat(audioFolder, "/");
                strcat(audioFolder, p.data);*/

                IO_write(1, "\n", 1);
                IO_write(1, audioFolderr, sizes);
                IO_write(1, "\n", 1);

                char buffer[124]="";
                strcpy(buffer, audioFolderr);
                buffer[sizes]=0;
                IO_write(1, "\n", 1);
                IO_write(1, buffer, sizes);
                IO_write(1, "\n", 1);

                if (UTILS_fileExists(buffer) != -1)
                {
                    IO_write(1, "Existe\n", strlen("Existe\n"));
                    Packet pack;

                    pack = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDRESP), H_AUDRESP, 0, NULL);
                    PACKET_write(pack, fd);
                    //PACKET_destroy(&pack);
                    /*do{
                        pack = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDRESP), H_AUDRESP, 0, NULL);
                        PACKET_write(pack, fd);
                    }while(IO_checkEOF(fdfdfd)!=1);
                    PACKET_destroy(&pack);*/
                }
                else
                {
                    IO_write(1, "No existe\n", strlen("No Existe\n"));
                    Packet pack = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDKO), H_AUDKO, 0, NULL);
                    PACKET_write(pack, fd);
                    //PACKET_destroy(&pack);
                }
                free(audioFolderr);
            }
        }
        //PACKET_destroy(&p);
        //free(p.data);
        //free(p.header);
    }

    return (void *)0;
}

char *DSERVER_showFiles(char *audios)
{

    DIR *dir;
    struct dirent *ent;
    char *audiosData = (char *)malloc(sizeof(char));
    char *audioFolder = (char *)malloc(sizeof(char));

    audioFolder = (char *)realloc((void *)audioFolder, strlen(audios));
    sprintf(audioFolder, "./%s", audios);
    //strtok(audioFolder, "\n");
    //audioFolder[strlen(audioFolder) - 1] = 0;
    IO_write(1, audioFolder, strlen(audioFolder));
    IO_write(1, "\n", 1);
    if ((dir = opendir(audioFolder)) != NULL)
    {
        int i = 0;
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type != DT_DIR)
            {
                if (i == 0)
                {
                    IO_write(1, audiosData, strlen(audiosData));
                    audiosData = (char *)realloc((void *)audiosData, UTILS_sizeOf(ent->d_name) + 1);
                    strcpy(audiosData,ent->d_name);
                    IO_write(1, i, 1);
                    i++;
                }
                else
                {
                    audiosData = (char *)realloc((void *)audiosData, UTILS_sizeOf(audiosData) + UTILS_sizeOf(ent->d_name) + 1);
                    sprintf(audiosData, "%s\n%s", audiosData, ent->d_name);
                    IO_write(1, i, 1);
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
