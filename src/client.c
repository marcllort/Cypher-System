#include "../libs/client.h"

Llista servers;
Config config;
int lastfd;

int CLIENT_initClient()
{
    // Creem llista bidireccional de on emmagatzerem els servers (nom, fd, socket)
    servers = LLISTABID_crea();
    return 1;
}

int CLIENT_checkPorts(char *buffer, Config inConfig)
{
    char *buffr;
    char *openPort;
    int availPorts[10];
    int availableConnections = 0;
    int fd[2];
    int status;
    pid_t pid;
    config = inConfig;
    int pipereturn = pipe(fd);

    if (pipereturn == -1)
    {
        IO_write(1, "Pipe pum pum", sizeof("Pipe pum pum"));
    }

    if ((pid = fork()) < 0)
    {
        perror("\nError en el fork");
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
            perror("\nError en el execl");
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
                        LLISTABID_vesInici(&servers);
                    }
                }
            }
        }
    }

    return 0;
}

int CLIENT_connectPort(Config config, int connectPort)
{
    // Connexio al server, llegim la classe config on tenim la informació necessaria

    Element newServer;
    char *ip = config.cypherIP;
    struct sockaddr_in s_addr;
    int socket_conn = -1;

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
            Packet p = PACKET_create(T_CONNECT, (int)strlen(H_NAME), H_NAME, (int)UTILS_sizeOf(config.username), config.username);
            PACKET_write(p, socket_conn);

            Packet j = PACKET_read(socket_conn);

            newServer.name = j.data;
            free(j.header);

            LLISTABID_inserirDarrere(&servers, newServer);

            int bytes = sprintf(buff, "%d connected: %s\n", newServer.port, newServer.name);
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
            if (strcmp(server.name, user) == 0)
            {
                Packet packet = PACKET_create(T_MSG, (int)strlen(H_MSG), H_MSG, UTILS_sizeOf(message), message);
                PACKET_write(packet, server.socketfd);
                PACKET_destroy(&packet);

                lastfd = server.socketfd;

                Packet readed = PACKET_read(server.socketfd);
                PACKET_destroy(&readed);
                trobat = 1;
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

            if (strcmp(server.name, user) == 0)
            {
                Packet p = PACKET_create(T_SHOWAUDIOS, (int)strlen(H_SHOWAUDIOS), H_SHOWAUDIOS, 0, NULL);
                PACKET_write(p, server.socketfd);
                PACKET_destroy(&p);

                Packet pa = PACKET_read(server.socketfd);
                IO_write(1, pa.data, pa.length);
                IO_write(1, "\n", 1);

                PACKET_destroy(&pa);

                trobat = 1;
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
            if (strcmp(server.name, user) == 0)
            {
                Packet psend = PACKET_create(T_DOWNLOAD, (int)strlen(H_AUDREQ), H_AUDREQ, UTILS_sizeOf(filename), filename);
                //IO_write(1,filename,UTILS_sizeOf(filename));
                PACKET_write(psend, server.socketfd);
                PACKET_destroy(&psend);

                Packet pa = PACKET_read(server.socketfd);

                if (!strcmp(H_AUDKO, pa.header))
                {
                    IO_write(1, "\nError, fichero inexistente\n", strlen("\nError, fichero inexistente\n"));
                }
                if (!strcmp(pa.header, H_AUDRESP))
                {
                    IO_write(1, "\nDescargando...\n", strlen("\nDescargando...\n"));
                    int fd1 =open(filename, O_WRONLY | O_CREAT, 0666);
                    
                   
                    do
                    {
                        write (fd1, pa.data, pa.length);
                        //write (1, pa.data, pa.length);
                        write (1, "aa", 2);
                        //PACKET_destroy(&pa);
                        pa = PACKET_read(server.socketfd);
                       write (1, pa.header, strlen(pa.header));
                        
                    } while (strcmp(pa.header, H_AUDEOF));
                    
                }

                 write(1,"abc",3);
                PACKET_destroy(&pa);

                trobat = 1;
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

int CLIENT_exit()
{
    // Funció per enviar paquets de desconnexio a tots els usuaris connectats

    LLISTABID_vesInici(&servers);

    while (!LLISTABID_final(servers))
    {
        Element server = LLISTABID_consulta(servers);

        Packet packet = PACKET_create(T_EXIT, (int)strlen(H_VOID), H_VOID, UTILS_sizeOf(config.username), config.username);
        PACKET_write(packet, server.socketfd);
        PACKET_destroy(&packet);
        
        // Llegim resposta de desconnexio OK (protocol desconnexio)
        Packet p = PACKET_read(server.socketfd);
        if(!UTILS_isEmpty(p.header)){
            PACKET_destroy(&p);
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
    LLISTABID_vesInici(&servers);
    while (!LLISTABID_final(servers) && !trobat)
    {
        Element server = LLISTABID_consulta(servers);

        if (server.socketfd == fd)
        {
            LLISTABID_elimina(&servers);
            trobat = 1;
        }
        else
        {
            LLISTABID_avanca(&servers);
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
    // Borrem el usuari amb el que no hem pogut connectar
    int i = CLIENT_borraUser(lastfd);
    // En cas de que retorni 0, vol dir que s'ha pogut borrar
    if (i == 0)
    {
        IO_write(1, UNKNOWN_CONNECTION2, sizeof(UNKNOWN_CONNECTION2));
    }
}

int CLIENT_freeMemory()
{
    // Funció de lliberar memoria del CLIENT on borrem la llista de servers
    LLISTABID_destrueix(&servers);

    return 0;
}
