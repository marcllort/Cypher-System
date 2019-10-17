#include "../libs/utils.h"

int UTILS_compare(const char *str1, const char *str2, int size)
{
    int i;
    for (i = 0; i < size; i++) if (str1[i] != str2[i]) return 1;
    return 0;
}