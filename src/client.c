#include "../libs/client.h"

int CLIENT_checkPorts(Config config)
{

    char *ip = config.cypherIP;
    int port = config.cypherStartPort;
    int endPort = config.cypherEndPort;
    int availableConnections = 0;
    int availPorts[10];

    struct sockaddr_in s_addr;
    int socket_conn = -1;

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
                //char buff[128];
                //int bytes = sprintf(buff, MSG_ERR_CONN, port);
                //write(1, buff, bytes);
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

    char buff[128];
    int bytes = sprintf(buff, MSG_AVAIL_CONN, availableConnections);
    write(1, buff, bytes);

    for (size_t i = 0; i < availableConnections; i++)
    {
        //if esta a la llista, printa port i nom, sino el port sol
        bytes = sprintf(buff, "%d\n", availPorts[i]);
        write(1, buff, bytes);
    }

    return socket_conn;
}

int CLIENT_connectPort(Config config, int connectPort)
{
    Server newServer;
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
            write(1, MSG_ERR_CONN, sizeof(MSG_ERR_CONN));
            close(socket_conn);
            socket_conn = -1;
        }
    }

    newServer.port = connectPort;
    newServer.socketfd = socket_conn;

    //Per provar amb server sessio lab 4 -- envio nom al server
    char *name = CLIENT_get_message(0, '\n');
    write(socket_conn, name, strlen(name));

    newServer.name = CLIENT_get_message(socket_conn, '\n');

    char buff[128];
    int bytes = sprintf(buff, MSG_CONNECTED, newServer.port, newServer.name);
    write(1, buff, bytes);


    // Només falta guardar en una estructura de dades (LLista/MAP) on posem la relacio. estreuctura ja esta creada, es server, cal posarlo a la llista nomes

    return newServer.socketfd;
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

int CLIENT_sayMessage(char *user, char *message)
{

    //fer un write i ja, primer a partir del username, buscar el socket al q cal enviar
 

}

//fer funcio de FREE