#include "../libs/commandManager.h"

Config config;

int MANAGER_setConfig(Config newConfig)
{
    // Emmagatzemem la configuració llegida anteriorment al main
    config = newConfig;
    return 1;
}

int MANAGER_manageCommand(char *inputString)
{
    // Funció encarregada de cridar les diferents funcionalitats del sistema i passarlis els parametres necessaris
    char *words[3];

    // Comprovem si el string esta buit o no
    if (!UTILS_isEmpty(inputString))
    {
        // Separem la comanda per espais per anar analitzantla pas a pas
        words[0] = strtok(inputString, " ");

        CLIENT_checkConnections();

        if (UTILS_compareCaseInsensitive(EXIT, words[0]) == 0)
        {
            // Alliberem memoria i sortim
            free(inputString);
            raise(SIGINT);
        }
        else if (UTILS_compareCaseInsensitive(CONNECT, words[0]) == 0)
        {
            words[1] = strtok(0, " ");
            if (words[1] && UTILS_valid_digit(words[1]))
            {
                free(inputString);
                int port = atoi(words[1]);
                IO_write(1, CONNECT_MSG, strlen(CONNECT_MSG));
                CLIENT_checkConnections();

                // Cridem a la funcio de connectarnos passant la nostra configuracio i el seu port
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

            char *user = malloc(strlen(words[1]));
            sprintf(user, "%s", words[1]);

            // Fem les comprovacions necessaries per saber que la comanda esta ben formada, sino mostrem error
            if (words[1])
            {
                words[2] = strtok(0, "\n");
                if (words[2])
                {
                    free(inputString);

                    // Comprovem que el text a enviar estigui envoltat de cometes
                    if (words[2][0] == '"' && words[2][UTILS_sizeOf(words[2]) - 1] == '"')
                    {
                        UTILS_removeChar(words[2], '"');

                        CLIENT_write(user, words[2]);
                        free(user);
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
                // Comprovem que el text a enviar estigui envoltat de cometes
                if (words[1][0] == '"' && words[1][UTILS_sizeOf(words[1]) - 1] == '"')
                {
                    UTILS_removeChar(words[1], '"');
                    CLIENT_broadcast(words[1]);
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
                    // Per descarregar, li passem el usuari i el nom del fitxer a descarregar
                    CLIENT_download(words[1], words[2]);
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
                // Muntem la comanda que caldra executar el CLIENT_checkPorts
                char *buffer = (char *)malloc(50 * sizeof(char));
                sprintf(buffer, "./show_connections_v2.sh %d %d 127.0.0.1", config.cypherStartPort, config.cypherEndPort);
                // Executem checkports
                CLIENT_checkPorts(buffer);
                free(buffer);
            }
            else
            {
                // Distingim els diferents tipus de SHOW
                if (UTILS_compareCaseInsensitive(AUDIOS, words[1]) == 0)
                {
                    words[2] = strtok(0, " ");
                    if (words[2])
                    {
                        free(inputString);
                        // Passem el usuari del que volem saber quins audios te
                        CLIENT_showAudios(words[2]);
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
    }
    else
    {
        free(inputString);
        IO_write(1, COMMAND_ERROR, strlen(COMMAND_ERROR));
    }
    return 1;
}
