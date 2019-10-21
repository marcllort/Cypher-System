#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>

#include "utils.h"

#define SHOW "SHOW"
#define CONNECTIONS "CONNECTIONS"
#define CONNECT "CONNECT"
#define BROADCAST "BROADCAST"
#define DOWNLOAD "DOWNLOAD"
#define EXIT "EXIT"
#define SAY "SAY"

int MANAGER_manageCommand(char *str1);
void MANAGER_registerSignal();

#endif 
