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
#include <netdb.h>

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

        char user[256];  

        while(1){

                int n = readLine(sc, buffer, 256); 
                if (n==-1){
                        printf("Error en el servidor de arriba\n");
                        break; 
                }

                if (strcmp(buffer,"REGISTER") == 0){
                        /*REGISTER*/
                        //enviamos confirmación
                        if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}  
                        
                        //obtenemos usuario
                        if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}

                        strcpy(user,buffer);

                        //comprobar si existe
                        int existe = nodoExiste(cabeza,buffer);

                        //registramos usuario
                        if (existe == 0){
                                printf("REGISTER %s OK\n",buffer);
                                insertarEnLista(&cabeza,user);
                                strcpy(buffer,"0");

                        }

                        if (existe == 1){ //ya existe el usuario. 
                                printf("REGISTER %s FAIL\n",buffer);

                                strcpy(buffer,"1");
                        }

                        imprimirLista(cabeza);//opcional

                        //enviamos el resultado
                        if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}
                
                }
                
                if (strcmp(buffer,"UNREGISTER") == 0){ 
                    /*UNREGISTER*/
                    //enviamos confirmación
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}  
                    
                    //obtenemos usuario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                    
                    //comprobar si existe
                    int existe = nodoExiste(cabeza,buffer);

                    //eliminamos usuario
                    if (existe == 1){
                        printf("UNREGISTER %s OK\n",buffer);
                        borrarPorUsuario(&cabeza,buffer);
                        imprimirLista(cabeza);
                        strcpy(buffer,"0");

                    }

                    if (existe == 0){ //no existe el usuario 
                        printf("UNREGISTER %s FAIL\n",buffer);

                        strcpy(buffer,"1");
                    }

                    //enviamos el resultado
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}
                }

                if (strcmp(buffer,"CONNECT") == 0){                
                    /*CONNECT*/

                    char usuario[256];
                    char ip[256];
                    char mensaje[256];
                    int puerto;
                    int sd;
                    struct sockaddr_in server_addr;
                    struct hostent *hp;
                    //struct hostent *hp;

                    //enviamos confirmación
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}  
                    
                    //obtenemos usuario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                    strcpy(usuario,buffer);

                    //comprobar si existe el usuario
                    int existe = nodoExiste(cabeza,usuario);
                    
                    if (existe == 1){
                        strcpy(buffer,"0");
                    }
                    else{
                        strcpy(buffer,"1");
                        printf("CONNECT %s FAIL\n",buffer);
                    }
                    //enviamos confirmación
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}

                    //Cambiamos los valores del usuario si existe y si está conectado
                    if (existe == 1){
                        printf("CONNECT %s OK\n",usuario);

                        
                        //obtenemos la ip
                        if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                        strcpy(ip,buffer);
                        printf("IP %s : \n",ip);

                        //obtenemos el puerto
                        if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                        puerto = atoi(buffer);
                        printf("Puerto %d : \n",puerto);
                        
                        modificarEnLista (cabeza,usuario,ip,puerto,"Conectado");
                        int sock = socket(AF_INET, SOCK_STREAM, 0);

                        int n = numItemsMessage(cabeza,usuario);
                        printf("Número de mensajes pendientes: %d\n",n);
                        //leer la lista de mensajes pendientes

                        strcpy(mensaje,obtenerUltimoMensaje(cabeza,usuario));
                        printf("Mensaje: %s\n",mensaje);




                        sd = socket(AF_INET, SOCK_STREAM, 0);
                        if (sd == 1) {
                                printf("Error en socket\n");
                        }
                        bzero((char *)&server_addr, sizeof(server_addr));
                        
                        hp = gethostbyname(ip); //en ip hay un string con la ip del cliente
                        if (hp == NULL) {
                                printf("Error en gethostbyname\n");
	                }
                        
   	                memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);

                        server_addr.sin_family = AF_INET;
                        server_addr.sin_port = htons(puerto);

                        int c = connect(sock, (struct sockaddr *) &server_addr,  sizeof(server_addr));
                        if (c == -1){
                                printf("Error en connect\n");
                        }

                        

                        strcpy(buffer,"hola mi bro");

                        //iterar sobre la lista de mensajes cuyo destinatario es CONECTADO

                        
                        int a = sendMessage(sock, buffer, strlen(buffer)+1);
                        if (a == -1){
                                printf("Error en el send message de hola mi bro\n");
                        }
                        imprimirLista(cabeza);
                    }
                }

                if (strcmp(buffer,"DISCONNECT") == 0){                
                    /*DISCONNECT*/

                    char usuario[256];
                    
                    //enviamos confirmación
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}  
                    
                    //obtenemos usuario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                    strcpy(usuario,buffer);

                    //comprobar si existe el usuario
                    int existe = nodoExiste(cabeza,usuario);

                    //Cambiamos los valores del usuario si existe y si está conectado
                    if (existe == 1){
                        printf("DISCONNECT %s OK\n",usuario);
                        modificarEnLista (cabeza,usuario,"",0,"Desconectado");
                        imprimirLista(cabeza);
                        strcpy(buffer,"0");
                    }

                    if (existe == 0){ //no existe el usuario
                        printf("DISCONNECT %s FAIL\n",buffer);

                        strcpy(buffer,"1");
                    }

                    //enviamos confirmación
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}
                }


                
                if (strcmp(buffer,"SEND") == 0){                  
                    /*SEND*/

                    char remitente[256];
                    char destinatario[256];
                    char mensaje[256];
                   
                    
                    //enviamos confirmación
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;} 

                    //obtenemos usuario que envía
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                    strcpy(remitente,buffer);

                    //obtenemos el usuario destinatario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                    strcpy(destinatario,buffer);

                    //obtenemos el mensaje
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error en el servidor");break;}
                    strcpy(mensaje,buffer);

                    //comprobar si existe el usuario
                    int existe = nodoExiste(cabeza,destinatario);

                    //Metemos el mensaje al la lista del usuario si existe
                    if (existe == 1){
                        sendMessageEnLista(cabeza,destinatario,remitente,mensaje);
                        strcpy(buffer,"0");
                    } 

                    if (existe == 0){ //no existe el usuario
                        strcpy(buffer,"error");
                    }

                    //enviamos confirmación
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}   
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
} 