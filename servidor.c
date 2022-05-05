#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "lines.h"
#include "linked-list.h"
#include <pthread.h>
#include <stdbool.h>

#define NUM_THREADS 10
pthread_mutex_t m;
pthread_cond_t c;
int busy;

tpuntero cabeza;

void tratar_peticion (void *s){
        char buffer[256];
        pthread_mutex_lock(&m);
        int sc = (* (int *)s);
        busy = false;
        pthread_cond_signal(&c);
        pthread_mutex_unlock(&m);

        char ip[256];
        char p[256];
        

        //borrarLista(&cabeza); //inicializamos lista

        //recibimos ip del cliente
        //ip 
        int n = readLine(sc, buffer, 256); 
        if (n==-1){
                printf("Error en el servidor de arriba\n");
                }

        strcpy(ip,buffer); //copia la ip al buffer

        //recibimos puerto del cliente
        //puerto
        
        n = readLine(sc, buffer, 256); 
        if (n==-1){
                printf("Error en el servidor de arriba\n");
                }

        strcpy(p,buffer); //copia la port al buffer
        

        printf("IP: %s\n",ip);
        printf("Port: %s\n",p);

        

        





        

        while(1){

                int n = readLine(sc, buffer, 256); 
                if (n==-1){
                        printf("Error en el servidor de arriba\n");
                        break; 
                }

                //printf("Buffer recibido: %s\n",buffer);

                
                if (strcmp(buffer,"Registro") == 0){
                
                    /*REGISTER*/

                    //enviamos confirmación
                    strcpy(buffer,"Registro");
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}  
                    
                    //obtenemos usuario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                    printf("Vamos a registrar al usuario: %s\n",buffer);


                    //comprobar si existe
                    int existe = nodoExiste(cabeza,buffer);
                    printf("Existe: %d\n",existe);
                    //registramos usuario
                    if (existe == 0){
                        printf("No existe el usuario %s\n",buffer);
                        insertarEnLista(&cabeza,buffer);
                        strcpy(buffer,"0");

                    }

                    if (existe == 1){ //ya existe el usuario. 
                        printf("Ya existe el usuario %s\n",buffer);

                        strcpy(buffer,"1");
                    }

                    imprimirLista(cabeza);//opcional

                    printf("Numero de ítems: %d\n",numItems(cabeza));

                     //enviamos confirmación
                    printf("Vamos a enviar la confirmación: %s\n",buffer);
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}  
                    printf("Enviado con éxito\n");
                
                }
                
                if (strcmp(buffer,"Unregister") == 0){
                
                    /*UNREGISTER*/

                    //enviamos confirmación
                    strcpy(buffer,"Unregister");
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}  
                    
                    //obtenemos usuario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                    printf("Vamos a borrar al usuario: %s\n",buffer);


                    //comprobar si existe
                    int existe = nodoExiste(cabeza,buffer);
                    printf("Existe: %d\n",existe);
                    //registramos usuario
                    if (existe == 1){
                        printf("Existe el usuario %s. Lo borramos\n",buffer);
                        borrarPorUsuario(&cabeza,buffer);
                        imprimirLista(cabeza);
                        strcpy(buffer,"0");

                    }

                    if (existe == 0){ //ya existe el usuario. 
                        printf("No existe el usuario %s\n",buffer);

                        strcpy(buffer,"1");
                    }


                    printf("Numero de ítems: %d\n",numItems(cabeza));

                     //enviamos confirmación
                    printf("Vamos a enviar la confirmación: %s\n",buffer);
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}  
                    printf("Enviado con éxito\n");
                }

                if (strcmp(buffer,"EXIT") == 0){
                        break;
                }
        }
        close(sc);
        pthread_exit(NULL);
}
int main(int argc, char *argv[]){
        struct sockaddr_in server_addr,  client_addr;
	socklen_t size;
        int sd, sc;
        int val;
        pthread_attr_t attr;
	pthread_t thid;

        pthread_mutex_init(&m,NULL);
        pthread_cond_init(&c,NULL);
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

        //char op;
	int err;
        //char buffer[256];

        if ((sd =  socket(AF_INET, SOCK_STREAM, 0))<0){
                printf ("SERVER: Error en el socket");
                return (0);
        }
        val = 1;
        setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

	bzero((char *)&server_addr, sizeof(server_addr));
    	server_addr.sin_family      = AF_INET;
    	server_addr.sin_addr.s_addr = INADDR_ANY;
    	server_addr.sin_port        = htons(4200);

    	err = bind(sd, (const struct sockaddr *)&server_addr,
			sizeof(server_addr));
	if (err == -1) {
		printf("Error en bind\n");
		return -1;
	}

    	err = listen(sd, SOMAXCONN);
	if (err == -1) {
		printf("Error en listen\n");
		return -1;
	}

    	size = sizeof(client_addr);

     	while (1){
    		printf("esperando conexion\n");
    		sc = accept(sd, (struct sockaddr *)&client_addr, (socklen_t *)&size);
                pthread_create(&thid, &attr, (void *)tratar_peticion, (void *)&sc);

                //esperar a que el hijo copie el descriptor

                pthread_mutex_lock(&m);
                while(busy == true){
                        pthread_cond_wait(&c,&m);
                }
                busy = true;
                pthread_mutex_unlock(&m);
                
		if (sc == -1) {
			printf("Error en accept\n");
			return -1;
		}

                

  		                     // cierra la conexión (sc)
	}
	close (sd);

     	return(0);
} /*fin main */