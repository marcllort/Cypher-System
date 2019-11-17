#include "../libs/listds.h"


//Funció per crear una nova llista.
Llistads 	LLISTADS_crea () {
    Llistads  l;
    int err = 1;
    l.elements = 0;
    l.pri = (Node*) malloc (sizeof(Node));
    if (l.pri != NULL) {
        l.ult = (Node*) malloc (sizeof(Node));
        if (l.ult != NULL) {
            l.pdi = l.pri;
            l.pri->seg = l.ult;
            l.pri->ant = NULL;
            l.ult->ant = l.pri;
            l.ult->seg = NULL;
            err = 0;
        } else {
            free(l.pri);
        }
    }

    if (err == 1) {
        l.pri = NULL;
        l.ult = NULL;
        l.pdi = NULL;
    }
    return l;
}


//Funció per inserir un nou node darrere/dreta del PDI amb el contingut element.
int	LLISTADS_inserirDarrere (Llistads  * l, Elementds  e) {    
    Nodeds* n;
    if (l->pdi == l->ult) return 0;

    n = (Node*) malloc (sizeof(Node));

    if (n == NULL) return 0;

    n->e = e;
    n->seg = l->pdi->seg;
    n->ant = l->pdi;

    l->pdi->seg->ant = n;
    l->pdi->seg = n;
    l->elements++;

    return 1;
}

//Funció per inserir un nou node davant/esquerra del PDI amb el contingut Elementds.
int LLISTADS_inserirDavant (Llistads  * l, Elementds e) {
    Nodeds* n;

    if (l->pdi == l->pri) return 0;

    n = (Node*) malloc (sizeof(Node));

    if (n == NULL) return 0;

    n->e = e;
    n->seg = l->pdi;
    n->ant = l->pdi->ant;

    l->pdi->ant->seg = n;
    l->pdi->ant = n;
    l->elements++;

    return 1;
}

//Funció per consultar la Elementdssituat sobre el PDI de la nostre llista.
Elementds LLISTADS_consulta (Llistads  l) {

    Elementds e;
    if(l.pdi->ant != NULL && l.pdi->seg != NULL){
        e = l.pdi->e;
    }
    
    return e;
}

//Funció per eliminar un node de la nostre llista.
int LLISTADS_elimina (Llistads  * l) {
    Nodeds* aux;

    if (l->pdi == l->pri || l->pdi == l->ult) {
        return 0;
    }
    aux = l->pdi;

    l->pdi->ant->seg = l->pdi->seg;
    l->pdi->seg->ant = l->pdi->ant;
    l->pdi = l->pdi->seg;
    l->elements--;

    free(aux);

    return 1;
}

//Funcio per eliminar un node concret de la llista
int LLISTADS_eliminaAmbNode (Llistads  * l, Nodeds*node) {
    (*l).pdi = node;
    return LLISTADS_elimina(l);

}

//Funcio per avancar el PDI en la nostre llista.
int LLISTADS_avanca (Llistads  * l) {

    if (l->pdi->seg == NULL) {
        return 0;
    }

    l->pdi = l->pdi->seg;
    return 1;
}

//Funcio per tirar enrere una posició el PDI en la nostre llista.
int LLISTADS_retrocedeix (Llistads  * l) {
    if (l->pdi->ant == NULL) {
        return 0;
    }

    l->pdi = l->pdi->ant;
    return 1;
}

//Funció per posar el nostre PDI en primera posicio.
int LLISTADS_vesInici (Llistads  * l) {

    l->pdi = l->pri->seg;
    return 1;
}

//Funcio per posar el nostre PDI en ultima posicio.
int LLISTADS_vesFinal (Llistads  * l) {

    l->pdi = l->ult->ant;
    return 1;
}

//Funcio per comprovar si la Llistads esta buida.
int LLISTADS_buida (Llistads  l) {
  
    return l.pri->seg == l.ult;
}

//Funcio que retorna la mida de la llista
int LLISTADS_getMida (Llistads  l) {
  
    return (l.elements);
}

//Funcio per comprovar que el nostre PDI esta situat al principi.
int LLISTADS_inici (Llistads  l) {
    return l.pdi == l.pri;
}

//Funcio per comprovar que el nostre PDI esta situat al final.
int LLISTADS_final (Llistads l) {
    return l.pdi == l.ult;
}

//Funcio per destruir la nostre llista.
void LLISTADS_destrueix (Llistads * l) {
    Nodeds* aux;

    while (l->pri != NULL) {
        aux = l->pri;
        l->pri = l->pri->seg;
        free(aux);
    }

    l->pri = NULL;
    l->ult = NULL;
    l->pdi = NULL;
}