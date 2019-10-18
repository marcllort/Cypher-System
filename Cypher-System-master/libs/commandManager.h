#ifndef COMMANDMANAGER_H
#define COMMANDMANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

#define SHOW "SHOW "
#define CONNECTIONS "CONNECTIONS"
#define CONNECT "CONNECT "
#define BROADCAST "BROADCAST "
#define DOWNLOAD "DOWNLOAD "
#define EXIT "EXIT "

int MANAGER_manageCommand(const char *str1);
void MANAGER_registerSignal();

#endif 
