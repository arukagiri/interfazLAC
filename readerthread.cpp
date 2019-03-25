#include "readerthread.h"
#include "better_serialfunc.h"

ReaderThread::ReaderThread(QSerialPort& serial_port)
{
    thread_serial_port = &serial_port;
}

void ReaderThread::handleRead(){
    uint16_t cant_msg=0, msgLeft=0; //Cantidad de mensajes(enteros) que se extrajeron del buffer, mensajes que quedan por procesar
    uint16_t first_byte[33]={0};    //Array de enteros que guarda la posicion del primer byte de un mensaje en pila, notar que el primer elemento de este vector siempre es 0
    static vector<char> pila;       //Vector utilizado para almacenar los bytes leidos
    static uint16_t notsup_count, notsup_gen;

    //Leemos el puerto pasando la pila en la cual se guardaran los mensajes, el array donde se indica la posicion del
    //principio de cada uno, y obviamente el puerto utilizado. Devuelve la cantidad de mensajes enteros leidos
    cant_msg=readport2(pila, first_byte, *thread_serial_port);
    msgLeft = cant_msg;//En un principio la cantidad de mensajes que faltan procesar es la misma que los leidos del puerto
    //Se procesa cada mensaje en cada ciclo
    for(int i=0;i<cant_msg;i++){
        LACAN_MSG msg;
        uint prevsize=0;
        char sub_pila[13]={0}; //Buffer para guardar los mensajes individuales (notar que tiene la longitud maxima posible de un mensaje)

        //Copio un mensaje de la pila al buffer(el primero que llego)
        for(uint j=0;j<first_byte[1];j++){
            //sub_pila[h]=pila.at(j);
            //h++;
            //VER esto deberia suplir lo q se hace arriba
            sub_pila[j]=pila.at(j);
        }

        //Borro el mensaje que acabo de copiar a la sub pila
        pila.erase(pila.begin()+first_byte[0],pila.begin()+first_byte[1]);

        //Reconfiguro los indices q indican el comienzo de cada mensaje (saco la longitud del proximo mensaje y se la sumo
        //al indice que indica el final del anterior)
        for(uint k=1; k<msgLeft; k++){
            first_byte[k]=first_byte[k+1]-first_byte[k]+first_byte[k-1];
        }
        msgLeft--;//Disminuyo la cantidad de mensajes que faltan procesar

        //Transformo el mensaje de bytes al tipo de dato LACAN_MSG para trabajarlo mas facilmente
        msg=mensaje_recibido2(sub_pila);
        emit receivedMsg(msg);

    }

}
