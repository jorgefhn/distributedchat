Compilación: 

Para compilar los archivos, basta con ejecutar el comando make


Ejecución: 

El comando a utilizar para ejecutar el servidor es el siguiente: 
                ./servidor -p <port_number>

Para ejecutar el web service, necesitamos primero instalarnos las dos librerías: 

		pip install zeep
		pip install spyne
		
Y después ejecutar el siguiente comando: 

		python3 ws-space-service.py
		
Y para ejecutar, en cada cliente:
                python3 client.py -s <ip_addr> -p <port_number>

Donde <ip_addr> indica la dirección del servidor, y <port_number> el puerto al que se conecta del servidor. 

Es importante ejecutarlos en este orden para su correcto funcionamiento.
