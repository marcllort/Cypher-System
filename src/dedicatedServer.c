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
    //PACKET_destroy(&p);
    ds->state = -1;
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

    char *audioFolder = (char *)malloc(sizeof(char) * UTILS_sizeOf(ds->audios));

    sprintf(audioFolder, "./%s", ds->audios);

    // Ens quedem al bucle metre no canvii el estat del server dedicat
    while (ds->state == 1)
    {
        p = PACKET_read(fd);
        if (p.headerLength == -1)
        {
            ds->state = -1;
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
        else if (p.type == T_DOWNLOAD)
        {
            if (!strcmp(p.header, H_AUDREQ))
            {
                int sizes = UTILS_sizeOf(ds->audios) + p.length + 3;

                char *audioFolderr = (char *)malloc(sizeof(char) * sizes);
                sprintf(audioFolderr, "%s/%s", audioFolder, p.data);

                audioFolderr[sizes] = 0;
                if (UTILS_fileExists(audioFolderr) != -1)
                {
                    Packet packet;
                    /*pack.type = T_DOWNLOAD;
                    pack.header = H_AUDRESP;
                    pack.length = 0;
                    pack.data = NULL;*/
                    packet = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDRESP), H_AUDRESP, 0, NULL);
                    PACKET_write(packet, fd);
                    
                    //free(p.data);
                    PACKET_destroy(&packet);
                    /*do{
                        pack = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDRESP), H_AUDRESP, 0, NULL);
                        PACKET_write(pack, fd);
                    }while(IO_checkEOF(fdfdfd)!=1);
                    */
                }
                else
                {
                    Packet pack = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDKO), H_AUDKO, 0, NULL);
                    IO_write(1, pack.header, pack.headerLength);
                    PACKET_write(pack, fd);
                    PACKET_destroy(&pack);
                }

                free(audioFolderr);
            }
        }
        else if (p.type == T_SHOWAUDIOS)
        {
            if (!strcmp(p.header, H_LISTAUDIOS))
            {
                IO_write(1, p.data, p.length);
            }
            if (!strcmp(p.header, H_SHOWAUDIOS))
            {
                char *a = DSERVER_showFiles(audioFolder);
                Packet pack = PACKET_create(T_SHOWAUDIOS, (int)strlen(H_LISTAUDIOS), H_LISTAUDIOS, UTILS_sizeOf(a), a);
                PACKET_write(pack, fd);
                // Alliberem memoria

                PACKET_destroy(&pack);
                free(a);
            }
        }
        
        PACKET_destroy(&p);
    }
    free(audioFolder);
    pthread_exit(0);
    return (void *)0;
}

char *DSERVER_showFiles(char *audioFolder)
{

    char *audiosData = (char *)malloc(sizeof(char));
    

    struct dirent **namelist;
    int n;

    n = scandir("./Audios1", &namelist, NULL, alphasort);
    if (n < 0)
        perror("scandir");
    else
    {
        int i = 0;
        //n = 2;
        while (n--)
        {
            if (namelist[n]->d_type != DT_DIR)
            {
                if (i == 0)
                {
                    audiosData = (char *)realloc((void *)audiosData, sizeof(char) * UTILS_sizeOf(namelist[n]->d_name) + sizeof(char) * 1);
                    strcpy(audiosData, namelist[n]->d_name);
                    i++;
                }
                else
                {
                    audiosData = (char *)realloc((void *)audiosData, sizeof(char) * UTILS_sizeOf(audiosData) + sizeof(char) * UTILS_sizeOf(namelist[n]->d_name) + sizeof(char) * 1);
                    sprintf(audiosData, "%s\n%s", audiosData, namelist[n]->d_name);
                }
                //printf("%s\n", namelist[n]->d_name);
            }
            free(namelist[n]);
        }
        free(namelist);
    }

    /*int return_code;
    DIR *dir;
    struct dirent entry;
    struct dirent *result;

    if ((dir = opendir(audioFolder)) == NULL)
        perror("opendir() error");
    else
    {
        int i = 0;
        for (return_code = readdir_r(dir, &entry, &result);
             result != NULL && return_code == 0;
             return_code = readdir_r(dir, &entry, &result))
        {
            if (entry.d_type != DT_DIR)
            {
                if (i == 0)
                {
                    audiosData = (char *)realloc((void *)audiosData, sizeof(char) * UTILS_sizeOf(entry.d_name) + sizeof(char) * 1);
                    strcpy(audiosData, entry.d_name);
                    i++;
                }
                else
                {
                    audiosData = (char *)realloc((void *)audiosData, sizeof(char) * UTILS_sizeOf(audiosData) + sizeof(char) * UTILS_sizeOf(entry.d_name) + sizeof(char) * 1);
                    sprintf(audiosData, "%s\n%s", audiosData, entry.d_name);
                }
            }
        }

        if (return_code != 0)
            perror("readdir_r() error");
    }
    
    closedir(dir);*/

    return audiosData;
}
