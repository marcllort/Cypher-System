#ifndef CONFIG_H
#define CONFIG_H

#include "io.h"

#define START_TRINITY "Starting Trinity...\n\n"
#define ERR_INVALID_CONFIG_FILE "Error: Invalid Config File\n"


typedef struct {
    char *username;
    char *audioFolder;
    char *myIP;
    int myPort;
    char *cypherIP;
    int cypherStartPort;
    int cypherEndPort;
    int state;
}Config;


Config CONFIG_load(char* filename);

char* CONFIG_getUsername(Config config);
char* CONFIG_getAudioFolder(Config config);
char* CONFIG_getMyIP(Config config);
int CONFIG_getMyPort(Config config);
char* CONFIG_getCypherIP(Config config);
int CONFIG_getCypherStartPort(Config config);
int CONFIG_getCypherEndPort(Config config);
int CONFIG_getState(Config config);

void CONFIG_close(Config *config);

#endif