#include "readerthread.h"
#include <QDebug>

ReaderThread::ReaderThread(QSerialPort& serial_port)
{
    thread_serial_port = &serial_port;
}

void ReaderThread::handleRead(){
    uint cant_msg=0, msgLeft=0; //Cantidad de mensajes(enteros) que se extrajeron del buffer, mensajes que quedan por procesar
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

//Se encarga de leer el puerto, busca un nuevo dato, si ReadChar no lo encuentra regresa automaticamente
//Verifica los primeros 12bits para verificar que es un mensaje valido, no se contempla la verificacion del final del mensaje
/*VER si hace falta pasar index_pila, quiza podria ser estatica dentro de readport, haciendo la verificacion del mensaje ahi*/
uint ReaderThread::readport2(vector<char> &pila, uint16_t* first_byte, QSerialPort& serial_port){
    qint64 newdataflag = 0;
    char buffer[200];
    uint cantBytes=0;
    static uint losedMsgCount = 0;
    uint16_t cant_msg = 0;
    static uint16_t index_buffer=0;
    static uint16_t dlc=0;
    first_byte[0]=0;    //esto es redundante pero fue, guarda la posicion del primer byte del proximo mensaje
    bool lastMsgIsFull = false;

    while((newdataflag=serial_port.read(buffer+index_buffer,1))==1){ //devuelve la cantidad de bytes leidos (deberia ser 1 por el limite impuesto)
        lastMsgIsFull=false;
        cantBytes++;

        qDebug()<<"nuevo byte: "<<QString::number(buffer[index_buffer]);
        index_buffer++;        //ya apunta a la siguiente
        char fuck = buffer[index_buffer];
        bool test = fuck==char(-86);
        qDebug()<<fuck<<test;
        if((buffer[index_buffer]&0xFF)==0xAA){
            if(index_buffer-first_byte[cant_msg]==1){
                qDebug()<<"\nLlego AA\n";
            }
            else{
                index_buffer=first_byte[cant_msg];
                losedMsgCount++;
            }
        }
        else{
            if(index_buffer-first_byte[cant_msg]==1){
                index_buffer=first_byte[cant_msg];
                losedMsgCount++;
            }
        }

        if(((buffer[index_buffer]&0xFF)>>4)==0xC){  //ultimos 4 bits de cabecera
            if(index_buffer-first_byte[cant_msg]==2){
                qDebug()<<"Llego 0xC + dlc\n";
                dlc=buffer[index_buffer]&15;                //extraigo dlc
            }
            else{
                index_buffer=first_byte[cant_msg];
                losedMsgCount++;
            }
        }
        else{
            if(index_buffer-first_byte[cant_msg]==2){
                index_buffer=first_byte[cant_msg];
                losedMsgCount++;
            }
        }


        if(((buffer[index_buffer])&0xFF)==0x55){
            if(index_buffer-first_byte[cant_msg]>=(dlc+5)){
                qDebug()<<"ENTRO UN MENSAJE COMPLETO";
                //index_buffer=0; //reseteamos variables para volverlas a usar en el proximo mensaje
                dlc=0;
                cant_msg++;
                lastMsgIsFull=true;
                first_byte[cant_msg]=index_buffer;    //notar que index buffer ya apunta a la siguiente
                                                //guardo la primer direccion del siguiente mensaje, si es que existe (index_buffer ya apunta al proximo)
            }                                  //osea, si es la primera vez que entra, estoy guardando en el segundo elemento de first_byte, la posicion del 0xAA del segundo mensaje que puede llegar
            else{
                index_buffer=first_byte[cant_msg];
                losedMsgCount++;
            }
        }
        else{
            if(index_buffer-first_byte[cant_msg]>=(dlc+5)){
                index_buffer=first_byte[cant_msg];
                losedMsgCount++;
            }
        }
    }

    for(uint i=cantBytes;i>0;i--){
        pila.push_back(buffer[index_buffer-i]);
    }

    if(lastMsgIsFull){
        index_buffer=0;
    }
    else{
        index_buffer=index_buffer-first_byte[cant_msg];
    }

    emit msgLost(losedMsgCount);

    return cant_msg;
}

LACAN_MSG ReaderThread::mensaje_recibido2(char *sub_pila){
    LACAN_MSG mje;
    mje.DLC=sub_pila[1]&DLC_MASK;//Me quedo unicamente con el DLC (la primer mitad del byte es 0xC)
    //Como los bytes de ID se mandan al reves, tenemos la parte menos significativa del campo de funcion
    //en el primer byte (primeros 3 bits) y la mas significativa en el segundo (ultimos 3 bits)
    uint16_t fun=((sub_pila[2]&BOTTOM_FUN_MASK)>>FUN_MOV_BOTTOM)|((sub_pila[3]&UPPER_FUN_MASK)<<FUN_MOV_UPPER);
    uint16_t source=sub_pila[2]&LACAN_IDENT_MASK;
    mje.ID=(fun<<FUN_MOV_FORSOURCE)|source;//armamos la ID de la forma en la cual esta diseñado CAN para facil entendimiento y utilizacion
    //Se almacenan los datos en la struct diseñada para el mensaje, el switch se comporta como cascada (sin breaks)
    switch(mje.DLC){
    case(8):
        mje.BYTE7=sub_pila[11];
    [[clang::fallthrough]]; case(7):
        mje.BYTE6=sub_pila[10];
    [[clang::fallthrough]]; case(6):
        mje.BYTE5=sub_pila[9];
    [[clang::fallthrough]]; case(5):
        mje.BYTE4=sub_pila[8];
    [[clang::fallthrough]]; case(4):
        mje.BYTE3=sub_pila[7];
    [[clang::fallthrough]]; case(3):
        mje.BYTE2=sub_pila[6];
    [[clang::fallthrough]]; case(2):
        mje.BYTE1=sub_pila[5];
    [[clang::fallthrough]]; case(1):
        mje.BYTE0=sub_pila[4];
    }
     return mje;
}
