#include "../libs/network.h"
#include <stdio.h>

#define MSG_CONNECTED "Connection Cyper -%s ready.\n"
#define MSG_DISCONNECTED "Disconnecting from %s.\n"


int NETWORK_init(Config config)
{

    //if (CONFIG_getState(config) != 0) return -1;

    //coses de server: reb connexio, crea threads
    //char buff [100];
    int port = CONFIG_getMyPort(config);
    char *ip = CONFIG_getMyIP(config);
    char *name = CONFIG_getUsername(config);
    //int bytes = sprintf(buff, "NETWORK PORT : %d IP: %s knjk\n",port ,ip);
    //write(1,buff, sizeof(buff));

    trinity = SERVER_init(ip, port, name);
    SERVER_setMT(&trinity, SERVER_threadFunc);

    if (pthread_create(SERVER_getThread(&trinity), NULL, SERVER_threadFunc, &trinity) != 0)
    { // CAL FER PTHREAD JOIN AL ACABAR, SINO MEMORY LEAK
        return 1;
    }
    return 0;
}

int NETWORK_close()
{
    trinity.state=-1;
    //tancar server i threads i forks
    CONFIG_close(&config);
    SERVER_close(&trinity);
    pthread_cancel(*SERVER_getThread(&trinity));
    pthread_join(*SERVER_getThread(&trinity), NULL);
    

    return 0;
}

