#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include "utils.h"
#include "config.h"
#include "client.h"
#include "packet.h"

#define CONNECT_MSG "Connecting... \n"
#define CONNECT_ERROR "CONNECT error, missing parameters\n"
#define SAY_ERROR_MESS "ERROR MISSING COMMAS \n"
#define SAY_ERROR "ERROR CONNECT, MISSING TEXT\n"
#define SAY_ERROR_2 "ERROR SAY, MISSING PARAMETERS\n"
#define BROADCAST_ERROR "ERROR BROADCAST, MISSING TEXT\n"
#define DOWNLOAD_MSG "TEST: DOWNLOADING X'S AUDIO Y\n"
#define DOWNLOAD_ERROR "TEST: ERROR DONWLOAD, MISSING AUDIO FILE\n"
#define DOWNLOAD_ERROR_2 "TEST: ERROR DOWNLOAD, MISSING PARAMETERS\n"
#define SHOW_ERROR "UNKNOWN SHOW COMMAND\n"
#define SHOW_AUDIOS "TEST: SHOWING X'S AUDIOS\n"
#define AUDIOS_ERROR "TEST: ERROR AUDIO, MISSING USER\n"
#define COMMAND_ERROR "UNKNOWN COMMAND\n"
#define SHOW "SHOW"
#define CONNECTIONS "CONNECTIONS"
#define AUDIOS "AUDIOS"
#define CONNECT "CONNECT"
#define BROADCAST "BROADCAST"
#define DOWNLOAD "DOWNLOAD"
#define EXIT "EXIT"
#define SAY "SAY"

int MANAGER_setConfig(Config newConfig);
int MANAGER_manageCommand(char *str1);

#endif
