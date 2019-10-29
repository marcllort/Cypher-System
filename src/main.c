#include "../libs/network.h"
#include "../libs/commandManager.h"

#define NUM_ARGS 2
#define ERR_INVALID_ARGS "Error, parámetros incorrectos\n"

void MAIN_registerSignal();
void MAIN_freeMemory();

int main(int argc, char **argv)
{

    if (argc != NUM_ARGS)
    {
        write(1, ERR_INVALID_ARGS, strlen(ERR_INVALID_ARGS));
        return 1;
    }

    Config config = CONFIG_load(argv[1]);
    MANAGER_setConfig(config);
    NETWORK_init(config);
    MAIN_registerSignal();
    char cadena[100];
    while (1)
    {

        UTILS_printName(config);
        read(0, cadena, 100);
        MANAGER_manageCommand(cadena);
    }

    return 0;
}

void MAIN_registerSignal()
{
    signal(SIGINT, MAIN_freeMemory);
}

void MAIN_freeMemory()
{
    MANAGER_freeMemory();
}