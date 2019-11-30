#include "../libs/client.h"

Llista servers;
Config config;

int CLIENT_initClient()
{
    // Creem llista bidireccional de on emmagatzerem els servers (nom,fd)
    servers = LLISTABID_crea();
    return 1;
}

char *CLIENT_get_message(int fd, char delimiter)
{
    // Funció encarregada de capturar el missatge de un FD i retornar el char*
    char *msg = (char *)malloc(1);
    char current;
    int i = 0;

    while (read(fd, &current, 1) > 0)
    {

        msg[i] = current;
        msg = (char *)realloc(msg, ++i + 1);

        if (current == delimiter)
            break;
    }

    msg[i] = '\0';

    return msg;
}

int CLIENT_runScript(char *buffer)
{
    // Funció per correr un script bash
    // Cal fer fork, ja que execl s'apropia de el procés, executa script i mor
    int status;
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        perror("\nError en el fork");
        exit(-1);
    }
    else if (pid == 0)
    { // El fill es on s'executa el script rebut per parametres, i al acabar mor

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
    { // El pare espera a que acabi la execució del fill
        waitpid(pid, &status, 0);
        return 0;
    }
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
    { // El fill es on s'executa el script rebut per parametres, i al acabar mor
        IO_write(1, TESTING, sizeof(TESTING));
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        if ((execl("/bin/sh", "/bin/sh", "-c", buffer, (char *)0) < 0)) // Executem script entre X i Y ports rebut per parametres a buffer
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
    { // El pare espera a que acabi la execució del fill
        waitpid(pid, &status, 0);
        close(fd[1]);

        while (1) // El script ha guardat en un pipe, el llegim
        {
            if (checkEOF(fd[0]) == 1)
                break;

            IO_readUntil(fd[0], &buffr, ' '); // Ens quedem només amb el nombre del port obert
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

            if (checkEOF(fd[0]) == 1)
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

        if (socket_conn < 0)
            IO_write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
        else
        {

            memset(&s_addr, 0, sizeof(s_addr));
            s_addr.sin_family = AF_INET;
            s_addr.sin_port = htons(connectPort);
            s_addr.sin_addr.s_addr = inet_addr(ip);

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
            newServer.port = connectPort;
            newServer.socketfd = socket_conn;

            //IMPORTANT POSAR newServer.name = CLIENT_get_message(socket_conn, '\n');
            char buff[128];
            Packet p = PACKET_create(T_CONNECT, (int)strlen(H_NAME), H_NAME, (int)strlen(config.username), config.username);
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

int CLIENT_msgConnection()
{
    return 0;
}

int CLIENT_write(char *user, char *message)
{
    // Funció per enviar un paquet a un altre usuari, al que previament estàs connectat

    int trobat = 0;
    LLISTABID_vesInici(&servers);
    char buff[128];
    int bytes;

    // Busquem a la llista la relació de nom i port per així saber a quin fd hem de enviar
    while (!LLISTABID_final(servers) && !trobat)
    {
        Element server = LLISTABID_consulta(servers);

        if (strcmp(server.name, user) == 0)
        {
            Packet packet;
            packet.type = 0x02;
            packet.header = "[MSG]";
            packet.length = UTILS_sizeOf(message);
            packet.data = message;

            PACKET_write(packet, server.socketfd);

            PACKET_read(server.socketfd);
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
    return 1;
}

int CLIENT_exit()
{
    // Funció per enviar un paquet a un altre usuari, al que previament estàs connectat

    LLISTABID_vesInici(&servers);

    while (!LLISTABID_final(servers))
    {
        Element server = LLISTABID_consulta(servers);

        Packet packet;

        packet.type = 0x06;
        packet.header = "[]";
        packet.length = UTILS_sizeOf(config.username);
        packet.data = config.username;

        PACKET_write(packet, server.socketfd);

        PACKET_read(server.socketfd); //Llegim desconnexio OK
        close(server.socketfd);
        free(server.name);

        LLISTABID_avanca(&servers);
    }

    return 1;
}

char *CLIENT_read(int fd, char delimiter)
{
    char *msg = (char *)malloc(1);
    char current;
    int i = 0;

    while (read(fd, &current, 1) > 0)
    {
        msg[i] = current;
        msg = (char *)realloc(msg, ++i + 1);

        if (current == delimiter)
            break;
    }

    msg[i] = '\0';

    return msg;
}

int CLIENT_freeMemory()
{
    // Funció de lliberar memoria del CLIENT

    LLISTABID_destrueix(&servers);

    return 0;
}
