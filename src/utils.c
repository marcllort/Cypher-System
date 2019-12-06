#include "../libs/utils.h"

char *bufferKB;

// String comparator
int UTILS_compare(const char *str1, const char *str2, int size)
{
    int i;
    for (i = 0; i < size; i++)
        if (str1[i] != str2[i])
            return 1;
    return 0;
}

// String sizeOf, returns int with the size
int UTILS_sizeOf(const char *str)
{
    int j = 0;
    int i = 0;

    while (j != 1)
    {
        if (str[i] == '\0')
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
    int size1 = strlen(str1);
    int size2 = strlen(str2);
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

// String number checker, returns true (1) if it's a number
int UTILS_valid_digit(char *ip_str)
{
    int size = UTILS_sizeOf(ip_str);
    int i = 0;
    while (size < i)
    {
        if (*ip_str >= '0' && *ip_str <= '9')
        {
            ++ip_str;
        }
        else
        {
            return 0;
        }
        i++;
    }
    return 1;
}

// Function to print name of the Trinity program
int UTILS_printName(char *config)
{
    char buff[128];
    int n = sprintf(buff, "$%s: ", config);
    IO_write(1, buff, n);

    return n;
}

// Function to remove specific char from string
void UTILS_removeChar(char *p, int ch)
{
    char *ptr;

    while ((ptr = strchr(p, ch)))
        strcpy(ptr, ptr + 1);
}

int UTILS_isEmpty(const char *s)
{
    // Function to check if string is empty
    while (*s != '\0')
    {
        if (!isspace((unsigned char)*s))
            return 0;
        s++;
    }
    return 1;
}

// Funciton to read keyboard dynamically
char *UTILS_readKB()
{
    char c = '\0';
    int count = 0;

    bufferKB = (char *)malloc(sizeof(char));
    while (c != '\n')
    {
        int value = read(0, &c, sizeof(char));
        if (value < 0)
        {
            IO_write(1, "Error read", sizeof("Error read"));
        }
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

int UTILS_fileExists(char *filename)
{
    return access(filename, F_OK);
}

int digits(long num)
{
    int digits;
    for (digits = 0; num > 0; num /= 10, digits++);
    return digits;
}

char *UTILS_md5(char *filename)
{
    int pid = fork();

    if (pid == 0)
    {
        char *data[4];
        data[0] = "sh";
        data[1] = "-c";
        data[2] = (char *)malloc(sizeof(char) * (strlen(filename) + 36 + digits(getpid())));
        sprintf(data[2], "md5sum \"%s\" | awk '{print $1}' > %d.txt", filename, getpid());
        data[3] = NULL;
        execvp(data[0], data);
    }

    waitpid(pid, NULL, 0);

    char *md5 = NULL;
    char *name = (char *)malloc(sizeof(char) * (digits(pid) + 5));
    sprintf(name, "%d.txt", pid);

    int fd = open(name, O_RDWR);

    IO_read(fd, &md5, 32);
    IO_close(fd);
    IO_deleteFile(name);

    free(name);

    return md5;
}

void UTILS_freeMemory()
{
    free(bufferKB);
}