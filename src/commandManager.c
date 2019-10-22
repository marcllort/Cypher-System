#include "../libs/commandManager.h"
#include <stdio.h>


void freeMemory(){
    //Alliberar la memòria pertinent
}

int MANAGER_manageCommand(char *str1)
{
    char** paraules;

    paraules = UTILS_str_split(str1, ' ');
    if (paraules)
    {
        printf("1\n");
        if (UTILS_compareCaseInsensitive(EXIT, paraules[0]) == 0)
        {
            /* FUNCIO PER ALLIBERAR MEMORIA I SORTIR */
            freeMemory();
            printf("EXIT CONSEGUIT\n");
            //return 0;
        }else if (UTILS_compareCaseInsensitive(CONNECT, paraules[0]) == 0)
        {
            printf("2\n");
            /* FUNCIO PER INICIAR CONNEXIO AMB PORT */
        }else if (UTILS_compareCaseInsensitive(SAY, paraules[0]) == 0)
        {
            /* code */
        }else if (UTILS_compareCaseInsensitive(BROADCAST, paraules[0]) == 0)
        {
            /* code */
        }else if (UTILS_compareCaseInsensitive(DOWNLOAD, paraules[0]) == 0)
        {
            /* code */
        }else if (UTILS_compareCaseInsensitive(SHOW, paraules[0]) == 0)
        {
            if (UTILS_compareCaseInsensitive(CONNECTIONS, paraules[1]) == 0)
            {
                /* SHOW CONNECTIONS */
            }else
            {
              if (UTILS_compareCaseInsensitive(AUDIOS, paraules[1]) == 0)
                {
                    if(paraules[2]){
                        /* SHOW AUDIOS */
                        printf("A");
                    }else{
                        printf("error");
                    }
                    
                }else{
                    //MISSATGE ERROR
                }  
            }
        
        }else{
            //MISSATGE ERROR
        }

        
        free(paraules);   
    }
    return 1;  
}
void MANAGER_registerSignal(){
    signal(SIGINT, freeMemory);
}



