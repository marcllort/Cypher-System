#include "../libs/utils.h"

int UTILS_compare(const char *str1, const char *str2, int size)
{
    int i;
    for (i = 0; i < size; i++) if (str1[i] != str2[i]) return 1;
    return 0;
}
int UTILS_sizeOf(const char *str){
    int j = 0;
    int i= 0;
    
    while(j != 1){
        if( str[i] == '\0' || str[i]== ' '){
            j = 1;
        }
        i++;
    }
    i--;
    return i;
}

int UTILS_compareCaseInsensitive(const char *str1, const char* str2)
{
    int size1 = UTILS_sizeOf(str1);
    int size2 = UTILS_sizeOf(str2);
    int size = (size1 < size2) ? size1 : size2; 
    int i;  
    size1=0;
    for (i = 0; i < size; i++) {
        if (str1[i] != str2[i] && str1[i]!= str2[i]-32){
            
            size1= 1;
        }
    }
    return size1;

}

char** UTILS_str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}