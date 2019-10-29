#include "../libs/network.h"
#include <stdio.h>


int NETWORK_init(Config config) {
    if (CONFIG_getState(config) != 0) return -1;

    //coses de server, threads

    return 0;
}

int NETWORK_close() {

    //tancar server i threads i forks

    CONFIG_close(&config);

    return 0;
}