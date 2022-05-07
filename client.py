from enum import Enum
import argparse
import socket
import sys
import threading

usuario_conectado = ""
sock2 = None
hilo = None

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
        print('connecting to {} port {}'.format(*server_address))
        sock.connect(server_address)
        return sock

  

    @staticmethod
    def listen():
        global sock2
        try:
            while(1):
                connection, client_address = sock2.accept()
                print("connection from: ",client_address)
                mensaje = client.readResponse(connection)
                print(mensaje)

        except:
            print("Conexión terminada")
            sock2.close()

    
    @staticmethod
    def readResponse(sock):
        a = ''
        while True:
            msg = sock.recv(1)
            if (msg == b'\0'):
                break;
            a += msg.decode()

        return a


    @staticmethod
    def register(user,host,port):
        
        sock = client.openSocket(host,port)
        
        try:
            #mandamos petición de registro
            sock.sendall("REGISTER".encode()+b'\0')
            #sock.sendall(b'\0')

            #recibimos respuesta
            print("La operación a realizar es: ",client.readResponse(sock))

            #enviamos usuario
            sock.sendall(str(user).encode()+b'\0')

            r = client.readResponse(sock) #respuesta
            print("Confirmación recibida: ",r)


            print("Closing socket")

            sock.close()

            if r == "0":
                return client.RC.OK

            if r == "1":
                return client.RC.ERROR1

        except:
            print("User error socket")
            sock.close()
            return client.RC.ERROR2

    # *
    # 	 * @param user - User name to unregister from the system
    # 	 * 
    # 	 * @return OK if successful
    # 	 * @return USER_ERROR if the user does not exist
    # 	 * @return ERROR if another error occurred
    @staticmethod
    def unregister(user,host,port):

        sock = client.openSocket(host,port)
        
        try:
            #mandamos petición de registro
            sock.sendall("UNREGISTER".encode()+b'\0')

            #recibimos respuesta
            print("La operación a realizar es: ",client.readResponse(sock))

            #enviamos usuario
            sock.sendall(str(user).encode()+b'\0')
    
            print("Aquí ya envia el usuario")

            r = client.readResponse(sock) #respuesta
            print("Confirmación recibida ",r)

            print("Closing socket")
            sock.close()

            if r == "0":
                return client.RC.OK

            if r == "1":
                return client.RC.ERROR1

        except:
            print("User error socket")
            sock.close()
            return client.RC.ERROR2


    # *
    # * @param user - User name to connect to the system
    # * 
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist or if it is already connected
    # * @return ERROR if another error occurred
    @staticmethod
    def connect(user, host, port):
        global usuario_conectado, sock2, hilo
        #Creamos el socket

        if usuario_conectado == user:
            return client.RC.ERROR2

        sock = client.openSocket(host,port)
        
        try:
            #enviamos solicitud de conexión
            sock.sendall("CONNECT".encode()+b'\0')
            
            #recibimos confirmación de la operación
            print("La operación a realizar es: ",client.readResponse(sock))
            
            #enviamos el usuario 
            sock.sendall(str(user).encode()+b'\0')

            ip = str(sock.getsockname()[0])
            puerto = str(sock.getsockname()[1])

            #enviamos la ip
            sock.sendall(ip.encode()+b'\0')

            #enviamos el puerto 
            sock.sendall(puerto.encode()+b'\0')

            #Recibimos la confirmación
            r = client.readResponse(sock) #respuesta
            print("Confirmación recibida ",r)

            print("Closing socket")
            sock.close()

            if r == "0":
                usuario_conectado = user
                print("EL usuario "+user+" se ha conectado")
                sock2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                server_address = (ip, 0)
                sock2.bind(server_address)
                sock2.listen(1)
                ip = str(sock2.getsockname()[0])
                puerto = str(sock2.getsockname()[1])
                hilo = threading.Thread(target=client.listen)
                hilo.start()

                return client.RC.OK

            if r == "1":
                return client.RC.ERROR1

            print("Closing socket")
            
        except:
            print("User error socket")
            sock.close()
            return client.RC.ERROR3


    # *
    # * @param user - User name to disconnect from the system
    # * 
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist
    # * @return ERROR if another error occurred
    @staticmethod
    def disconnect(user, host, port) :
        global usuario_conectado, sock2, hilo

        if usuario_conectado != user:
            return client.RC.ERROR2

        sock = client.openSocket(host,port)

        try:
            #enviamos solicitud de conexión
            sock.sendall("DISCONNECT".encode()+b'\0')
            
            #recibimos confirmación de la operación
            print("La operación a realizar es:",client.readResponse(sock))

            #enviamos el usuario 
            sock.sendall(str(user).encode()+b'\0')

            #Recibimos la confirmación
            r = client.readResponse(sock) #respuesta
            print("Confirmación recibida ",r)

            print("Closing socket")
            sock.close()

            sock2.shutdown(socket.SHUT_RDWR)
            hilo.join()

            #Devolvemos el resultado
            if r == "0":
                usuario_conectado = ""
                return client.RC.OK

            if r == "1":
                return client.RC.ERROR1

        except:
            print("User error socket")
            sock.close()
            return client.RC.ERROR3

    # *
    # * @param user    - Receiver user name
    # * @param message - Message to be sent
    # * 
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def send(user, message):
        global usuario_conectado

        id = None

        #usuario al que se le quiere enviar el mensaje no existe
        if usuario_conectado == "":
            return client.RC.ERROR, id

        sock = client.openSocket(host,port)

        try:
            #enviamos solicitud de conexión
            sock.sendall("SEND".encode()+b'\0')

            #recibimos confirmación de la operación
            print("La operación a realizar es:",client.readResponse(sock))

            #enviamos el usuario 
            sock.sendall(str(user).encode()+b'\0')

            #enviamos el mensaje 
            sock.sendall(str(message).encode()+b'\0')

            #Recibimos la confirmación que en este caso es el id asociado al mensaje o error si no existe
            id = client.readResponse(sock) #respuesta
            print("Confirmación recibida ",r)

            print("Closing socket")
            sock.close()

            #exito
            if ip == "error":
                return client.RC.ERROR, id
            else:
                return client.RC.OK, id

        except:
            #error
            print("User error socket")
            sock.close()
            return client.RC.ERROR2, id

    # *
    # * @param user    - Receiver user name
    # * @param file    - file  to be sent
    # * @param message - Message to be sent
    # * 
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def  sendAttach(user,  file,  message) :
        #  Write your code here
        return client.RC.ERROR

    # *
    # **
    # * @brief Command interpreter for the client. It calls the protocol functions.
    @staticmethod
    def shell():

        host = sys.argv[2]
        port = sys.argv[4]

        while (True) :
            try :
                command = input("c> ")
                line = command.split(" ")
                if (len(line) > 0):
                    line[0] = line[0].upper()
                    if (line[0]=="REGISTER") :
                        if (len(line) == 2) :
                            var = client.register(line[1],host,port).value
                            print("Var: ",var)
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
                            print("Var: ",var)
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
                            if usuario_conectado != "":
                                print("USER ALREADY CONNECTED")

                            else:
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
                                print("DISCONNECT FAIL, USER DOES NOT EXIST")
                            if (var) == 2: #error, usuario no estaba conectado
                                print("USER NOT CONNECTED")
                            if (var) == 3: #error
                                print("DISCONNECT FAIL")
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="SEND") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            message = ' '.join(line[2:])
                            var, id = client.send(line[1], message)
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