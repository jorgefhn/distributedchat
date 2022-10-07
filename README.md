Chat distribuido escrito en C y Python basado en el modelo Cliente-Servidor.

Compilación: 

Para compilar los archivos, basta con ejecutar el comando make


Ejecución: 

El comando a utilizar para ejecutar el servidor es el siguiente: 
                ./servidor -p <port_number>

Y para ejecutar, en cada cliente:
                python3 client.py -s <ip_addr> -p <port_number>

Donde <ip_addr> indica la dirección del servidor, y <port_number> el puerto al que se conecta del servidor. 
