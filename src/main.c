#include "../libs/network.h"
#include "../libs/commandManager.h"
#include "../libs/client.h"
#include "../libs/io.h"

#define NUM_ARGS 2
#define ERR_INVALID_ARGS "Error, parámetros incorrectos\n"

void MAIN_registerSignal();
void MAIN_freeMemory();

int main(int argc, char **argv)
{

    if (argc != NUM_ARGS)
    { 
        // Comprovem el correcte nombre de arguments
        IO_write(1, ERR_INVALID_ARGS, strlen(ERR_INVALID_ARGS));
        return 1;
    }

    // Carreguem/Llegim la configuració
    Config config = CONFIG_load(argv[1]); 
    // Pasem config a el Manager
    MANAGER_setConfig(config);            
    CLIENT_initClient();
    NETWORK_init(config);
    // Registrem el signal de ctrl c
    MAIN_registerSignal();
    char *cadena;

    while (1)
    { 
        // Mentre no es tanqui el progama, escrivim nom de programa i llegim la comanda
        UTILS_printName(CONFIG_getUsername(config));
        cadena = UTILS_readKB();
        if (strlen(cadena) > 3)
        {
            MANAGER_manageCommand(cadena);
        }
        else
        {
            IO_write(1, COMMAND_ERROR, strlen(COMMAND_ERROR));
        }
    }

    return 0;
}

void MAIN_registerSignal()
{ 
    // En cas de ctrl+c, cridarem freeMemory
    signal(SIGINT, MAIN_freeMemory);
    signal(SIGPIPE, CLIENT_messageError);
}

void MAIN_freeMemory()
{
    CLIENT_exit();
    CLIENT_freeMemory();
    UTILS_freeMemory();
    NETWORK_close();
    exit(1);
}
