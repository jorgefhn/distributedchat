import time
from spyne import Application, ServiceBase, String, Unicode, srpc
from spyne.protocol.soap import Soap11
from spyne.server.wsgi import WsgiApplication

class Espaciador(ServiceBase):
    @srpc(String,_returns=String)

    def conversor(mensaje):

        palabras,final,new,counter = [],"","",0
        for m in mensaje:
            if m != " ":
                new += m
            if (m == " " or counter == len(mensaje)-1) and new != "":
                palabras.append(new)
                new = ""
            counter += 1
          
        for i in range(len(palabras)-1):
            final += palabras[i] + " "
        final += palabras[-1]
        return final


soap_app = Application(
            services = [Espaciador],
            tns='http://tests.python-zeep.org/',
            in_protocol = Soap11(validator='lxml'),
            out_protocol = Soap11())

application = WsgiApplication(soap_app)

if __name__ == '__main__':
    import logging
    from wsgiref.simple_server import make_server
    logging.basicConfig(level=logging.DEBUG)
    logging.getLogger('spyne.protocol.xml').setLevel(logging.DEBUG)
    logging.info("listening to http://127.0.0.1:8000")
    logging.info("wsdl is at: http://localhost:8000/?wsdl")
    server = make_server('127.0.0.1', 8000, application)
    server.serve_forever()