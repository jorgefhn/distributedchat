#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE	256



int insertarEnLista (tpuntero *cabeza, char* user){
    tpuntero nuevo; //Creamos un nuevo nodo
    nuevo = malloc(sizeof(tnodo)); //Utilizamos malloc para reservar memoria para ese nodo
    strcpy(nuevo->user,user);

    nuevo->sig = *cabeza; //Le asignamos al siguiente el valor de cabeza
    *cabeza = nuevo; //Cabeza pasa a ser el ultimo nodo agregado
    //falta indicar si la clave ya existe devolviendo -1
    
    return 0;
}



int borrarLista(tpuntero *cabeza){ 
    tpuntero actual; //Puntero auxiliar para eliminar correctamente la lista
  
    while(*cabeza != NULL){ //Mientras cabeza no sea NULL
        actual = *cabeza; //Actual toma el valor de cabeza
        *cabeza = (*cabeza)->sig; //Cabeza avanza 1 posicion en la lista
        free(actual); //Se libera la memoria de la posicion de Actual (el primer nodo), y cabeza queda apuntando al que ahora es el primero
    }

    return(0);
}


int imprimirLista(tnodo *cabeza){
    printf("Resultado de la lista: \n");
    tnodo *actual = cabeza;
    printf("----------------------------------\n");

    while(actual != NULL){ //Mientras cabeza no sea NULL
        char* user = actual -> user;
        

        printf("USER:%s\n",user); //Imprimimos el usuario
        printf("----------------------------------\n");
                
        actual = actual->sig; //Pasamos al siguiente nodo
    }

    return(0);
}

int nodoExiste(tnodo *cabeza,char* user){
    tnodo *actual = cabeza;
  
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (strcpy(actual->user,user) != 0){
            return(1);
        }

        actual = actual -> sig;

    }

    return(0); //no existe           
}



/*



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
 
int numItems(tnodo *cabeza){
    tnodo *actual = cabeza;
    int counter = 0;
    while(actual != NULL){ //Mientras cabeza no sea NULL
        counter++;
        actual = actual -> sig;
    }
    return(counter);         
}
int borrarPorClave(tpuntero *cabeza,int key){
    //borra un nodo por su clave
    
    tpuntero actual = *cabeza;
    tpuntero prev_node;

    
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (actual->clave == key){
            if (actual == *cabeza){
                *cabeza = (*cabeza)->sig;
                free(actual);  
            }
            else{
                prev_node->sig = actual->sig;
                free(actual);
            }
        
            break;
        }
        prev_node = actual;
        actual = actual -> sig;
        }
    if(actual == NULL){
        printf("hola");
        return -1;
    
    }
    return 0;    
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