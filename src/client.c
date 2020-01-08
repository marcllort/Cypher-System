#include "../libs/client.h"

Llista servers;
Config config;
int lastfd;

int CLIENT_initClient(Config inConfig)
{
    // Creem llista bidireccional de on emmagatzerem els servers (nom, fd, socket)
    servers = LLISTABID_crea();
    config = inConfig;
    return 1;
}

int CLIENT_checkPorts(char *buffer)
{
    char *buffr;
    char *openPort;
    int availPorts[10];
    int availableConnections = 0;
    int fd[2];
    int status;
    pid_t pid;
    int pipereturn = pipe(fd);

    // Creem pipe per capturar el output que hauria de sortir per pantalla
    if (pipereturn == -1)
    {
        IO_write(1, PIPE_ERR, sizeof(PIPE_ERR));
    }

    if ((pid = fork()) < 0)
    {
        IO_write(1, FORK_ERR, sizeof(FORK_ERR));
        exit(-1);
    }
    else if (pid == 0)
    {
        // El fill es on s'executa el script rebut per parametres, i al acabar mor
        IO_write(1, TESTING, sizeof(TESTING));
        // Dupliqeum el pipe per així llegir el que hauria de sortir per pantalla
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        // Executem script entre X i Y ports rebut per parametres a buffer
        if ((execl("/bin/sh", "/bin/sh", "-c", buffer, (char *)0) < 0))
        {
            IO_write(1, EXECL_ERR, sizeof(EXECL_ERR));
            exit(-1);
        }
        else
        {
            exit(0);
        }
    }
    else
    {
        // El pare espera a que acabi la execució del fill
        waitpid(pid, &status, 0);
        close(fd[1]);

        // El script ha guardat en un pipe, el llegim
        while (1)
        {
            if (IO_checkEOF(fd[0]) == 1)
                break;

            // Ens quedem només amb el nombre del port obert
            IO_readUntil(fd[0], &buffr, ' ');
            free(buffr);

            IO_readUntilv2(fd[0], &openPort, ' ');
            availPorts[availableConnections] = atoi(openPort);
            free(openPort);

            IO_readUntil(fd[0], &buffr, '\n');
            free(buffr);
            if (availPorts[0] != 0)
            {
                availableConnections++;
            }

            if (IO_checkEOF(fd[0]) == 1)
                break;
        }

        // Un cop sabem els ports oberts, mirem si ja estava a la nostra llista de servers, per mostrar el nom en comptes de el numero de port
        // Si no hi era, el guardem a la llista, per després alhora de enviar saber quin és el nom del server
        char buff[128];
        int bytes = sprintf(buff, MSG_AVAIL_CONN, availableConnections - 1);
        IO_write(1, buff, bytes);

        int trobat = 0;

        for (int i = 0; i < availableConnections; i++)
        {
            trobat = 0;
            if (availPorts[i] != config.myPort)
            {
                // Si esta a la llista, printa port i nom, sino el port sol
                if (LLISTABID_buida(servers))
                {
                    bytes = sprintf(buff, "%d\n", availPorts[i]);
                    IO_write(1, buff, bytes);
                }
                else
                {
                    LLISTABID_vesInici(&servers);
<<<<<<< HEAD
=======

>>>>>>> EntregaF3
                    while (!LLISTABID_final(servers) && !trobat)
                    {
                        Element server = LLISTABID_consulta(servers);
                        if (server.port == availPorts[i])
                        {
                            bytes = sprintf(buff, "%d %s\n", availPorts[i], server.name);
                            IO_write(1, buff, bytes);
                            trobat = 1;
                            LLISTABID_vesInici(&servers);
                        }
                        else
                        {
                            LLISTABID_avanca(&servers);
                        }
                    }
                    if (!trobat)
                    {
                        bytes = sprintf(buff, "%d\n", availPorts[i]);
                        IO_write(1, buff, bytes);
                    }
                }
            }
        }
    }

    return 0;
}
//Funcio que comproba que tots els servers que hi ha a la llista segueixen disponibles
int CLIENT_checkConnections()
{
    if (!LLISTABID_buida(servers))
    {
        LLISTABID_vesInici(&servers);

        while (!LLISTABID_final(servers))
        {
            //Prova d'enviar alguna cosa a tots els servers i en cas que no pugui enviar, l'esborra de la llista
            Element server = LLISTABID_consulta(servers);
            int error = write(server.socketfd, " ", 1);
            if (error == -1)
            {
                lastfd = server.socketfd;
                CLIENT_borraUser(lastfd);
            }
            LLISTABID_avanca(&servers);
        }
    }
    if (LLISTABID_buida(servers))
    {
        LLISTABID_destrueix(&servers);
        servers = LLISTABID_crea();
    }
    return 1;
}

