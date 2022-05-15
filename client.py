from enum import Enum
import argparse
import sys
import socket
import subprocess
import threading
from time import sleep
from threading import Semaphore

usuario_conectado = ""
sock2 = None
hilo = None
hilo2 = None

s = Semaphore()
class client:

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR1 = 1
        ERROR2 = 2
        ERROR3 = 3

    # ****************** ATTRIBUTES ******************
    _server = None
    _port = -1

    # ******************** METHODS *******************
    # *
    # * @param user - User name to register in the system
    # * 
    # * @return OK if successful
    # * @return USER_ERROR if the user is already registered
    # * @return ERROR if another error occurred 


    @staticmethod
    def openSocket(host,port):
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (host,int(port))
        sock.connect(server_address)
        return sock
  
    @staticmethod
    def listen():
        global sock2
        try:
            while(1):
                connection, client_address = sock2.accept()

                #recibe el la operación
                id_operacion = client.readResponse(connection)
                if(id_operacion == "SEND MESSAGE"):
                    #recibe un mensaje
                    #recibe el remitente
                    user_sender = client.readResponse(connection)

                    #recibe el id del del mensaje
                    id = client.readResponse(connection)

                    #recibe el mensaje    
                    mensaje = client.readResponse(connection)

                    print("MESSAGE "+ id +" FROM "+ user_sender +":\n"+ mensaje +"\nEND\nc> ", end="")
                else:
                    #recibe el ack
                    #recibe el id del del mensaje
                    id = client.readResponse(connection)
                    print("ACK OF MESSAGE "+ id +" RECEIVED\nc> ", end="")
                
        except:
            sock2.close()

    
    @staticmethod
    def readResponse(sock):
        a = ''
        while True:
            msg = sock.recv(1)
            if (msg == b'\0'):
                break
            a += msg.decode()

        return a


    @staticmethod
    def register(user,host,port):
        
        #esta la posibilidad de que el nombre sea "" asique quitamos esa posibilidad
        if user == "":
            return client.RC.ERROR2
        try:
            sock = client.openSocket(host,port)
            #mandamos petición de registro
            sock.sendall("REGISTER".encode()+b'\0')

            #enviamos usuario
            sock.sendall(str(user).encode()+b'\0')

            r = client.readResponse(sock) #respuesta

            sock.close()

            if r == "0":
                return client.RC.OK

            #ya existe un usuario con ese nombre
            if r == "1":
                return client.RC.ERROR1

        #si el servidor no está conectado
        except ConnectionRefusedError as e:
            return client.RC.ERROR2
        except:
            sock.close()
            return client.RC.ERROR2

    @staticmethod
    def unregister(user,host,port):

        if usuario_conectado == user:
            return client.RC.ERROR2

        try:
            sock = client.openSocket(host,port)
            #mandamos petición de registro
            sock.sendall("UNREGISTER".encode()+b'\0')

            #enviamos usuario
            sock.sendall(str(user).encode()+b'\0')

            r = client.readResponse(sock) #respuesta

            sock.close()

            if r == "0":
                return client.RC.OK

            if r == "1":
                return client.RC.ERROR1

        #si el servidor no está conectado
        except ConnectionRefusedError as e:
            return client.RC.ERROR2
        except:
            sock.close()
            return client.RC.ERROR2

    @staticmethod
    def connect(user, host, port):
        global usuario_conectado, sock2, hilo

        #comprobamos que ningún usuario esté ya conectado 
        if usuario_conectado == user:
            return client.RC.ERROR2
    
        try:
            sock = client.openSocket(host,port)

            #enviamos solicitud de conexión
            sock.sendall("CONNECT".encode()+b'\0')
            
            #enviamos el usuario 
            sock.sendall(str(user).encode()+b'\0')

            #Recibimos la confirmación
            r = client.readResponse(sock) #respuesta
            
            #si la confirmación es satisfactoria 
            if r == "0":
                #actualizamos la variable global del usuario que está conectado
                usuario_conectado = user

                #creamos el nuevo socket tipo servidor que será el que utilice el hilo que escucha mensajes
                sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                sock2.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

                ip = str(sock.getsockname()[0])
                server_address = (ip, 0)

                sock2.bind(server_address)
                
                ip = str(sock2.getsockname()[0])
                puerto = str(sock2.getsockname()[1])

                #Vamos a enviar los datos para que el servidor actualice al usuario
                #enviamos la ip
                sock.sendall(ip.encode()+b'\0')

                #enviamos el puerto 
                sock.sendall(puerto.encode()+b'\0')

                sock2.listen(1)
                #creamos el hilo que va a recibir mensajes mientras el usuario siga conectado
                hilo = threading.Thread(target=client.listen)
                hilo.start()

                return client.RC.OK

            if r == "1":
                return client.RC.ERROR1

            sock.close()

        #si el servidor no está conectado
        except ConnectionRefusedError as e:
            return client.RC.ERROR3
        except:
            sock.close()
            return client.RC.ERROR3

    @staticmethod
    def disconnect(user, host, port) :
        global usuario_conectado, sock2, hilo

        if usuario_conectado != user:
            return client.RC.ERROR2

        try:
            sock = client.openSocket(host,port)

            #enviamos solicitud de conexión
            sock.sendall("DISCONNECT".encode()+b'\0')
            
            #enviamos el usuario 
            sock.sendall(str(user).encode()+b'\0')

            #Recibimos la confirmación
            r = client.readResponse(sock) #respuesta


            sock.close()

            sock2.shutdown(socket.SHUT_RDWR)
           
            hilo.join()
            
            #Devolvemos el resultado
            if r == "0":
                usuario_conectado = ""
                return client.RC.OK

            if r == "1":
                return client.RC.ERROR1

        #si el servidor no está conectado
        except ConnectionRefusedError as e:
            return client.RC.ERROR3
        except:
            sock.close()
            return client.RC.ERROR3

    @staticmethod
    def send(user, message,host,port):
        global usuario_conectado
        id = ""
        #cuando se intenta hacer un send sin estar conectado o un send al mismo usuario conectado
        if usuario_conectado == "" or usuario_conectado == user:
            return client.RC.ERROR2, id

        try:
            sock = client.openSocket(host,port)

            #enviamos solicitud de conexión
            sock.sendall("SEND".encode()+b'\0')

            #enviamos el usuario que envía el mensaje
            sock.sendall(str(usuario_conectado).encode()+b'\0')

            #enviamos el usuario al que se le envia el mensaje 
            sock.sendall(str(user).encode()+b'\0')

            #enviamos el mensaje 
            sock.sendall(str(message).encode()+b'\0')

            #Recibimos la confirmación
            r = client.readResponse(sock) #respuesta
 
            if r == "0":
                id = client.readResponse(sock) #respuesta
                return client.RC.OK, id
            else:
                return client.RC.ERROR1, id

            sock.close()

        #si el servidor no está conectado
        except ConnectionRefusedError as e:
            return client.RC.ERROR2
        except:
            #error
            sock.close()
            return client.RC.ERROR2, id

    @staticmethod
    def  sendAttach(user,  file,  message) :
        #  Write your code here
        return client.RC.ERROR

    @staticmethod
    def shell():

        host = sys.argv[2]
        port = sys.argv[4]

        #subprocess.Popen(['python3','ws-space-service.py']) #corre el servicio en background

        while (True) :
            try :
                command = input("c> ")
                line = command.split(" ")
                if (len(line) > 0):
                    line[0] = line[0].upper()
                    if (line[0]=="REGISTER") :
                        if (len(line) == 2) :
                            var = client.register(line[1],host,port).value
                            if var == 0: #ok
                                print("REGISTER OK")
                            if (var) == 1: #usuario en uso
                                print("USERNAME IN USE")
                            if (var) == 2: #fallo en el registro
                                print("REGISTER FAIL")
                        else :
                            print("Syntax error. Usage: REGISTER <userName>")

                    elif(line[0]=="UNREGISTER") :
                        if (len(line) == 2) :
                            var = client.unregister(line[1],host,port).value
                            if var == 0: #ok
                                print("UNREGISTER OK")
                            if (var) == 1: #usuario no existe
                                print("USER DOES NOT EXIST")
                            if (var) == 2: #error
                                print("UNREGISTER FAIL")
                        else :
                            print("Syntax error. Usage: UNREGISTER <userName>")

                    elif(line[0]=="CONNECT") :
                        if (len(line) == 2) :
                            var = client.connect(line[1],host,port).value
                            if var == 0: #ok
                                print("CONNECT OK")
                            if (var) == 1: #usuario no existe
                                print("CONNECT FAIL, USER DOES NOT EXIST")
                            if (var) == 2: #error, usuario ya conectado
                                print("USER ALREADY CONNECTED")
                            if (var) == 3: #error
                                print("CONNECT FAIL")
                        else :
                            print("Syntax error. Usage: CONNECT <userName>")

                    elif(line[0]=="DISCONNECT") :
                        if (len(line) == 2) :
                            var = client.disconnect(line[1],host,port).value
                            if var == 0: #ok
                                print("DISCONNECT OK")
                            if (var) == 1: #usuario no existe
                                print("DISCONNECT FAIL / USER DOES NOT EXIST")
                            if (var) == 2: #error, usuario no estaba conectado
                                print("DISCONNECT FAIL / USER NOT CONNECTED")
                            if (var) == 3: #error
                                print("DISCONNECT FAIL")
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="SEND") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            message = '"'+' '.join(line[2:])+'"'
                            formateado = subprocess.check_output(['python3','ws-space-client.py',message])#formatea el mensaje
                            formateado = formateado.decode()[1:-2] #para el output
                            
                            print("Mensaje formateado:",formateado)
                            var, id = client.send(line[1], formateado,host,port)
                            var = var.value
                            if var == 0: #ok
                                print("SEND OK - MESSAGE "+id)
                            if var == 1: #usuario no existe
                                print("SEND FAIL / USER DOES NOT EXIST")
                            if var == 2: #error
                                print("SEND FAIL")
                        else :
                            print("Syntax error. Usage: SEND <userName> <message>")

                    elif(line[0]=="SENDATTACH") :
                        if (len(line) >= 4) :
                            #  Remove first two words
                            message = ' '.join(line[3:])
                            client.sendAttach(line[1], line[2], message)
                        else :
                            print("Syntax error. Usage: SENDATTACH <userName> <filename> <message>")

                    elif(line[0]=="QUIT") :
                        if (len(line) == 1) :
                            break
                        else :
                            print("Syntax error. Use: QUIT")
                    else :
                        print("Error: command " + line[0] + " not valid.")
            except Exception as e:
                print("Exception: " + str(e))

    # *
    # * @brief Prints program usage
    @staticmethod
    def usage() :
        print("Usage: python3 client.py -s <server> -p <port>")


    # *
    # * @brief Parses program execution arguments
    @staticmethod
    def  parseArguments(argv) :
        parser = argparse.ArgumentParser()
        parser.add_argument('-s', type=str, required=True, help='Server IP')
        parser.add_argument('-p', type=int, required=True, help='Server Port')
        args = parser.parse_args()

        if (args.s is None):
            parser.error("Usage: python3 client.py -s <server> -p <port>")
            return False

        if ((args.p < 1024) or (args.p > 65535)):
            parser.error("Error: Port must be in the range 1024 <= port <= 65535");
            return False;
        
        _server = args.s
        _port = args.p

        return True


    # ******************** MAIN *********************
    @staticmethod
    def main(argv) :
        if (not client.parseArguments(argv)) :
            client.usage()
            return

        #  Write code here
        client.shell()
        print("+++ FINISHED +++")
    

if __name__=="__main__":
    client.main([])