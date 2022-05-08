#include "nodo.h"
#include "linked-list.c"

int insertarEnLista (tpuntero *cabeza, char* user);
int imprimirLista (tnodo *cabeza);
int borrarLista (tpuntero *cabeza);
int nodoExiste(tnodo *cabeza,char* user);
int borrarPorUsuario(tpuntero*cabeza, char *user);
int numItems(tnodo *cabeza);
int modificarEnLista(tnodo *cabeza, char * user, char * ip, int port, char* estado);
int insertarEnListaMessage(tpuntero_mensaje *cabeza, char* remitente, char* mensaje, int id);
int imprimirListaMessage(tmensaje *cabeza);

int sendMessageEnLista(tnodo *cabeza, char * user, char * remitente, char * mensaje);
//------------------------------------------------------------------------------------
int borrarListaMessage(tpuntero_mensaje *cabeza);
/*



struct snodo buscarEnLista (tnodo *cabeza, int key);
int modificarEnLista (tnodo *cabeza, int key, char *value1, int value2, float value3);
int borrarPorClave(tpuntero*cabeza, int key);

*/