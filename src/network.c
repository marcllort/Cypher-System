#include "../libs/network.h"
#include <stdio.h>

#define MSG_CONNECTED       "Connection Cyper -%s ready.\n"
#define MSG_DISCONNECTED    "Disconnecting from %s.\n"

int NETWORK_init(Config config) {

    //if (CONFIG_getState(config) != 0) return -1;

    //coses de server: reb connexio, crea threads
    //char buff [100];
    int port = CONFIG_getMyPort(config);
    char* ip = CONFIG_getMyIP(config);
    //int bytes = sprintf(buff, "NETWORK PORT : %d IP: %s knjk\n",port ,ip);
    //write(1,buff, sizeof(buff));

    trinity = SERVER_init(ip, port);
    //SERVER_setMT(&mainServer, SERVER_threadFunc, MCG_threadISR, MCG_SIG, MCG_DS_operate, MCG_DS_threadISR, MCG_DS_SIG);

    
    if (pthread_create(SERVER_getThread(&trinity), NULL, SERVER_threadFunc, &trinity) != 0) {
        return 1;
    }
        return 0;
    
        pthread_join(*SERVER_getThread(&trinity), NULL);

}

int NETWORK_close() {

    //tancar server i threads i forks

    CONFIG_close(&config);

    return 0;
}

int NETWORK_synAck(DServer dServer, int type)
{
    Packet pw = PACKET_create(type, (int)strlen(H_CONOK), H_CONOK, 0, NULL);
    PACKET_write(pw, DSERVER_getFd(&dServer));
    PACKET_destroy(&pw);
    return 0;
}

void* MCG_DS_operate(void* dat) {

    DServer* ds = (DServer*) dat;
    Packet pr;

    while (DSERVER_getState(ds)) {

        pr = PACKET_read(ds->fd);

        switch ((int) pr.type) {

            case T_CONNECT:
                printMsg(MSG_CONNECTED, DSERVER_setName(ds, pr.data, (size_t) pr.length));
                NETWORK_synAck(*ds, T_CONNECT);
                break;

            case T_EXIT:
                printMsg(MSG_DISCONNECTED, ds->name);
                NETWORK_synAck(*ds, T_EXIT);
                DSERVER_setState(ds, 0);
                break;

            /*case T_TRANSFER:

                switch (pr.header[1]) {

                    case 'M':
                        ds->file = FILE_init();
                        FILE_setMetadata(&ds->file, NETWORK_extractMetadata(pr));
                        printf("\nReceiving file: %s\n", META_getFilename(FILE_getMetadata(ds->file)));
                        break;

                    case ']':
                        if (FILE_addContent(&ds->file, pr.data, pr.dataLength)) {
                            printProgress(&ds->file);
                            NETWORK_transfer(*ds, 0, 0);
                        } else {
                            NETWORK_transfer(*ds, 0, 1);
                        }
                        break;

                    case 'E':

                        write(1, "\n", 1);

                        if (NETWORK_verifyChecksum(ds, pr) == 0) {
                            printf("File Received Successfully\n");
                            NETWORK_transfer(*ds, 1, 0);
                        } else {
                            printf("File Checksum doesn't match\n");
                            NETWORK_transfer(*ds, 1, 1);
                        }

                        FILE_destroy(&ds->file);

                        break;

                    default:
                        break;
                }

                break;*/

            default:
                printf("\nConnection Lost\n");
                printf("Type: %d\n", (int)pr.type);
                printMsg(MSG_DISCONNECTED, ds->name);
                DSERVER_setState(ds, 0);
                //FILE_destroy(&ds->file);
                break;
        }
        PACKET_destroy(&pr);

    }
    return NULL;
}

void* MCT_DS_operate(void* dat) {

    DServer* ds = (DServer*) dat;

    size_t size = 0;
    char* msg = NULL;

    while (DSERVER_getState(ds)) {

        ssize_t x;
        if((x = read(ds->fd, &size, sizeof(size_t))) <= 0) {
            printf("X: %zi\n", x);
            break;
        } else {
            msg = (char*)malloc(sizeof(char) * size + 1);
            msg[size] = '\0';
            read(ds->fd, msg, size);

            IO_write(1, CLIENT_SAYS, strlen(CLIENT_SAYS));
            IO_write(1, msg, strlen(msg));
            IO_write(1, "\n", sizeof(char));

            if(strcmp(msg, KEYPHRASE) == 0) {
                free(msg);
                break;
            }
            free(msg);
            size = 0;
        }
    }
    return NULL;
}

void MCG_threadISR(int sig) {
    if (sig == MCG_SIG) {
        IO_write(1, MCG_EXIT, strlen(MCG_EXIT));
        pthread_exit(0);
    }
}

void MCT_threadISR(int sig) {
    if (sig == MCT_SIG) {
        IO_write(1, MCT_EXIT, strlen(MCT_EXIT));
        pthread_exit(0);
    }
}

void MCG_DS_threadISR(int sig) {
    if (sig == MCG_DS_SIG) {
        IO_write(1, MCG_DS_EXIT, strlen(MCG_DS_EXIT));
        pthread_exit(0);
    }
}

void MCT_DS_threadISR(int sig) {
    if (sig == MCT_DS_SIG) {
        IO_write(1, MCT_DS_EXIT, strlen(MCT_DS_EXIT));
        pthread_exit(0);
    }
}

void printMsg(char *msg, char *name)
{
    char* data;

    if (name != NULL) {
        if ((data = (char*) malloc(sizeof(char) * (strlen(name) + strlen(msg)))) != NULL) {
            sprintf(data, msg, name);
            IO_write(1, data, strlen(data));
            free(data);
        }
    }
}