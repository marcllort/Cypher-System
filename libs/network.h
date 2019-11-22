#ifndef _NETWORK_H
#define _NETWORK_H

#include "config.h"
#include "utils.h"
#include "server.h"
#include "packet.h"

#define MCG_SIG     SIGUSR1
#define MCG_DS_SIG  SIGCONT
#define MCT_SIG     SIGUSR2
#define MCT_DS_SIG  SIGPROF

#define MCG_EXIT    "[McGruder] -> Exit\n"
#define MCG_DS_EXIT "[McGruder - DS] -> Exit\n"
#define MCT_EXIT    "[McTavish] -> Exit\n"
#define MCT_DS_EXIT "[McTavish - Ds] -> Exit\n"


#define NET_TESTING    "Testing...\n"
#define NET_AVAILABLE_CONN    "%d connections available\n"
#define NET_CONNECTING    "Connecting...\n"
#define NET_CONNECTED    "%d connected: %s\n"
#define NET_AUDIOS    "[%s] Audios:\n"
#define NET_DOWNLOADED    "[%s] %s downloaded\n"
#define NET_DISCONNECT    "Disconnecting Trinity...\n"

Config config;
Server trinity;

int NETWORK_init(Config config);
int NETWORK_close();

void* MCG_DS_operate(void* data);
void* MCT_DS_operate(void* data);

void MCG_threadISR(int sig);
void MCT_threadISR(int sig);

void MCG_DS_threadISR(int sig);
void MCT_DS_threadISR(int sig);

void printMsg(char *msg, char *name);

#endif
