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
            //char buff[128];
            write(1, MSG_ERR_CONN, sizeof(MSG_ERR_CONN));
            //int bytes = sprintf(buff, "errno says: %s\n", strerror(errno)); // molt útil
            //write(1, buff, bytes);
            close(socket_conn);
            socket_conn = -1;
        }
    }

    

    newServer.port = connectPort;
    newServer.socketfd = socket_conn;
    char buff[128];
    int bytes = sprintf(buff, "socketfd: %d\n", newServer.socketfd);
    write(1, buff, bytes);

    char* name = CLIENT_get_message(0, '\n');
	write(socket_conn, name, strlen(name));
    

    char* question = CLIENT_get_message(socket_conn, '\n');
    write(1, question, strlen(question));

    //newServer.name = CLIENT_get_message(socket_conn, '\n');
    //printf("%s", newServer.name);

    return socket_conn;
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

int CLIENT_sayMessage(char *user, char *message, int socketfd)
{
}

//fer funcio de FREE