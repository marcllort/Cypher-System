#include "../libs/utils.h"

int UTILS_compare(const char *str1, const char *str2, int size)
{
    int i;
    for (i = 0; i < size; i++) if (str1[i] != str2[i]) return 1;
    return 0;
}

int UTILS_compareCaseInsensitive(const char *str1, const char* str2, int index)
{
    int size = sizeof(*str1);
    for (i = index; i < size; i++) if (str1[i] != str2[i] && str1[i]!= str2[i]-33 || str1[i] == ' ') return i;
    return 0;
}