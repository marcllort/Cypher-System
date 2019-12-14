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
    Packet p = PACKET_create(T_EXIT, H_CONOK, 0, NULL);
    PACKET_write(p, ds->fd);
    PACKET_destroy(&p);
    free(ds->user);
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
    int showconn = 0;

    char *audioFolder = (char *)malloc(sizeof(char) * UTILS_sizeOf(ds->audios));

    sprintf(audioFolder, "./%s", ds->audios);

    // Ens quedem al bucle metre no canvii el estat del server dedicat
    while (ds->state == 1)
    {
        p = PACKET_read(fd);

        if (p.type == T_CONNECT)
        {
            // En cas de voler connectar-se enviem la resposta
            if (!strcmp(p.header, H_NAME))
            {
                ds->user = malloc(p.length * sizeof(char));
                sprintf(ds->user, "%s", p.data);
                PACKET_destroy(&p);
                p = PACKET_create(T_CONNECT, H_CONOK, UTILS_sizeOf(ds->name), ds->name);
                PACKET_write(p, fd);
                char buff[128];
                int bytes = sprintf(buff, USER_CONN, ds->user);
                IO_write(1, buff, bytes);
            }
        }
        else if (p.type == T_MSG)
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
            Packet pok = PACKET_create(T_MSG, H_MSGOK, 0, NULL);
            PACKET_write(pok, ds->fd);

            // Alliberem memoria
            PACKET_destroy(&pok);
        }
        else if (p.type == T_EXIT)
        {
            // Tanquem el dedicated server en cas de desconnexio
            char buff[124];
            int bytes = sprintf(buff, USER_DISCON, ds->user);
            IO_write(1, buff, bytes);

            DSERVER_close(ds);
        }
        else if (p.type == T_DOWNLOAD)
        {
            //En cas de rebre un paquet de tipus download comprovem si es de tipus showaudios o audiorequest
            if (!strcmp(p.header, H_AUDREQ))
            {
                int sizes = UTILS_sizeOf(ds->audios) + p.length + 3;

                char *audioFolderr = (char *)malloc(sizeof(char) * sizes);
                sprintf(audioFolderr, "%s/%s", audioFolder, p.data);
                audioFolderr[sizes] = 0;
                //En cas de que sigui showaudios mirem que el fitxer existeixi
                if (UTILS_fileExists(audioFolderr) != -1)
                {
                    char buff[FRAGMENT_SIZE];
                    int counter;
                    int fd_in = open(audioFolderr, O_RDONLY);

                    char script[125];
                    sprintf(script, "md5sum %s", audioFolderr);

                    char *a = UTILS_md5(script);
                    //Obrim el fitxer i iterem fins que la mida a escriure sigui menor al buffer, que voldra dir que estem al final del fitxer
                    do
                    {
                        counter = read(fd_in, buff, FRAGMENT_SIZE);
                        Packet pack = PACKET_create(T_DOWNLOAD, H_AUDRESP, counter, buff);
                        PACKET_sendFile(pack, fd, buff);
                        PACKET_destroy(&pack);

                    } while (counter == FRAGMENT_SIZE);
                    
                    //Un cop hem acabat d'enviar el fitxer enviem el md5 amb aquest ultim paquet
                    Packet pack = PACKET_create(T_DOWNLOAD, H_AUDEOF, strlen(a), a);
                    PACKET_write(pack, fd);
                    PACKET_destroy(&pack);
                    free(a);
                }
                else
                {
                    //En cas de que el fitxer no existeixi enviem AUDIOKO
                    Packet pack = PACKET_create(T_DOWNLOAD, H_AUDKO, 0, NULL);
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
                //En cas que calgui mostrar els fitxers enviem un paquet amb els diferents fitxers
                char *a = DSERVER_showFiles(audioFolder);
                Packet pack = PACKET_create(T_SHOWAUDIOS, H_LISTAUDIOS, UTILS_sizeOf(a), a);
                PACKET_write(pack, fd);

                // Alliberem memoria
                PACKET_destroy(&pack);

                free(a);
            }
        }
        else
        {
            ds->state = -1;
            showconn = -1;
        }
        if (showconn != -1)
        {
            PACKET_destroy(&p);
        }
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
    n = scandir(audioFolder, &namelist, NULL, alphasort);
    if (n < 0)
    {
        audiosData = (char *)realloc((void *)audiosData, sizeof(char) * UTILS_sizeOf("Empty folder!\n"));
        strcpy(audiosData, "Empty folder!\n");
    }
    else
    {
        int i = 0;
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
            }
            free(namelist[n]);
        }
        free(namelist);
    }

    return audiosData;
}
