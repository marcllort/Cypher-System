#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include "config.h"
#include "utils.h"


#define MSG_ERR_CONN "Puerto %d Cerrado\n"
#define MSG_AVAIL_CONN "%d connections available\n"
#define TESTING "Testing...\n"
#define MSG_ERR_SOCKET "Error durante la creacion del socket.\n"


int CLIENT_checkPorts(Config config);


#endif //_CLIENT_H
