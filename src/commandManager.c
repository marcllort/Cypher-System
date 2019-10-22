#include "../libs/commandManager.h"



int MANAGER_manageCommand(char *inputString)
{
    char **words;

    words = UTILS_str_split(inputString, ' ');
    if (words)
    {

        if (UTILS_compareCaseInsensitive(EXIT, words[0]) == 0)
        {
            /* FUNCIO PER ALLIBERAR MEMORIA I SORTIR */
            MANAGER_freeMemory();
            printf("TEST: Disconnecting Trinity...\n");
            raise(SIGINT);
            //return 0;
        }
        else if (UTILS_compareCaseInsensitive(CONNECT, words[0]) == 0)
        {
            //words[1]="1443";                                         //Si descomentes aixo, si que funciona correctament
            if (words[1] && UTILS_valid_digit(words[1]))        //Problema aqui, la funcio esta funcionanrt be si li paso "4567", pero amb el valor del teu split falla nose perq, crec q deu tenir algun espai o potrser el \0, nose
            {                                                   // Mai entra a la funcio perq sempre retorna 0
                printf("TEST: CONNECTING TO X\n");
            }
            else
            {
                printf("TEST: ERROR CONNECT, MISSING PARAMETER\n");
            }
        }
        else if (UTILS_compareCaseInsensitive(SAY, words[0]) == 0)
        {
            if (words[1])
            {
                if (words[2])
                {
                    printf("TEST: SAYING Y TO X\n");
                }
                else
                {
                    printf("TEST: ERROR CONNECT, MISSING TEXT\n");
                }
            }
            else
            {
                printf("TEST: ERROR SAY, MISSING PARAMETERS\n");
            }
        }
        else if (UTILS_compareCaseInsensitive(BROADCAST, words[0]) == 0)
        {
            if (words[1])
            {
                printf("TEST: BROADCASTING Y\n");
            }
            else
            {
                printf("TEST: ERROR BROADCAST, MISSING TEXT\n");
            }
        }
        else if (UTILS_compareCaseInsensitive(DOWNLOAD, words[0]) == 0)
        {
            if (words[1])
            {
                if (words[2])
                {
                    printf("TEST: DOWNLOADING X'S AUDIO Y\n");
                }
                else
                {
                    printf("TEST: ERROR DONWLOAD, MISSING AUDIO FILE\n");
                }
            }
            else
            {
                printf("TEST: ERROR DOWNLOAD, MISSING PARAMETERS\n");
            }
        }
        else if (UTILS_compareCaseInsensitive(SHOW, words[0]) == 0)
        {
            if (!words[1])
            {
                printf("TEST: UNKNOWN SHOW COMMAND\n");
            }
            else if (UTILS_compareCaseInsensitive(CONNECTIONS, words[1]) == 0)
            {
                printf("TEST: SHOWING CONNECTIONS\n");
            }
            else
            {

                if (UTILS_compareCaseInsensitive(AUDIOS, words[1]) == 0)
                {
                    if (words[2])
                    {
                        printf("TEST: SHOWING X'S AUDIOS\n");
                    }
                    else
                    {
                        printf("TEST: ERROR AUDIO, MISSING USER\n");
                    }
                }
                else
                {
                    printf("TEST: UNKNOWN SHOW COMMAND\n");
                }
            }
        }
        else
        {
            printf("TEST: UNKNOWN COMMAND\n");
        }

        free(words);
    }
    return 1;
}
void MANAGER_freeMemory()
{
    //Alliberar la memòria pertinent

    printf("TEST: FREE MEMORY \n");
    exit(1);
}
