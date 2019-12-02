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
#include <sys/types.h>
#include <sys/wait.h>

#include "config.h"
#include "utils.h"
#include "list.h"
#include "packet.h"

#define MSG_ERR_CONN "Puerto %d Cerrado\n"
#define MSG_AVAIL_CONN "%d connections available\n"
#define TESTING "Testing...\n"
#define MSG_ERR_SOCKET "Error durante la creacion del socket.\n"
#define MSG_ERR_PORT "Error! No puedes connectarte a tu propio puerto.\n"
#define MSG_CONNECTED "%d connected: %s\n"
#define UNKNOWN_CONNECTION "Couldn't send message. Unknown connection %s\n"
#define UNKNOWN_CONNECTION2 "Couldn't send message. Unknown connection\n"


int CLIENT_runScript(char *buffer);
int CLIENT_checkPorts(char *buffer, Config inConfig);
int CLIENT_sayMessage(char *user, char *message);
int CLIENT_connectPort(Config config, int connectPort);
int CLIENT_initClient();
char *CLIENT_get_message(int fd, char delimiter);
int CLIENT_write(char *user, char *message);
int CLIENT_exit();
int CLIENT_freeMemory();
void CLIENT_messageError();
int CLIENT_borraUser(int fd);
char *CLIENT_read(int fd, char delimiter);
int CLIENT_showAudios(char *user);
#endif //_CLIENT_H
