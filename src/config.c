#include "../libs/config.h"

Config CONFIG_load(char *filename)
{

    // Llegim el fitxer de configuració rebut per parametres

    IO_write(1, START_TRINITY, strlen(START_TRINITY));

    Config config; // Guardem la informació en el tipus Config
    char *temp = NULL;
    config.username = NULL;
    config.audioFolder = NULL;
    config.myIP = NULL;
    config.cypherIP = NULL;

    int fd = IO_openFile(filename);

    if (fd < 0)
    {
        IO_write(1, ERR_INVALID_CONFIG_FILE, strlen(ERR_INVALID_CONFIG_FILE));
        config.state = -1;
        exit(-1); // En cas de no trobar el fitxer tanquem el programa
        return config;
    }

    IO_readUntil(fd, &(config.username), '\n');

    IO_readUntilv2(fd, &(config.audioFolder), '\n');

    IO_readUntilv2(fd, &(config.myIP), '\n');

    IO_readUntilv2(fd, &temp, '\n');
    config.myPort = (int)strtol(temp, NULL, 10);
    free(temp);

    char buff[100];
    int bytes = sprintf(buff, "PORT : %d \n", config.myPort);
    IO_write(1, buff, bytes);
    IO_readUntilv2(fd, &(config.cypherIP), '\n');

    IO_readUntilv2(fd, &temp, '\n');
    config.cypherStartPort = (int)strtol(temp, NULL, 10);
    free(temp);

    IO_readUntilv2(fd, &temp, '\n');
    config.cypherEndPort = (int)strtol(temp, NULL, 10);
    free(temp);

    IO_close(fd);

    config.state = 0;

    return config;
}

//Getters
char *CONFIG_getUsername(Config config)
{
    return config.username;
}

char *CONFIG_getAudioFolder(Config config)
{
    return config.audioFolder;
}

char *CONFIG_getMyIP(Config config)
{
    return config.myIP;
}

int CONFIG_getMyPort(Config config)
{
    return config.myPort;
}

char *CONFIG_getCypherIP(Config config)
{
    return config.cypherIP;
}

int CONFIG_getCypherStartPort(Config config)
{
    return config.cypherStartPort;
}

int CONFIG_getCypherEndPort(Config config)
{
    return config.cypherEndPort;
}

int CONFIG_getState(Config config)
{
    return config.state;
}

// Tancar/Alliberar el fitxer config
void CONFIG_close(Config *config)
{
    if (config->username != NULL)
        free(config->username);
    if (config->audioFolder != NULL)
        free(config->audioFolder);
    if (config->myIP != NULL)
        free(config->myIP);
    if (config->cypherIP != NULL)
        free(config->cypherIP);
}