int CLIENT_connectPort(Config config, int connectPort)
{
    // Connexio al server, llegim la classe config on tenim la informació necessaria
    Element newServer;
    char *ip = config.cypherIP;
    struct sockaddr_in s_addr;
    int socket_conn = -1;

    if (!LLISTABID_buida(servers))
    {
        LLISTABID_vesInici(&servers);

        while (!LLISTABID_final(servers))
        {
            Element server = LLISTABID_consulta(servers);
            
            if (connectPort == server.port)
            {
                IO_write(1, CLIENT_ALREADY_CONNECTED, strlen(CLIENT_ALREADY_CONNECTED));
                return 0;
            }
            if(!LLISTABID_final(servers)){
                LLISTABID_avanca(&servers);
            }
        }
    }
    if (connectPort == config.myPort)
    {
        IO_write(1, MSG_ERR_PORT, sizeof(MSG_ERR_PORT));
    }
    else
    {
        socket_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        // Mostrem error de socket mal creat
        if (socket_conn < 0)
        {
            IO_write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
        }
        else
        {
            memset(&s_addr, 0, sizeof(s_addr));
            s_addr.sin_family = AF_INET;
            s_addr.sin_port = htons(connectPort);
            s_addr.sin_addr.s_addr = inet_addr(ip);

            // Creem socket de connexio
            if (connect(socket_conn, (void *)&s_addr, sizeof(s_addr)) < 0)
            {
                char buff[128];
                int bytes = sprintf(buff, MSG_ERR_CONN, connectPort);
                IO_write(1, buff, bytes);
                close(socket_conn);
                socket_conn = -1;
            }
        }
        if (socket_conn != -1)
        {
            // Enviem primer missatge seguint protocol comunicacio
            newServer.port = connectPort;
            newServer.socketfd = socket_conn;

            char buff[128];
            Packet p = PACKET_create(T_CONNECT, H_NAME, UTILS_sizeOf(config.username), config.username);
            PACKET_write(p, socket_conn);
            // Esperem la resposta del server
            Packet j = PACKET_read(socket_conn);

            newServer.name = j.data;
            free(j.header);
            // L'afegim a la llista de servers disponibles
            if (!LLISTABID_buida(servers))
            {
                LLISTABID_vesInici(&servers);
            }
            LLISTABID_inserirDarrere(&servers, newServer);

            int bytes = sprintf(buff, MSG_CONNECTED, newServer.port, newServer.name);
            IO_write(1, buff, bytes);
            PACKET_destroy(&p);
        }
    }
    return 0;
}

int CLIENT_write(char *user, char *message)
{
    // Funció per enviar un paquet a un altre usuari, al que previament estàs connectat
    int trobat = 0;
    char buff[128];
    int bytes;

    if (!LLISTABID_buida(servers))
    {
        LLISTABID_vesInici(&servers);

        // Busquem a la llista la relació de nom i port per així saber a quin fd hem de enviar
        while (!LLISTABID_final(servers) && !trobat)
        {
            Element server = LLISTABID_consulta(servers);

            // Enviem packet amb el missatge i esperem resposta (protocol comunicacio)
            if (UTILS_compareCaseInsensitive(server.name, user) == 0)
            {
                Packet packet = PACKET_create(T_MSG, H_MSG, UTILS_sizeOf(message), message);
                int error = PACKET_write(packet, server.socketfd);
                PACKET_destroy(&packet);
                if (error != -1)
                {
                    lastfd = server.socketfd;

                    Packet readed = PACKET_read(server.socketfd);
                    PACKET_destroy(&readed);
                    trobat = 1;
                }
                else
                {
                    lastfd = server.socketfd;
                    CLIENT_borraUser(lastfd);
                }
            }
            else
            {
                LLISTABID_avanca(&servers);
            }
        }
        if (!trobat)
        {
            // En cas de no trobarlo a la llista, mostrem error
            bytes = sprintf(buff, UNKNOWN_CONNECTION, user);
            IO_write(1, buff, bytes);
        }
    }
    else
    {
        // En cas de no trobarlo a la llista, mostrem error
        bytes = sprintf(buff, UNKNOWN_CONNECTION, user);
        IO_write(1, buff, bytes);
    }
    return 1;
}

