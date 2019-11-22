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
    char **words;

    //Separem la comanda per espais per anar analitzantla pas a pas
    words = UTILS_str_split(inputString, ' ');

    if (words)
    {
        if (UTILS_compareCaseInsensitive(EXIT, words[0]) == 0)
        {
            // Alliberem memoria i sortim
            MANAGER_freeMemory();

            write(1, EXIT_MSG, strlen(EXIT_MSG));
            //raise(SIGINT);
        }
        else if (UTILS_compareCaseInsensitive(CONNECT, words[0]) == 0)
        {
            if (words[1] && UTILS_valid_digit(words[1]))
            {
                UTILS_sizeOf(words[1]);
                int port = atoi(words[1]);

                write(1, CONNECT_MSG, strlen(CONNECT_MSG));

                CLIENT_connectPort(config, port);
            }
            else
            {
                write(1, CONNECT_ERROR, strlen(CONNECT_ERROR));
            }
        }
        else if (UTILS_compareCaseInsensitive(SAY, words[0]) == 0)
        {
            if (words[1])
            {
                if (words[2])
                {
                    if (words[2][0] == '"' && words[2][UTILS_sizeOf(words[2]) - 2] == '"')
                    {
                        UTILS_removeChar(words[2], '"');

                        //write(1, SAY_MSG, strlen(SAY_MSG));

                        char buff[128];
                        int bytes = sprintf(buff, "%s \n", words[2]);
                        write(1, buff, bytes);

                        CLIENT_write(words[1], words[2]);
                    }
                    else
                    {
                        write(1, SAY_ERROR_MESS, strlen(SAY_ERROR_MESS));
                    }
                }
                else
                {
                    write(1, SAY_ERROR, strlen(SAY_ERROR));
                }
            }
            else
            {
                write(1, SAY_ERROR_2, strlen(SAY_ERROR_2));
            }
        }
        else if (UTILS_compareCaseInsensitive(BROADCAST, words[0]) == 0)
        {
            if (words[1])
            {
                write(1, BROADCAST, strlen(BROADCAST));
            }
            else
            {
                write(1, BROADCAST_ERROR, strlen(BROADCAST_ERROR));
            }
        }
        else if (UTILS_compareCaseInsensitive(DOWNLOAD, words[0]) == 0)
        {
            if (words[1])
            {
                if (words[2])
                {
                    write(1, DOWNLOAD, strlen(DOWNLOAD));
                }
                else
                {
                    write(1, DOWNLOAD_ERROR, strlen(DOWNLOAD_ERROR));
                }
            }
            else
            {
                write(1, DOWNLOAD_ERROR_2, strlen(DOWNLOAD_ERROR_2));
            }
        }
        else if (UTILS_compareCaseInsensitive(SHOW, words[0]) == 0)
        {
            if (!words[1])
            {
                write(1, SHOW_ERROR, strlen(SHOW_ERROR));
            }
            else if (UTILS_compareCaseInsensitive(CONNECTIONS, words[1]) == 0)
            {
                //write(1, SHOW_CONNECTIONS, strlen(SHOW_CONNECTIONS));
                char *buffer = (char *)malloc(50 * sizeof(char));

                sprintf(buffer, "./show_connections.sh %d %d >> output", config.cypherStartPort, config.cypherEndPort);

                //write(1, buffer, size);
                CLIENT_checkPorts(buffer);
                free(buffer);
            }
            else
            {

                if (UTILS_compareCaseInsensitive(AUDIOS, words[1]) == 0)
                {
                    if (words[2])
                    {
                        write(1, SHOW_AUDIOS, strlen(SHOW_AUDIOS));
                    }
                    else
                    {
                        write(1, AUDIOS_ERROR, strlen(AUDIOS_ERROR));
                    }
                }
                else
                {
                    write(1, SHOW_ERROR, strlen(SHOW_ERROR));
                }
            }
        }
        else
        {
            write(1, COMMAND_ERROR, strlen(COMMAND_ERROR));
        }
    }
    free(words);

    return 1;
}
void MANAGER_freeMemory()
{
    //write(1, FREE_MEM, strlen(FREE_MEM));
}
