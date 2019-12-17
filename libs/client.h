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

#define MSG_ERR_CONN "Port: %d closed\n"
#define MSG_AVAIL_CONN "%d connections available\n"
#define TESTING "Testing...\n"
#define MSG_ERR_SOCKET "Error when creating socket\n"
#define MSG_ERR_PORT "Error! Can't connect to your own port\n"
#define MSG_CONNECTED "%d connected: %s\n"
#define UNKNOWN_CONNECTION "Couldn't send message. Unknown connection %s\n"
#define UNKNOWN_CONNECTION2 "Couldn't send message. Unknown connection\n"
#define DOWNLOADING "\nDownloading...\n"
#define FILEEXISTS "File already exists\n"
#define NOFILE "\nError, file doesn't exist\n"
#define PIPE_ERR "PIPE CREATION ERROR\n"
#define FORK_ERR "FORK CREATION ERROR\n"
#define EXECL_ERR "EXEC SCRIPT ERROR\n"
#define DISCON_SERVER_ERR "\nDidn't receive server CONOK, Server is disconnected\n"
#define FILE_DOWNLOADED "[%s] downloaded\n"
#define FILE_DOWNLOAD_KO "Corrupted file, failed download\n"

int CLIENT_checkPorts(char *buffer);
int CLIENT_sayMessage(char *user, char *message);
int CLIENT_connectPort(Config config, int connectPort);
int CLIENT_initClient(Config inConfig);
int CLIENT_write(char *user, char *message);
int CLIENT_exit();
int CLIENT_freeMemory();
void CLIENT_messageError();
int CLIENT_borraUser(int fd);
char *CLIENT_read(int fd, char delimiter);
int CLIENT_download(char *user, char *filename);
int CLIENT_showAudios(char *user);
int CLIENT_broadcast(char* message);
#endif //_CLIENT_H
