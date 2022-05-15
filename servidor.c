#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
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

int crearsocket(char * ip, int puerto){
        struct sockaddr_in server_addr;
        struct hostent *hp;
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == 1) {
                printf("Error en socket\n");
        }
        bzero((char *)&server_addr, sizeof(server_addr));
        hp = gethostbyname(ip); //en ip hay un string con la ip del cliente
        if (hp == NULL) {
                printf("Error en socket\n");
        }
        memcpy (&(server_addr.sin_addr), hp->h_addr, hp->h_length);
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(puerto);
        int c = connect(sock, (struct sockaddr *) &server_addr,  sizeof(server_addr));
        if (c == -1){
                printf("Error en socket\n");
        }
        return sock;
}

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
                        printf("Error\n");
                        break; 
                }

                if (strcmp(buffer,"REGISTER") == 0){
                        /*REGISTER*/

                        //obtenemos usuario
                        if ((readLine(sc, buffer, 256)==-1)){printf("Error\n");break;}

                        strcpy(user,buffer);

                        //comprobar si existe
                        int existe = nodoExiste(cabeza,buffer);

                        //si no existe registramos usuario
                        if (existe == 0){
                                printf("s> REGISTER %s OK\n",buffer);
                                insertarEnLista(&cabeza,user);
                                strcpy(buffer,"0");

                        }

                        if (existe == 1){ //ya existe el usuario. 
                                printf("s> REGISTER %s FAIL\n",buffer);

                                strcpy(buffer,"1");
                        }

                        //enviamos el resultado
                        strcat(buffer, "\0");
                        if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}
                
                }
                
                if (strcmp(buffer,"UNREGISTER") == 0){ 
                    /*UNREGISTER*/  
                    
                    //obtenemos usuario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error\n");break;}
                    
                    //comprobar si existe
                    int existe = nodoExiste(cabeza,buffer);

                    //eliminamos usuario si existe
                    if (existe == 1){
                        printf("s> UNREGISTER %s OK\n",buffer);
                        borrarPorUsuario(&cabeza,buffer);
                        strcpy(buffer,"0");

                    }

                    if (existe == 0){ //no existe el usuario 
                        printf("s> UNREGISTER %s FAIL\n",buffer);

                        strcpy(buffer,"1");
                    }

                    //enviamos el resultado
                    strcat(buffer, "\0");
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}
                }

                if (strcmp(buffer,"CONNECT") == 0){                
                    /*CONNECT*/

                    char usuario[256];
                    char ip[256];
                    char ip_remitente[256];
                    char mensaje[1024];
                    char usuario_remitente[256];
                    char id_mensaje[256];
                    int puerto;
                    int puerto_remitente;
                    
                    //obtenemos usuario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error\n");break;}
                    strcpy(usuario,buffer);

                    //comprobar si existe el usuario
                    int existe = nodoExiste(cabeza,usuario);
                    
                    //si existe
                    if (existe == 1){
                        strcpy(buffer,"0");
                    }
                    //si no existe
                    else{
                        strcpy(buffer,"1");
                        printf("s> CONNECT %s FAIL\n",buffer);
                    }
                    //enviamos confirmación
                    strcat(buffer, "\0");
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                    //Cambiamos los valores del usuario si existe y si está conectado
                    if (existe == 1){

                        printf("s> CONNECT %s OK\n",usuario);

                        //obtenemos la ip
                        if ((readLine(sc, buffer, 256)==-1)){printf("Error\n");break;}
                        strcpy(ip,buffer);

                        //obtenemos el puerto
                        if ((readLine(sc, buffer, 256)==-1)){printf("Error\n");break;}
                        puerto = atoi(buffer);
                        
                        //con los datos obtenidos del servidor actualizamos al usuario
                        modificarEnLista(cabeza,usuario,ip,puerto,"Conectado");

                        //obtenemos el número de mensajes que tiene el usuario en la lista de mensajes
                        int numero_mensajes = numItemsMessage(cabeza,usuario);

                        //Vamos a enviar todos los mensajes que tiene el usuario que se acaba de conectar, uno por uno
                        for(int i=0; i<numero_mensajes; i++){
                                //para cada mensaje creamos el socket que va a enviar el mensaje al hilo del cliente correspondiente
                                int sock = crearsocket(ip, puerto);
                                
                                //obtenemos el último mensaje de la lsita de mensajes (el último el el mensaje más antiguo)
                                obtenerUltimoMensaje(cabeza,usuario,mensaje,id_mensaje,usuario_remitente);

                                //cuando el usuario destinatario está desconectado y se conecta 
                                if(strcmp(usuario_remitente,"") == 0){
                                        //message ACK
                                        //enviamos la operación al hilo del cliente
                                        strcpy(buffer,"SEND MESS ACK");
                                        strcat(buffer, "\0");
                                        if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                        //enviamos el id del mensaje
                                        strcpy(buffer,mensaje);
                                        strcat(buffer, "\0");
                                        if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                        close(sock); //cerramos la conexion
                                }
                                else{
                                        //enviamos la operación al hilo del cliente
                                        strcpy(buffer,"SEND MESSAGE");
                                        strcat(buffer, "\0");
                                        if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}
                                        
                                        //enviamos el remitente
                                        strcpy(buffer,usuario_remitente);
                                        strcat(buffer, "\0");
                                        if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                        //enviamos el id del mensaje
                                        strcpy(buffer,id_mensaje);
                                        strcat(buffer, "\0");
                                        if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}
                                        
                                        //enviamos el mensaje
                                        strcpy(buffer,mensaje);
                                        strcat(buffer, "\0");
                                        if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                        //cambiamos el id del úlmtimo mensaje recibido
                                        LastRcv(cabeza, usuario, atoi(id_mensaje));

                                        close(sock); //cerramos esta conexión
                                }

                                //Una vez enviado el mensaje vamos a enviarle al remitente el ack. Para ello deberemos crear una socket que se conecte al hilo del remitente
                                //si el remitente sigue conectado solo le notificaremos, de lo contrario almacenaremos un mensaje en su lista de mensajes
                                int conectado = Conectado(cabeza, usuario_remitente);
                                
                                if (conectado == 1){
                                        //Sacamos la ip y el puerto del remitente
                                        obtenerIpYPuerto(cabeza,usuario_remitente,ip_remitente,&puerto_remitente);

                                        //creamos el socket
                                        int sock = crearsocket(ip_remitente, puerto_remitente);

                                        //enviamos la operación al hilo del cliente
                                        strcpy(buffer,"SEND MESS ACK");
                                        strcat(buffer, "\0");
                                        if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                        //enviamos el id del mensaje
                                        strcpy(buffer,id_mensaje);
                                        strcat(buffer, "\0");
                                        if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                        close(sock); //cerramos la conexion
                                }
                                else{
                                        //no está conectado el remitente (tiene que almacenar la comprobación como un mensaje)
                                        sendMessageEnLista(cabeza,usuario_remitente,"",id_mensaje);
                                        
                                }
                        }                        
                    }
                }

                if (strcmp(buffer,"DISCONNECT") == 0){                
                    /*DISCONNECT*/

                    char usuario[256];
                     
                    //obtenemos usuario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error");break;}
                    strcpy(usuario,buffer);

                    //comprobar si existe el usuario
                    int existe = nodoExiste(cabeza,usuario);

                    //Cambiamos los valores del usuario si existe y si está conectado
                    if (existe == 1){
                        printf("s> DISCONNECT %s OK\n",usuario);
                        modificarEnLista (cabeza,usuario,"",0,"Desconectado");
                        strcpy(buffer,"0");
                    }

                    if (existe == 0){ //no existe el usuario
                        printf("s> DISCONNECT %s FAIL\n",buffer);

                        strcpy(buffer,"1");
                    }

                    //enviamos confirmación
                    strcat(buffer, "\0");
                    if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error en envío\n");break;}
                }

                if (strcmp(buffer,"SEND") == 0){                  
                    /*SEND*/

                    char remitente[256];
                    char destinatario[256];
                    char mensaje[256];
                    char id_mensaje[256];
                    char ip_destinatario[256];
                    char ip_remitente[256];
                    int puerto_destinatario;
                    int puerto_remitente;

                    //obtenemos usuario que envía
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error\n");break;}
                    strcpy(remitente,buffer);

                    //obtenemos el usuario destinatario
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error\n");break;}
                    strcpy(destinatario,buffer);

                    //obtenemos el mensaje
                    if ((readLine(sc, buffer, 256)==-1)){printf("Error\n");break;}
                    strcpy(mensaje,buffer);

                    //comprobar si existe el usuario que envía (es redundante ya que si llega a enviar significa que existe)
                    int existe1 = nodoExiste(cabeza,remitente);

                    //comprobar si existe el usuario que recibe
                    int existe2 = nodoExiste(cabeza,destinatario);
 
                    if (existe1 == 1 && existe2 == 1){
                        //metemos el mesaje en la lista de mensajes del destinatario y sacamos el id del mensaje
                        strcat(buffer, "\0");
                        unsigned int id = sendMessageEnLista(cabeza,destinatario,remitente,mensaje);
                        
                        //comprobamos que el destinatario esté conectado para saber si enviarle el mensaje
                        int conectado = Conectado(cabeza, destinatario);

                        if (conectado == 1){
                                //obtenemos la ip y el puerto del destinatario para crear el socket y enviarle el mensaje
                                obtenerIpYPuerto(cabeza,destinatario,ip_destinatario,&puerto_destinatario);

                                //creamos el socket        
                                int sock = crearsocket(ip_destinatario, puerto_destinatario);

                                //obtenemos los datos que acabamos de meter en la lista del destinatario
                                obtenerUltimoMensaje(cabeza,destinatario,mensaje,id_mensaje,remitente);

                                //enviamos la operación al hilo del cliente
                                strcpy(buffer,"SEND MESSAGE");
                                strcat(buffer, "\0");
                                if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                //enviamos el remitente
                                strcpy(buffer,remitente);
                                strcat(buffer, "\0");
                                if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                //enviamos el id del mensaje
                                strcpy(buffer,id_mensaje);
                                strcat(buffer, "\0");
                                if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}
                                
                                
                                //enviamos el mensaje
                                strcpy(buffer,mensaje);
                                strcat(buffer, "\0");
                                if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                //actualizamos el último mensaje recibido
                                LastRcv(cabeza, destinatario, atoi(id_mensaje));

                                //cerramos el socket
                                close(sock);

                                printf("s> SEND MESSAGE %s FROM %s TO %s\n", id_mensaje, remitente, destinatario);

                                //Una vez enviado el mensaje vamos a enviarle al remitente el ack. Para ello le enviaremos un mensaje al remitente 
                                //En este caso el remitente está ya conectado por lo que no vamos a meter el mensaje dentro de la lista de mensajes, solo le notificaremos 
                                //Sacamos la ip y el puerto del remitente
                                obtenerIpYPuerto(cabeza,remitente,ip_remitente,&puerto_remitente);

                                //creamos el socket
                                sock = crearsocket(ip_remitente, puerto_remitente);

                                //enviamos la operación al hilo del cliente
                                strcpy(buffer,"SEND MESS ACK");
                                strcat(buffer, "\0");
                                if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                //enviamos el id del mensaje
                                strcpy(buffer,id_mensaje);
                                strcat(buffer, "\0");
                                if((sendMessage(sock, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}

                                close(sock); //cerramos la conexion
                        }
                        else{
                                printf("s> MESSAGE %d FROM %s TO %s STORED\n", id, remitente, destinatario);
                        }
                    
                        strcpy(buffer,"0");
                        //enviamos confirmación
                        strcat(buffer, "\0");
                        if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}
                        //enviamos el id
                        sprintf(id_mensaje,"%d",id);
                        strcpy(buffer, id_mensaje);
                        strcat(buffer, "\0");
                        if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}
                    }
                    else{
                        strcpy(buffer,"1");
                        //enviamos confirmación
                        strcat(buffer, "\0");
                        if ((sendMessage(sc, buffer, strlen(buffer)+1) == -1)){printf("Error\n");break;}
                    }

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

        pid_t pid = fork();
        char *arguments[4] = {"python3","ws-space-service.py", "&",NULL};
        if (pid != 0){
                execvp(arguments[0],arguments);
        }
        else{
                wait(NULL);
        }
        printf("Ha llegado\n");
        //char op;
	int err;
        //char buffer[256];

        if ((sd =  socket(AF_INET, SOCK_STREAM, 0))<0){
                printf("error en socket\n");
                return (0);
        }
        val = 1;
        setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &val, sizeof(int));

	bzero((char *)&server_addr, sizeof(server_addr));
    	server_addr.sin_family      = AF_INET;
    	server_addr.sin_addr.s_addr = INADDR_ANY;
    	server_addr.sin_port        = htons(atoi(argv[2]));

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