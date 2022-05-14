#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXSIZE	256

int imprimirListaMessage(tmensaje *cabeza){
    printf("Resultado de la lista: \n");
    tmensaje *actual = cabeza;
    char sender[256];
    char mensaje[256];

    printf("----------------------------------\n");

    while(actual != NULL){ //Mientras cabeza no sea NULL

        strcpy(mensaje,actual->message);
        strcpy(sender,actual->user_sender);
        
        int id = actual -> id;
        

        printf("ID DEL MENSAJE:%d\n",id); //Imprimimos el usuario
        printf("MENSAJE:%s\n",mensaje);
        printf("REMITENTE:%s\n",sender);
        printf("----------------------------------\n");


                
        actual = actual->sig; //Pasamos al siguiente nodo
    }

    return(0);
}

int insertarEnLista(tpuntero *cabeza, char* user){
    tpuntero nuevo; //Creamos un nuevo nodo
    nuevo = malloc(sizeof(tnodo)); //Utilizamos malloc para reservar memoria para ese nodo
    strcpy(nuevo->user,user);
    strcpy(nuevo->ip,"0");
    nuevo->puerto = 0;
    strcpy(nuevo->estado,"Desconectado");
    
    nuevo->id_counter = 0; //el contador es 0
    nuevo->last_recv = 0; //por defecto, último mensaje recibido
    nuevo->cabeza_mensaje = NULL; //por defecto
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
        char* ip = actual -> ip;
        int port = actual -> puerto;
        
        printf("USER:%s\n",user); //Imprimimos el usuario
        printf("IP:%s\n",ip);
        printf("PUERTO:%d\n",port);
        imprimirListaMessage(actual->cabeza_mensaje);
        printf("----------------------------------\n");

                
        actual = actual->sig; //Pasamos al siguiente nodo
    }

    return(0);
}

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

int borrarPorUsuario(tpuntero *cabeza,char* user){
    //borra un nodo por su clave
    
    tpuntero actual = *cabeza;
    tpuntero prev_node;

    
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (strcmp(actual->user,user) == 0){
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
        return -1;
    
    }
    return 0;    
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


int numItemsMessage(tnodo *cabeza,char* usuario){
    tnodo *actual = cabeza;
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (strcmp(actual->user,usuario) == 0){
            //ha encontrado al usuario
            tmensaje *msg_actual = actual->cabeza_mensaje;
        
            int counter = 0;
            while(msg_actual != NULL){ //Mientras cabeza no sea NULL
                counter++;
                msg_actual = msg_actual -> sig;
            }
            
            return(counter);         
        }
        actual = actual->sig;

    }
    
    return 0;
}

int obtenerUltimoMensaje(tnodo *cabeza,char* usuario,char* mensaje_param,char* id_mensaje,char* usuario_remitente){
    //obtiene el último mensaje recibido por un usuario
    tnodo *actual = cabeza;

    while(actual != NULL){ 
        if (strcmp(actual->user,usuario) == 0){
            
            if (actual->cabeza_mensaje != NULL){ //hay algun mensaje
                tmensaje *msg_actual = actual->cabeza_mensaje;
                tmensaje *aux = actual->cabeza_mensaje;
                while (msg_actual->sig != NULL){
                    aux = msg_actual; 
                    msg_actual = msg_actual->sig;
                } 

                

                //borramos ese nodo

                //toma el último mensjae
                strcpy(mensaje_param,msg_actual->message);
                sprintf(id_mensaje,"%d",msg_actual->id);
                strcpy(usuario_remitente,msg_actual->user_sender);
                
                
                if (actual->cabeza_mensaje->sig == NULL){
                    //solo queda uno
                    free(actual->cabeza_mensaje);
                    actual->cabeza_mensaje = NULL;
                }
                

                else{ //no es el último
                    aux->sig = NULL;
                    free(msg_actual);

                }
                


                return 0;

                //mensaje = *msg_actual->message+";"+*msg_actual->user_sender;

            }
            return(-1);
        }
        actual = actual->sig;
    }
    return(-1);


    //devolver algo


}

int insertarEnListaMessage(tpuntero_mensaje *cabeza, char* remitente, char* mensaje, int id){
    tpuntero_mensaje nuevo; //Creamos un nuevo nodo
    nuevo = malloc(sizeof(tmensaje)); //Utilizamos malloc para reservar memoria para ese nodo
    strcpy(nuevo->message,mensaje);
    strcpy(nuevo->user_sender,remitente);
    nuevo->id = id;
    
    nuevo->sig = *cabeza; //Le asignamos al siguiente el valor de cabeza
    *cabeza = nuevo; //Cabeza pasa a ser el ultimo nodo agregado
    //falta indicar si la clave ya existe devolviendo -1
    
    return 0;
}

unsigned int sendMessageEnLista(tnodo *cabeza, char * destinatario, char * remitente, char * mensaje){
    //busca por un usuario y lo modifica
    tnodo *actual = cabeza;
    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (strcmp(actual->user,destinatario) == 0){  
            actual->id_counter = actual->id_counter +1;
            if(actual->id_counter == 0){ //si se desborda
                actual->id_counter = actual->id_counter +1;
            } 

            insertarEnListaMessage(&(actual->cabeza_mensaje),remitente,mensaje,actual->id_counter);

            imprimirListaMessage(actual->cabeza_mensaje);

            return actual->id_counter;
        }
        actual = actual->sig; //Cabeza avanza 1 posicion en la lista
    }
    return 0; //código de operacion 8: no encontrado
}

int modificarEnLista (tnodo *cabeza, char * user, char * ip, int port, char * estado){
    //busca por un usuario y lo modifica
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


int Conectado(tnodo *cabeza, char* destinatario){
    //esta función comprueba si remitente y destinatario están conectados a la vez. La ejecuta el remitente, por tanto, éste siempre está conectado.
    // Devuelve 2 si ambos están conectados, 1 si está solo el remitente y 0 en otro caso.
    tnodo *actual = cabeza;

    while(actual != NULL){ //Mientras cabeza no sea NULL
        if (strcmp(actual->user,destinatario) == 0){
            if (strcmp(actual->estado,"Conectado") == 0){
                //ip y puerto != 0 (destinatario conectado)
                return 1;
            }
            else{
                return 0;
            }
        }
        actual = actual -> sig;
    }
    return 0; //no existe           

}


int obtenerIpYPuerto(tnodo *cabeza,char* destinatario,char *ip_destinatario,int* puerto_destinatario){
    tnodo *actual = cabeza;

    while(actual != NULL){ //Mientras cabeza no sea NULL

        if (strcmp(actual->user,destinatario) == 0){ //busca al destinatario
            strcpy(ip_destinatario,actual->ip);
            *puerto_destinatario = actual->puerto;
        }

        
        actual = actual -> sig;

    }

    return 0;
}



//----------------------------------------------------------------




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