int CLIENT_showAudios(char *user)
{
    // Funció per enviar un paquet a un altre usuari, i que aquest et retorni els seus audios disponibles
    int trobat = 0;
    char buff[128];
    int bytes;

    if (!LLISTABID_buida(servers))
    {
        LLISTABID_vesInici(&servers);

        // Busquem a la llista la relació de nom i port per així saber a quin fd hem de enviar
        while (!LLISTABID_final(servers) && !trobat)
        {
            Element server = LLISTABID_consulta(servers);

            if (UTILS_compareCaseInsensitive(server.name, user) == 0)
            {
                // Enviem el request de llista de audios i llegim la llista que ens retorna
                Packet p = PACKET_create(T_SHOWAUDIOS, H_SHOWAUDIOS, 0, NULL);
                int error = PACKET_write(p, server.socketfd);
                PACKET_destroy(&p);
                if (error != -1)
                {
                    lastfd = server.socketfd;
                    Packet pa = PACKET_read(server.socketfd);
                    IO_write(1, pa.data, pa.length);
                    IO_write(1, "\n", 1);

                    PACKET_destroy(&pa);
                    trobat = 1;
                }
                else
                {
                    lastfd = server.socketfd;
                    CLIENT_borraUser(lastfd);
                }
            }
            else
            {
                LLISTABID_avanca(&servers);
            }
        }
        if (!trobat)
        {
            bytes = sprintf(buff, UNKNOWN_CONNECTION, user);
            IO_write(1, buff, bytes);
        }
    }
    else
    {
        bytes = sprintf(buff, UNKNOWN_CONNECTION, user);
        IO_write(1, buff, bytes);
    }
    return 1;
}

int CLIENT_download(char *user, char *filename)
{
    // Funció per enviar un paquet per solicitar la descarrega d'un audio a un servidor i guardar l'arxiu a la carpeta d'audios del client
    int trobat = 0;
    char buff[128];
    int bytes;

    char *file = malloc(strlen(filename));
    strcpy(file, filename);
    if (!LLISTABID_buida(servers))
    {
        LLISTABID_vesInici(&servers);

        // Busquem a la llista la relació de nom i port per així saber a quin fd hem de enviar
        while (!LLISTABID_final(servers) && !trobat && trobat != -10)
        {
            Element server = LLISTABID_consulta(servers);
            if (UTILS_compareCaseInsensitive(server.name, user) == 0)
            {
                //Enviem el nom del fitxer i esperem la resposta del server
                Packet psend = PACKET_create(T_DOWNLOAD, H_AUDREQ, UTILS_sizeOf(filename), filename);

                int error = PACKET_write(psend, server.socketfd);
                PACKET_destroy(&psend);
                if (error != -1)
                {
                    lastfd = server.socketfd;
                    Packet pa = PACKET_read(server.socketfd);

                    if (!strcmp(H_AUDKO, pa.header))
                    {
                        IO_write(1, NOFILE, strlen(NOFILE));
                        trobat = -10;
                    }
                    else if (!strcmp(pa.header, H_AUDRESP))
                    {

                        // Guardem el fitxer a la carpeta de audios
                        char *path = malloc(strlen(filename) + strlen(CONFIG_getAudioFolder(config) + 1));
                        sprintf(path, "%s/%s", CONFIG_getAudioFolder(config), filename);

                        int fd1 = open(path, O_WRONLY | O_TRUNC | O_CREAT, 0666);

                        // En cas de que el server ens dongui el ok per descarregar iniciem la lectura de paquets fins trobar un paquet amb capçalera EOF
                        IO_write(1, DOWNLOADING, strlen(DOWNLOADING));

                        // Fem un bucle de lectura per anar "muntant" el fixer
                        do
                        {
                            IO_write(fd1, pa.data, pa.length);
                            PACKET_destroy(&pa);
                            pa = PACKET_read(server.socketfd);

                        } while (strcmp(pa.header, H_AUDEOF) != 0);

                        close(fd1);
                        char *script = malloc(sizeof(char) * (7 + strlen(path)));
                        sprintf(script, "md5sum %s", path);
                        free(path);

                        char *md5 = UTILS_md5(script);
                        md5[32] = 0;
                        free(script);

                        // Comparem el md5 per saber si la descarrega ha estat correcta
                        if (UTILS_compare(pa.data, md5, 32) == 0)
                        {
                            char buff[128];
                            int bytes = sprintf(buff, FILE_DOWNLOADED, file);
                            IO_write(1, buff, bytes);
                        }
                        else
                        {
                            IO_write(1, FILE_DOWNLOAD_KO, strlen(FILE_DOWNLOAD_KO));
                        }
                        free(md5);
                    }

                    PACKET_destroy(&pa);
                    trobat = 1;
                }
                else
                {
                    lastfd = server.socketfd;
                    CLIENT_borraUser(lastfd);
                }
            }
            else
            {
                LLISTABID_avanca(&servers);
            }
        }
        if (!trobat)
        {
            bytes = sprintf(buff, UNKNOWN_CONNECTION, user);
            IO_write(1, buff, bytes);
        }
    }
    else
    {
        bytes = sprintf(buff, UNKNOWN_CONNECTION, user);
        IO_write(1, buff, bytes);
    }
    free(file);
    return 1;
}

