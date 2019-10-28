#include "../libs/config.h"

Config CONFIG_load(char* filename) {

    write(1, START_TRINITY, strlen(START_TRINITY));

    Config config;
    char *temp = NULL;
    config.username = NULL;
    config.audioFolder = NULL;
    config.myIP = NULL;
    config.cypherIP = NULL;

    int fd = IO_openFile(filename);

    if (fd < 0) {
        write(1, ERR_INVALID_CONFIG_FILE, strlen(ERR_INVALID_CONFIG_FILE));
        config.state = -1;
        return config;
    }

    IO_readUntil(fd, &(config.username), '\n');
    
    
    IO_readUntil(fd, &(config.audioFolder), '\n');
    
    IO_readUntil(fd, &(config.myIP), '\n');

    IO_readUntil(fd, &temp, '\n');
    config.myPort = (int) strtol(temp, NULL, 10);
    free(temp);

    IO_readUntil(fd, &(config.cypherIP), '\n');

    IO_readUntil(fd, &temp, '\n');
    config.cypherStartPort = (int) strtol(temp, NULL, 10);
    free(temp);

    IO_readUntil(fd, &temp, '\n');
    config.cypherEndPort = (int) strtol(temp, NULL, 10);
    free(temp);

    IO_close(fd);

    config.state = 0;

    return config;
}

char* CONFIG_getUsername(Config config) {
    return config.username;
}

char* CONFIG_getAudioFolder(Config config) {
    return config.audioFolder;
}

char* CONFIG_getMyIP(Config config) {
    return config.myIP;
}

int CONFIG_getMyPort(Config config) {
    return config.myPort;
}

char* CONFIG_getCypherIP(Config config) {
    return config.cypherIP;
}

int CONFIG_getCypherStartPort(Config config) {
    return config.cypherStartPort;
}

int CONFIG_getCypherEndPort(Config config) {
    return config.cypherEndPort;
}

int CONFIG_getState(Config config) {
    return config.state;
}

void CONFIG_close(Config *config) {
    if (config->username != NULL) free(config->username);
    if (config->audioFolder != NULL) free(config->audioFolder);
    if (config->myIP != NULL) free(config->myIP);
    if (config->cypherIP != NULL) free(config->cypherIP);
}