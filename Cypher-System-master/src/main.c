#include "../libs/network.h"
#include "../libs/commandManager.h"

#define NUM_ARGS            2
#define ERR_INVALID_ARGS    "Error, parámetros incorrectos\n"


int main(int argc, char **argv) {

    if (argc != NUM_ARGS) {
        write(1, ERR_INVALID_ARGS, strlen(ERR_INVALID_ARGS));
        return 1;
    }

    NETWORK_init(argv[1]);
    char cadena[100];
    int i;
    int n;
    n=read(0,cadena,100);
    i = MANAGER_manageCommand(cadena);


    return 0;
}