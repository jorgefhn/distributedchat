#include "mensaje.h"

#define MAXSIZE 256

typedef struct snodo{ //snodo es el nombre de la estructura
    char user[MAXSIZE];
    char estado[MAXSIZE];
    char ip[MAXSIZE];
    int puerto;
    int last_recv;
    tpuntero_mensaje cabeza;


    
    struct snodo *sig; //El puntero siguiente para recorrer la lista enlazada
}tnodo; //tnodo es el tipo de dato para declarar la estructura
 
typedef tnodo *tpuntero; //Puntero al tipo de dato tnodo para no utilizar punteros de punteros