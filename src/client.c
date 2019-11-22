#include "../libs/client.h"

Llista servers;

int CLIENT_initClient()
{
    servers = LLISTABID_crea();
    return 1;
}

char *CLIENT_get_message(int fd, char delimiter)
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

int CLIENT_runScript(char *buffer)
{

    int status;
    pid_t pid;

    if ((pid = fork()) < 0)
    {
        perror("\nError en el fork");
        exit(-1);
    }
    else if (pid == 0)
    {

        if ((execl("/bin/sh", "/bin/sh", "-c", buffer, 0) < 0))
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
        waitpid(pid, &status, 0);
    }
}

int CLIENT_checkPorts(char *buffer)
{
    FILE *fp;
    char *buffr;
    char *openPort;
    int availPorts[10];
    int availableConnections = 0;

    CLIENT_runScript(buffer);

    int fd = IO_openFile("output");

    while (1)
    {
        IO_readUntil(fd, &buffr, ' ');
        free(buffr);

        IO_readUntilv2(fd, &openPort, ' ');
        availPorts[availableConnections] = atoi(openPort);
        free(openPort);

        IO_readUntil(fd, &buffr, '\n');
        free(buffr);
        if (availPorts[0] != 0)
        {
            availableConnections++;
        }

        if (checkEOF(fd) == 1)
            break;
    }

    // Un cop sabem els ports oberts, mirem si ja estava a la nostra llista de servers, per mostrar el nom en comptes de el numero de port
    // Si no hi era, el guardem a la llista, per després alhora de enviar saber quin és el nom del server

    char buff[128];
    int bytes = sprintf(buff, MSG_AVAIL_CONN, availableConnections);
    write(1, buff, bytes);

    int trobat = 0;
    for (size_t i = 0; i < availableConnections; i++)
    {
        trobat = 0;

        // Si esta a la llista, printa port i nom, sino el port sol
        if (LLISTABID_buida(servers))
        {
            bytes = sprintf(buff, "%d\n", availPorts[i]);
            write(1, buff, bytes);
        }
        else
        {
            while (!LLISTABID_final(servers) && !trobat)
            {
                Element server = LLISTABID_consulta(servers);
                if (server.port == availPorts[i])
                {
                    bytes = sprintf(buff, "%d %s\n", availPorts[i], server.name);
                    write(1, buff, bytes);
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
                write(1, buff, bytes);
                LLISTABID_vesInici(&servers);
            }
        }
    }

    CLIENT_runScript("rm output");

    return 0;
}

int CLIENT_connectPort(Config config, int connectPort)
{
    Element newServer;
    char *ip = config.cypherIP;

    struct sockaddr_in s_addr;
    int socket_conn = -1;

    socket_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket_conn < 0)
        write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
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
            write(1, buff, bytes);
            close(socket_conn);
            socket_conn = -1;
        }
    }
    if (socket_conn != -1)
    {
        newServer.port = connectPort;
        newServer.socketfd = socket_conn;

        // Per provar amb server sessio lab 4 -- envio nom al server IMPORTANT BORRARRRRRRRRRRRRR QUAN TINGUEM EL NOSTRE SERVEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEER
        char *name = CLIENT_read(0, '\n');
        write(socket_conn, name, strlen(name));
        free(name);
        // Cal borrar fins aqui

        //IMPORTANT POSAR newServer.name = CLIENT_get_message(socket_conn, '\n');
        CLIENT_get_message(socket_conn, '\n'); //borrar
        newServer.name = "prova";              //CAL BORRAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAR

        char buff[128];
        int bytes = sprintf(buff, MSG_CONNECTED, newServer.port, newServer.name);
        write(1, buff, bytes);
        LLISTABID_inserirDarrere(&servers, newServer);
    }

    return 0;
}

int CLIENT_write(char *user, char *message)
{
    int trobat = 0;
    LLISTABID_vesInici(&servers);
    char buff[128];
    int bytes;

    while (!LLISTABID_final(servers) && !trobat)
    {
        Element server = LLISTABID_consulta(servers);
        if (strcmp(server.name, user) == 0) //strcmp("prova", user) serveix per provar el say, si fas connect i despres say prova sallefest, funciona
        {

            Packet packet;
            bytes = sprintf(buff, "%s\n", message);

            packet.type = 0x01;
            packet.header = "[MSG]";
            packet.length = bytes;
            packet.data = buff;

            //void *ptr = &buff; Serveix per provar enviar missatge en comptes de paquet
            void *ptr = &packet;

            write(server.socketfd, ptr, bytes); //Cal provar si funciona, fent cast desde el server
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
        write(1, buff, bytes);
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
    LLISTABID_vesInici(&servers);
    while (!LLISTABID_final(servers))
    {
        Element server = LLISTABID_consulta(servers);
        close(server.socketfd);
        free(server.name);
    }
    LLISTABID_destrueix(&servers);

    return 0;
}
