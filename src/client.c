#include "../libs/client.h"

Llista servers;

int CLIENT_initClient()
{
    servers = LLISTABID_crea();
    return 1;
}

int CLIENT_checkPorts(Config config)
{

    char *ip = config.cypherIP;
    int port = config.cypherStartPort;
    int endPort = config.cypherEndPort;
    int availableConnections = 0;
    int availPorts[10];

    struct sockaddr_in s_addr;
    int socket_conn = -1;

    // Fem busqueda dins el rang de ports, guardem el nombre de connecxions trobades, i el numero de port en un array
    write(1, TESTING, sizeof(TESTING));
    while (port < endPort)
    {
        socket_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (socket_conn < 0)
            write(1, MSG_ERR_SOCKET, sizeof(MSG_ERR_SOCKET));
        else
        {

            memset(&s_addr, 0, sizeof(s_addr));
            s_addr.sin_family = AF_INET;
            s_addr.sin_port = htons(port);
            s_addr.sin_addr.s_addr = inet_addr(ip);

            if (connect(socket_conn, (void *)&s_addr, sizeof(s_addr)) < 0)
            {
                close(socket_conn);
                socket_conn = -1;
            }
            else
            {
                availPorts[availableConnections] = port;
                availableConnections++;
            }
            port++;
        }
    }

    // Un cop sabem els ports oberts, mirem si ja estava a la nostra llista de servers, per mostrar el nom en comptes de el numero de port
    // Si no hi era, el guardem a la llista, per després alhora de enviar saber quin és el nom del server
    
    char buff[128];
    int bytes = sprintf(buff, MSG_AVAIL_CONN, availableConnections);
    write(1, buff, bytes);
    LLISTABID_vesInici(&servers);

    int trobat = 0;
    for (size_t i = 0; i < availableConnections; i++)
    {
        trobat = 0;
        LLISTABID_vesInici(&servers);

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
            }
        }
    }

    return socket_conn;
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
        char *name = CLIENT_get_message(0, '\n');
        write(socket_conn, name, strlen(name));
        free(name);
        // Cal borrar fins aqui

        newServer.name = CLIENT_get_message(socket_conn, '\n');

        char buff[128];
        int bytes = sprintf(buff, MSG_CONNECTED, newServer.port, newServer.name);
        write(1, buff, bytes);

        LLISTABID_inserirDarrere(&servers, newServer);
    }

    return -1;
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
            packet.lenght = bytes;
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





// cal comprovar el cas de: connect, show connections, connect a un altre server show connections -- he provat algo similar i semblava fallar el 2n connect, pero podria ser fallo del srever del lab


// mirar de fer algunes funcions privades per client.c repeteixo molt codi, en el cas de saymessage estic fent el mateix q a checkports, que es buscar, aixo hauria de ser una funcio
// tmb estic fent el mateix a check ports i a connect, casi tota la funcio de connect hauria de ser una funcio a part q rebi ip i port, i ferla servir a connect i a checkports

/*int CLIENT_checkPorts(char* buffer) {
    //char *cmd = "./show_connections.sh 8000 8010";
   

    //int size = sprintf(buffer, "./show_connections.sh %d %d", port, endPort);

    char buf[BUFSIZE];
    FILE *fp;
    

    if ((fp = popen(buffer, "r")) == NULL) {
        printf("Error opening pipe!\n");
        return -1;
    }
    int fd = fileno(fp);

    char* buff;
    char* openPort;

    IO_readUntil(fd, &buff, ' ');
    free(buff);
    IO_readUntil(fd, &openPort, ' ');
    write(1, openPort, sizeof(openPort));
    IO_readUntil(fd, &buff, '\n');
    free(buff);

    

    if(pclose(fp))  {
        //printf("Command not found or exited with error status\n");
        return -1;
    }

    return 0;
}*/