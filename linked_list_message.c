#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE	256



/*
int nodoExiste(tnodo *cabeza,char* user){
    tnodo *actual = cabeza;
  
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (strcmp(actual->user,user) == 0){
            return(1);
        }

        actual = actual -> sig;

    }

    return(0); //no existe           
}

int numItems(tnodo *cabeza){
    tnodo *actual = cabeza;
    int counter = 0;
    while(actual != NULL){ //Mientras cabeza no sea NULL
        counter++;
        actual = actual -> sig;
    }
    return(counter);         
}

int modificarEnLista (tnodo *cabeza, char * user, char * ip, int port, char * estado){
    //busca por una key y devuelve la peticion
    tnodo *actual = cabeza;
  
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (strcmp(actual->user,user) == 0){
            strcpy(actual->ip,ip);
            actual->puerto = port;
            strcpy(actual->estado,estado);
            break;
        }
        actual = actual->sig; //Cabeza avanza 1 posicion en la lista
    }

    if (actual == NULL){

        return -1; //código de operacion 8: no encontrado
    }
    return(0);
}





struct snodo buscarEnLista (tnodo *cabeza, int key){
    //busca por una key y devuelve la peticion
    tnodo *actual = cabeza;
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (actual->clave == key){
            break;
        }
        actual = actual->sig; //Cabeza avanza 1 posicion en la lista
    }
    return(*actual);
}

int modificarEnLista (tnodo *cabeza, int key, char *value1, int value2, float value3){
    //busca por una key y devuelve la peticion
    tnodo *actual = cabeza;
  
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (actual->clave == key){
            strcpy(actual->valor1,value1);
            actual->valor2 = value2;
            actual->valor3 = value3;
            break;
        }
        actual = actual->sig; //Cabeza avanza 1 posicion en la lista
    }

    if (actual == NULL){

        return -1; //código de operacion 8: no encontrado
    }
    return(0);
}
 


int nodoExiste(tnodo *cabeza,int key){
    tnodo *actual = cabeza;
  
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (actual->clave == key){
            return(1);
        }

        actual = actual -> sig;

    }

    return(0); //no existe           
}

*/