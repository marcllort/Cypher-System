#include "../libs/network.h"
#include <stdio.h>

int NETWORK_init(Config config)
{

    int port = CONFIG_getMyPort(config);
    char *ip = CONFIG_getMyIP(config);
    char *name = CONFIG_getUsername(config);

    trinity = SERVER_init(ip, port, name);
    SERVER_setFunc(&trinity, SERVER_threadFunc);

    if (pthread_create(SERVER_getThread(&trinity), NULL, SERVER_threadFunc, &trinity) != 0)
    {
        return 1;
    }
    if (pthread_detach(*SERVER_getThread(&trinity)) != 0) {
        return 1;
    }
    return 0;
}

int NETWORK_close()
{
    trinity.state = -1;
    //tancar server i threads i forks
    CONFIG_close(&config);
    SERVER_close(&trinity);
    IO_write(1, "SERVER_close", strlen("SERVER_close"));
    pthread_cancel(*SERVER_getThread(&trinity));
    pthread_join(*SERVER_getThread(&trinity), NULL);

    return 0;
}