int CLIENT_exit()
{
    // Funció per enviar paquets de desconnexio a tots els usuaris connectats
    LLISTABID_vesInici(&servers);

    while (!LLISTABID_final(servers))
    {
        Element server = LLISTABID_consulta(servers);

        Packet packet = PACKET_create(T_EXIT, H_VOID, UTILS_sizeOf(config.username), config.username);
        int error = PACKET_write(packet, server.socketfd);
        PACKET_destroy(&packet);
        if (error != -1)
        {
            // Llegim resposta de desconnexio OK (protocol desconnexio)
            Packet p = PACKET_read(server.socketfd);
            if (strcmp(p.header, H_CONOK) == 0)
            {
                PACKET_destroy(&p);
            }
            else
            {
                IO_write(1, DISCON_SERVER_ERR, sizeof(DISCON_SERVER_ERR));
            }
        }
        // Tanquem els fd i free de memoria
        close(server.socketfd);
        free(server.name);

        LLISTABID_avanca(&servers);
    }

    return 1;
}

int CLIENT_borraUser(int fd)
{
    // Funcio per borrar un usuari de la llista quan es desconnecta
    int trobat = 0;

    // Iterem per tota la llista fins trobarlo i borrarlo
    if (!LLISTABID_buida(servers))
    {
        LLISTABID_vesInici(&servers);
        while (!LLISTABID_final(servers) && !trobat)
        {
            Element server = LLISTABID_consulta(servers);
            if (server.socketfd == fd)
            {
                free(server.name);
                LLISTABID_elimina(&servers);
                trobat = 1;
            }
            else
            {
                LLISTABID_avanca(&servers);
            }
        }
    }
    if (!trobat)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void CLIENT_messageError()
{
    
}

int CLIENT_freeMemory()
{
    // Funció de lliberar memoria del CLIENT on borrem la llista de servers
    LLISTABID_destrueix(&servers);

    return 0;
}

int CLIENT_broadcast(char *message)
{
    // Funció per enviar paquets de broadcast a tots els usuaris connectats
    if (!LLISTABID_buida(servers))
    {
        LLISTABID_vesInici(&servers);

        while (!LLISTABID_final(servers))
        {
            Element server = LLISTABID_consulta(servers);

            Packet packet = PACKET_create(T_BROADCAST, H_BROADCAST, UTILS_sizeOf(message), message);
            int error = PACKET_write(packet, server.socketfd);
            PACKET_destroy(&packet);
            if (error != -1)
            {
                // Llegim resposta de message OK (protocol broadcast)
                Packet p = PACKET_read(server.socketfd);
                if (strcmp(p.header, H_MSGOK) == 0)
                {
                    PACKET_destroy(&p);
                }
                else
                {
                    IO_write(1, DISCON_SERVER_ERR, sizeof(DISCON_SERVER_ERR));
                }
            }
            else
            {
                lastfd = server.socketfd;
                CLIENT_borraUser(lastfd);
            }

            LLISTABID_avanca(&servers);
        }
    }
    return 1;
}