#include "../libs/commandManager.h"

#define EXIT_MSG "TEST: Disconnecting Trinity...\n"
#define CONNECT_MSG "Connecting... \n"
#define CONNECT_ERROR "TEST: ERROR CONNECT, MISSING PARAMETER\n"

#define SAY_MSG "TEST: SAYING Y TO X\n"
#define SAY_ERROR_MESS "TEST: ERROR MISSING COMMAS \n"
#define SAY_ERROR "TEST: ERROR CONNECT, MISSING TEXT\n"
#define SAY_ERROR_2 "TEST: ERROR SAY, MISSING PARAMETERS\n"

#define BROADCAST_MSG "TEST: BROADCASTING Y\n"
#define BROADCAST_ERROR "TEST: ERROR BROADCAST, MISSING TEXT\n"

#define DOWNLOAD_MSG "TEST: DOWNLOADING X'S AUDIO Y\n"
#define DOWNLOAD_ERROR "TEST: ERROR DONWLOAD, MISSING AUDIO FILE\n"
#define DOWNLOAD_ERROR_2 "TEST: ERROR DOWNLOAD, MISSING PARAMETERS\n"

#define SHOW_ERROR "TEST: UNKNOWN SHOW COMMAND\n"
#define SHOW_CONNECTIONS "TEST: SHOWING CONNECTIONS\n"
#define SHOW_AUDIOS "TEST: SHOWING X'S AUDIOS\n"
#define AUDIOS_ERROR "TEST: ERROR AUDIO, MISSING USER\n"

#define COMMAND_ERROR "TEST: UNKNOWN COMMAND\n"
#define FREE_MEM "TEST: FREE MEMORY \n"

Config config;

int MANAGER_setConfig(Config newConfig)
{
    config = newConfig; // Emmagatzemem la configuració llegida anteriorment al main
    return 1;
}

int MANAGER_manageCommand(char *inputString)
{
    // Funció encarregada de cridar les diferents funcionalitats del sistema i passarlis els parametres necessaris
    char *words[3];

    // Separem la comanda per espais per anar analitzantla pas a pas
    words[0] = strtok(inputString, " ");

    if (UTILS_compareCaseInsensitive(EXIT, words[0]) == 0)
    {
        // Alliberem memoria i sortim
        MANAGER_freeMemory();
        free(inputString);
        IO_write(1, EXIT_MSG, strlen(EXIT_MSG));
        raise(SIGINT);
    }
    else if (UTILS_compareCaseInsensitive(CONNECT, words[0]) == 0)
    {
        words[1] = strtok(0, " ");
        if (words[1] && UTILS_valid_digit(words[1]))
        {
            free(inputString);
            UTILS_sizeOf(words[1]);
            int port = atoi(words[1]);
            IO_write(1, CONNECT_MSG, strlen(CONNECT_MSG));
            CLIENT_connectPort(config, port);
        }
        else
        {
            free(inputString);
            IO_write(1, CONNECT_ERROR, strlen(CONNECT_ERROR));
        }
    }
    else if (UTILS_compareCaseInsensitive(SAY, words[0]) == 0)
    {
        words[1] = strtok(0, " ");

        char user[128];
        sprintf(user, "%s", words[1]);

        if (words[1])
        {
            words[2] = strtok(0, "\n");
            if (words[2])
            {
                free(inputString);

                /*char buff[124];
                    sprintf(buff,"Ultima lletra: %c",words[2][UTILS_sizeOf(words[2]) - 1]);
                     IO_write(1, buff, strlen(buff));*/

                if (words[2][0] == '"' && words[2][UTILS_sizeOf(words[2]) - 1] == '"') // Comprovem que el text a enviar estigui envoltat de cometes
                {
                    UTILS_removeChar(words[2], '"');

                    CLIENT_write(user, words[2]);
                }
                else
                {
                    IO_write(1, SAY_ERROR_MESS, strlen(SAY_ERROR_MESS));
                }
            }
            else
            {
                free(inputString);
                IO_write(1, SAY_ERROR, strlen(SAY_ERROR));
            }
        }
        else
        {
            free(inputString);
            IO_write(1, SAY_ERROR_2, strlen(SAY_ERROR_2));
        }
    }
    else if (UTILS_compareCaseInsensitive(BROADCAST, words[0]) == 0)
    {
        words[1] = strtok(0, "\n");
        if (words[1])
        {

            if (words[1][0] == '"' && words[1][UTILS_sizeOf(words[1]) - 1] == '"') // Comprovem que el text a enviar estigui envoltat de cometes
            {
                UTILS_removeChar(words[1], '"');
                IO_write(1, words[1], strlen(words[1])); //Cal borrar
                IO_write(1, BROADCAST, strlen(BROADCAST));
            }
            else
            {
                IO_write(1, BROADCAST_ERROR, strlen(BROADCAST_ERROR));
            }
        }
        else
        {
            free(inputString);
            IO_write(1, BROADCAST_ERROR, strlen(BROADCAST_ERROR));
        }
    }
    else if (UTILS_compareCaseInsensitive(DOWNLOAD, words[0]) == 0)
    {
        words[1] = strtok(0, " ");
        if (words[1])
        {
            words[2] = strtok(0, " ");
            if (words[2])
            {
                free(inputString);
                IO_write(1, DOWNLOAD, strlen(DOWNLOAD));
            }
            else
            {
                free(inputString);
                IO_write(1, DOWNLOAD_ERROR, strlen(DOWNLOAD_ERROR));
            }
        }
        else
        {
            free(inputString);
            IO_write(1, DOWNLOAD_ERROR_2, strlen(DOWNLOAD_ERROR_2));
        }
    }
    else if (UTILS_compareCaseInsensitive(SHOW, words[0]) == 0)
    {
        words[1] = strtok(0, " ");
        if (!words[1])
        {
            free(inputString);
            IO_write(1, SHOW_ERROR, strlen(SHOW_ERROR));
        }
        else if (UTILS_compareCaseInsensitive(CONNECTIONS, words[1]) == 0)
        {
            free(inputString);
            char *buffer = (char *)malloc(50 * sizeof(char));
            sprintf(buffer, "./show_connections.sh %d %d >> output", config.cypherStartPort, config.cypherEndPort);

            CLIENT_checkPorts(buffer);
            free(buffer);
        }
        else
        {

            if (UTILS_compareCaseInsensitive(AUDIOS, words[1]) == 0)
            {
                words[2] = strtok(0, " ");
                if (words[2])
                {
                    free(inputString);
                    IO_write(1, SHOW_AUDIOS, strlen(SHOW_AUDIOS));
                }
                else
                {
                    free(inputString);
                    IO_write(1, AUDIOS_ERROR, strlen(AUDIOS_ERROR));
                }
            }
            else
            {
                free(inputString);
                IO_write(1, SHOW_ERROR, strlen(SHOW_ERROR));
            }
        }
    }
    else
    {
        free(inputString);
        IO_write(1, COMMAND_ERROR, strlen(COMMAND_ERROR));
    }

    free(inputString);

    return 1;
}
void MANAGER_freeMemory()
{
    //IO_write(1, FREE_MEM, strlen(FREE_MEM));
}
