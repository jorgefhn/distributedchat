


typedef struct mensaje{ //snodo es el nombre de la estructura
    char user_sender[256];
    char message[256];
    unsigned int id;
    struct mensaje *sig; //El puntero siguiente para recorrer la lista enlazada
}tmensaje; //tnodo es el tipo de dato para declarar la estructura
 
typedef tmensaje *tpuntero_mensaje; //Puntero al tipo de dato tnodo para no utilizar punteros de punteros


typedef struct snodo{ //snodo es el nombre de la estructura
    char user[256];
    char estado[256];
    char ip[256];
    int puerto;
    unsigned int id_counter;
    unsigned int last_recv;
    struct mensaje *cabeza_mensaje;


    
    struct snodo *sig; //El puntero siguiente para recorrer la lista enlazada
}tnodo; //tnodo es el tipo de dato para declarar la estructura
 
typedef tnodo *tpuntero; //Puntero al tipo de dato tnodo para no utilizar punteros de punteros



