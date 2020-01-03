#include "../libs/network.h"
#include <stdio.h>

int NETWORK_init(Config config)
{
    //Llegim tota la info de config
    int port = CONFIG_getMyPort(config);
    char *ip = CONFIG_getMyIP(config);
    char *name = CONFIG_getUsername(config);
    char *audios = CONFIG_getAudioFolder(config);

    trinity = SERVER_init(ip, port, name, audios);
    SERVER_setFunc(&trinity, SERVER_threadFunc);

    // Creem el thread del server principal, encarregat de gestionar les connexions entrants
    if (pthread_create(SERVER_getThread(&trinity), NULL, SERVER_threadFunc, &trinity) != 0)
    {
        // Detach perquè s'alliberi la memoria al tancar el thread
        pthread_detach(*SERVER_getThread(&trinity));
    }

    return 0;
}

int NETWORK_close()
{
    // Tancar server, threads i alliberar memoria de configuracio
    trinity.state = -1;
    CONFIG_close(&config);
    SERVER_close(&trinity);
    pthread_cancel(*SERVER_getThread(&trinity));
    pthread_join(*SERVER_getThread(&trinity), NULL);

    return 0;
}
