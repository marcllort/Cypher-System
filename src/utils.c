#include "../libs/utils.h"

// String comparator
int UTILS_compare(const char *str1, const char *str2, int size)
{
    int i;
    for (i = 0; i < size; i++)
        if (str1[i] != str2[i])
            return 1;
    return 0;
}

// String sizeOf
int UTILS_sizeOf(const char *str)
{
    int j = 0;
    int i = 0;

    while (j != 1)
    {
        if (str[i] == '\0' || str[i] == ' ')
        {
            j = 1;
        }
        i++;
    }
    i--;
    return i;
}

// String comparator, case Insensitive
int UTILS_compareCaseInsensitive(const char *str1, const char *str2)
{
    int size1 = UTILS_sizeOf(str1);
    int size2 = UTILS_sizeOf(str2);
    int size = (size1 < size2) ? size1 : size2;
    int i;
    size1 = 0;
    for (i = 0; i < size; i++)
    {
        if (str1[i] != str2[i] && str1[i] != str2[i] - 32)
        {

            size1 = 1;
        }
    }
    return size1;
}

// String splitter by delimiter
char **UTILS_str_split(char *a_str, const char a_delim)
{
    char **result = 0;
    size_t count = 0;
    char *tmp = a_str;
    char *last_comma = 0;
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

    result = malloc(sizeof(char *) * count);

    if (result)
    {
        size_t idx = 0;
        char *token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        free(token);
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

// String number checker, returns true (1) if it's a number
int UTILS_valid_digit(char *ip_str)
{
    int size = UTILS_sizeOf(ip_str);
    int i = 0;
    while (size < i)
    {
        printf("CARACTER: %c\n", *ip_str);
        if (*ip_str >= '0' && *ip_str <= '9')
        {
            ++ip_str;
        }
        else
        {
            printf("no va:  %c\n", *ip_str);
            return 0;
        }
        i++;
    }
    return 1;
}

// String IP checker, returns true (1) if it's a valid IP
int UTILS_ip_checker(char *ip_str)
{
    int num, dots = 0;
    char *ptr;

    if (ip_str == NULL)
    {
        return 0;
    }

    ptr = strtok(ip_str, IP_DELIM); //Split String by delimiter

    if (ptr == NULL)
    {
        return 0;
    }

    while (ptr)
    {

        // Check if is a valid digit
        if (!UTILS_valid_digit(ptr))
        {
            return 0;
        }
        // Transform to number
        num = atoi(ptr);

        // Check if it's between the valid values
        if (num >= 0 && num <= 255)
        {
            ptr = strtok(NULL, IP_DELIM); // By passing null, you get next part of the string in the ptr
            if (ptr != NULL)
            {
                ++dots;
            }
        }
        else
        {
            return 0;
        }
    }

    // Check number of dots found while parsing, must be 3 to be valid
    if (dots != 3)
    {
        return 0;
    }
    return 1;
}

int UTILS_printName(Config config)
{

    char buff[128];
    int n = sprintf(buff, "$%s: ", CONFIG_getUsername(config));
    write(1, buff, n);

    return n;
}

void UTILS_removeChar(char *p, int ch)
{
    char *ptr;

    while (ptr = strchr(p, ch))
        strcpy(ptr, ptr + 1);

    return p;
}

char *UTILS_readKB()
{
    char c = '\0';
    int count = 0;

    char *bufferKB = (char *)malloc(sizeof(char));
    while (c != '\n')
    {
        read(0, &c, sizeof(char));
        if (c != '\n')
        {
            bufferKB[count] = c;
            bufferKB = (char *)realloc(bufferKB, sizeof(char) * (count + 2));
        }
        count++;
    }
    bufferKB[count - 1] = '\0';
    return bufferKB;
}