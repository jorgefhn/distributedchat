import sys
import zeep
wsdl = "http://localhost:8000/?wsdl"

client = zeep.Client(wsdl=wsdl)
print(client.service.conversor(str(sys.argv[1])))
    



