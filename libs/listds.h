#ifndef LISTDS_H
#define LISTDS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    char* user;
    int state;
    int socketfd;
    pthread_t thread;
    void* dedicated;
} Elementds;

typedef struct p
{
    Elementds e;
    struct p *seg;
    struct p *ant;
} Nodeds;

typedef struct
{
    Nodeds *pri;
    Nodeds *pdi;
    Nodeds *ult;
    int elements;
} Llistads;

Llistads LLISTADS_crea();
int LLISTADS_inserirDavant(Llistads *l, Elementds e);
int LLISTADS_inserirDarrere(Llistads *l, Elementds e);
Elementds LLISTADS_consulta(Llistads l);
int LLISTADS_elimina(Llistads *l);
int LLISTADS_eliminaAmbNode(Llistads *l, Nodeds *node);
int LLISTADS_avanca(Llistads *l);
int LLISTADS_retrocedeix(Llistads *l);
int LLISTADS_vesInici(Llistads *l);
int LLISTADS_vesFinal(Llistads *l);
int LLISTADS_buida(Llistads l);
int LLISTADS_getMida(Llistads l);
int LLISTADS_inici(Llistads l);
int LLISTADS_final(Llistads l);
void LLISTADS_destrueix(Llistads *l);

#endif
