
#define MAXSIZE 256


typedef struct mensaje{ //snodo es el nombre de la estructura
    char user_sender[MAXSIZE];
    char message[MAXSIZE];
    unsigned int id;
    
    struct mensaje *sig; //El puntero siguiente para recorrer la lista enlazada
}tmensaje; //tnodo es el tipo de dato para declarar la estructura
 
typedef tmensaje *tpuntero_mensaje; //Puntero al tipo de dato tnodo para no utilizar punteros de punteros


typedef struct snodo{ //snodo es el nombre de la estructura
    char user[MAXSIZE];
    char estado[MAXSIZE];
    char ip[MAXSIZE];
    int puerto;
    unsigned int last_recv;
    tpuntero_mensaje cabeza_mensaje;


    
    struct snodo *sig; //El puntero siguiente para recorrer la lista enlazada
}tnodo; //tnodo es el tipo de dato para declarar la estructura
 
typedef tnodo *tpuntero; //Puntero al tipo de dato tnodo para no utilizar punteros de punteros



