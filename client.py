from enum import Enum
import argparse
import socket
import sys

class client :

    # ******************** TYPES *********************
    # *
    # * @brief Return codes for the protocol methods
    class RC(Enum) :
        OK = 0
        ERROR = 1
        USER_ERROR = 2

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

            #envía host y puerto
            


            #mandamos petición de registro
            sock.sendall("Registro".encode()+b'\0')
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
                return client.RC.ERROR


        except:
            print("User error socket")
            sock.close()
            return client.RC.USER_ERROR


                
        
           

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
            sock.sendall("Unregister".encode())
            sock.sendall(b'\0')

            #recibimos respuesta
            print("La operación a realizar es: ",client.readResponse(sock))

            #enviamos usuario
            sock.sendall(str(user).encode())
            sock.sendall(b'\0')

            print("Aquí ya envia el usuario")

            r = client.readResponse(sock) #respuesta
            print("Confirmación recibida ",r)


            print("Closing socket")
            sock.close()

            if r == "0":
                return client.RC.OK

            if r == "1":
                return client.RC.ERROR


        except:
            print("User error socket")
            sock.close()
            return client.RC.USER_ERROR


    # *
    # * @param user - User name to connect to the system
    # * 
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist or if it is already connected
    # * @return ERROR if another error occurred
    @staticmethod
    def  connect(user) :
        #  Write your code here
        return client.RC.ERROR


    # *
    # * @param user - User name to disconnect from the system
    # * 
    # * @return OK if successful
    # * @return USER_ERROR if the user does not exist
    # * @return ERROR if another error occurred
    @staticmethod
    def  disconnect(user) :
        #  Write your code here
        return client.RC.ERROR

    # *
    # * @param user    - Receiver user name
    # * @param message - Message to be sent
    # * 
    # * @return OK if the server had successfully delivered the message
    # * @return USER_ERROR if the user is not connected (the message is queued for delivery)
    # * @return ERROR the user does not exist or another error occurred
    @staticmethod
    def  send(user,  message) :
        #  Write your code here
        return client.RC.ERROR

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

                            if (var) == 2: #usuario en uso
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
                            client.connect(line[1])
                        else :
                            print("Syntax error. Usage: CONNECT <userName>")

                    elif(line[0]=="DISCONNECT") :
                        if (len(line) == 2) :
                            client.disconnect(line[1])
                        else :
                            print("Syntax error. Usage: DISCONNECT <userName>")

                    elif(line[0]=="SEND") :
                        if (len(line) >= 3) :
                            #  Remove first two words
                            message = ' '.join(line[2:])
                            client.send(line[1], message)
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