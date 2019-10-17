#include "../libs/network.h"
#include <stdio.h>


int NETWORK_init(char *filename) {

    Config config = CONFIG_load(filename);

   
    //Provant si funciona la lectura
    char aux[100];

    //printf("%s",CONFIG_getAudioFolder(config));//Perq funcioni amb CLion

    int n = sprintf(aux, "%s \n",CONFIG_getAudioFolder(config));
    write(1, aux, n);

    if (CONFIG_getState(config) != 0) return -1;

    //coses de server, threads

    return 0;
}

int NETWORK_close() {

    //tancar server i threads i forks

    CONFIG_close(&config);

    return 0;
}