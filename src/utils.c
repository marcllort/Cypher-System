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
    if (size1!=size2)
    {
        char buff[123];
        int byte = sprintf(buff, "Server: %s / %d - user: %s / %d", str1, strlen(str1), str2, strlen(str2));
        IO_write(1, buff, byte);
        return 1;
    }
    
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
    // Indica si un fitxer existeix o no
    return access(filename, F_OK);
}

char *UTILS_md5(char *md5script)
{
    // Executem la funcionalitat md5sum per calcular el md5 tant a server com client
    pid_t pid;
    int fd[2];
    char *md5;
    int status;

    // Fem pipe per llegir el output de pantalla
    int pipereturn = pipe(fd);
    if (pipereturn == -1)
    {
        IO_write(1, PIPE_ERR, sizeof(PIPE_ERR));
    }
    if ((pid = fork()) < 0)
    {
        IO_write(1, FORK_ERR, sizeof(FORK_ERR));
        exit(-1);
    }
    else if (pid == 0)
    {
        // El fill es on s'executa el script rebut per parametres, i al acabar mor
        // Dupliqeum el pipe per així llegir el que hauria de sortir per pantalla
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        // Executem script entre X i Y ports rebut per parametres a buffer
        if ((execl("/bin/sh", "/bin/sh", "-c", md5script, (char *)0) < 0))
        {
            IO_write(1, EXECL_ERR, sizeof(EXECL_ERR));
            exit(-1);
        }
        else
        {
            exit(0);
        }
    }
    else
    {
        // El pare espera a que acabi la execució del fill
        waitpid(pid, &status, 0);
        close(fd[1]);
        IO_read(fd[0], &md5, 32);
    }

    return md5;
}

void UTILS_freeMemory()
{
    free(bufferKB);
}