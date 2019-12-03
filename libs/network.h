#ifndef _NETWORK_H
#define _NETWORK_H

#include "config.h"
#include "utils.h"
#include "server.h"
#include "packet.h"

#define NET_AVAILABLE_CONN "%d connections available\n"
#define NET_CONNECTING "Connecting...\n"
#define NET_CONNECTED "%d connected: %s\n"
#define NET_AUDIOS "[%s] Audios:\n"
#define NET_DOWNLOADED "[%s] %s downloaded\n"

Config config;
Server trinity;

int NETWORK_init(Config config);
int NETWORK_close();

#endif
