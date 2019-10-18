#include "../libs/commandManager.h"
#include <stdio.h>

int MANAGER_manageCommand(const char *str1)
{
    if (UTILS_compareCaseInsensitive(EXIT, const char* str1, 0) == 0)
    {
        /* FUNCIO PER ALLIBERAR MEMORIA I SORTIR */
        freeMemory();
        return 0
    }else if (UTILS_compareCaseInsensitive(CONNECT, const char* str1, 0) == 0)
    {

        /* FUNCIO PER INICIAR CONNEXIO AMB PORT */
    }else if (UTILS_compareCaseInsensitive(SAY, const char* str1, 0) == 0)
    {
        /* code */
    }else if (UTILS_compareCaseInsensitive(BROADCAST, const char* str1, 0) == 0)
    {
        /* code */
    }else if (UTILS_compareCaseInsensitive(DOWNLOAD, const char* str1, 0) == 0)
    {
        /* code */
    }else if (UTILS_compareCaseInsensitive(SHOW, const char* str1, 0) == 0)
    {
        if (UTILS_compareCaseInsensitive(CONNECTIONS, const char* str1, sizeof(SHOW)) == 0)
        {
            /* SHOW CONNECTIONS */
        }else
        {
            /* SHOW AUDIOS */
        }
    
    }   
    
}
void MANAGER_registerSignal(){
    signal(SIGINT, freeMemory);
}

void freeMemory(){
    //Alliberar la memòria pertinent
}

