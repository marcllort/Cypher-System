#include "../libs/network.h"

#define NUM_ARGS            2
#define ERR_INVALID_ARGS    "Error, parámetros incorrectos\n"


int main(int argc, char **argv) {

    if (argc != NUM_ARGS) {
        write(1, ERR_INVALID_ARGS, strlen(ERR_INVALID_ARGS));
        return 1;
    }

    NETWORK_init(argv[1]);

    return 0;
}