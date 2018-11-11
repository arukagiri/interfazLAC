#include "better_serialfunc.h"

#include <iostream>
#include <QDebug>
#include "tiempo.h"

using namespace std;

//Envia un mensaje ya armado por el puerto que se tiene abierto
void serialsend2(QSerialPort& serial_port,LACAN_MSG msg){
    uint8_t byte2, byteID1, byteID2;
    QByteArray msg_hexa;
    //aa cX Bid2 Bid1 Bdat*X 55
    msg_hexa.append(0xAA); //se agrega el byte de comienzo del mensaje

    byte2=0xC0 + msg.DLC; //se agrega el byte con la indicacion de cantidad de bytes que posee el mensaje
    msg_hexa.append(byte2);

    byteID1=(uint16_t)msg.ID&0xFF;       //La ID del mensaje se manda al reves (comportamiento natural del adaptador)
    byteID2=(uint16_t)(msg.ID>>8)&0x7;
    msg_hexa.append(byteID1);
    msg_hexa.append(byteID2);

    //Se agregan los bytes del campo de datos en orden segun sea la longitud del mismo
    if(msg.DLC>0){
        msg_hexa.append(msg.BYTE0);
        if(msg.DLC>1){
            msg_hexa.append(msg.BYTE1);
            if(msg.DLC>2){
                msg_hexa.append(msg.BYTE2);
                if(msg.DLC>3){
                    msg_hexa.append(msg.BYTE3);
                    if(msg.DLC>4){
                        msg_hexa.append(msg.BYTE4);
                        if(msg.DLC>5){
                            msg_hexa.append(msg.BYTE5);
                            if(msg.DLC>6){
                                msg_hexa.append(msg.BYTE6);
                                if(msg.DLC>7)
                                    msg_hexa.append(msg.BYTE7);
                                }
                            }
                        }
                    }
                }
            }
        }
    //Se agrega el indicador del final del mensaje
    msg_hexa.append(0x55);

    serial_port.write(msg_hexa);//se envia el array de bytes armado, REVISAR EL CASO DE Q OCURRAN ERRORES
    serial_port.flush();
}
//Envia la secuencia de inicializacion del adaptador, se configuran parametros propios de la comunicacion
//En un principio solo se contempla la posible modificacion del baud rate del CAN
void sendinit2(QSerialPort& serial_port,uint8_t bdr){
    uint8_t crc=0xFE;
    //suma modular, checksum para corregir posibles errores
    crc=(crc+0xAA+0x55+0x12+bdr+0x01+0xFF+0x07)%0xFF;
    //primeros dos bytes de cabecera
    //3er byte ??
    //4to byte Baud rate, ver opciones
    //5to byte modo ID: standard(0x02 extended)
    //6to y 7mo byte: filtro ID y mask ID 0,0,0,0,0,70,0,0,0,0,0,0,0,0,0,0,0

    QByteArray msg_hexa;
    msg_hexa.append(0xAA);
    msg_hexa.append(0x55);
    msg_hexa.append(0x12);
    msg_hexa.append(bdr);
    msg_hexa.append(0x01);
    for(int i=0; i<12; i++){
        msg_hexa.append(static_cast<char>(0));//magia de la buena, fuerzo un cast a char de los 0, de otra manera no lo toma o los confunde con un final de string
    }
    msg_hexa.append(crc);

    serial_port.write(msg_hexa);
    serial_port.flush();
}

LACAN_MSG mensaje_recibido2(char *sub_pila){
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
    case(7):
        mje.BYTE6=sub_pila[10];
    case(6):
        mje.BYTE5=sub_pila[9];
    case(5):
        mje.BYTE4=sub_pila[8];
    case(4):
        mje.BYTE3=sub_pila[7];
    case(3):
        mje.BYTE2=sub_pila[6];
    case(2):
        mje.BYTE1=sub_pila[5];
    case(1):
        mje.BYTE0=sub_pila[4];
    }
    return mje;
}

//Se encarga de leer el puerto, busca un nuevo dato, si ReadChar no lo encuentra regresa automaticamente
//Verifica los primeros 12bits para verificar que es un mensaje valido, no se contempla la verificacion del final del mensaje
/*VER si hace falta pasar index_pila, quiza podria ser estatica dentro de readport, haciendo la verificacion del mensaje ahi*/
uint16_t readport2(char* pila, uint16_t* first_byte, QSerialPort& serial_port){
    bool newdataflag = false;
    uint16_t cant_msg = 0;
    static uint16_t index_pila=0;   //  ESTO POR QUE ES STATIC???????????????????????????????????
    static uint16_t dlc=0;          // ESTO POR QUE ES STATIC????????????????????????????????????
    first_byte[0]=0;    //esto es redundante pero fue

    //ver de poner un while, es posible perder datos de esta forma TESTEAR
    while((newdataflag=serial_port.read(pila+index_pila,1))==1){ //devuelve la cantidad de bytes leidos (deberia ser 1 por el limite impuesto)

        qDebug()<<"algo se puede leer: "<<QString::number(pila[index_pila]);

        index_pila++;   //ya apunta a la siguiente
        if(index_pila==1){
            if((pila[0]&0xFF)==0xAA)
                qDebug()<<"\nLlego AA\n";   //primeros 8 bits de cabecera de mensaje
                //timeout.start();
            else
                index_pila=0;
                //conterror++;
        }
        if(index_pila==2){
            if(((pila[1]&0xFF)>>4)==0xC){   //ultimos 4 bits de cabecera
                qDebug()<<"Llego 0xC + dlc\n";
                dlc=pila[1]&15;             //extraigo dlc
                //conterror=0;
            }
            else
                index_pila=0;
                //conterror++;
            }

        if(newdataflag==-1){//si es 0 no pasa es pq no hay dato; si es -1 hay un error
            qDebug()<<"\nError en la lectura\n";
            //index_pila=0;
        }

        if((index_pila>=(dlc+5))&&(((pila[dlc+4])&0xFF)==0x55)){//comprobamos que el mensaje llego entero
            qDebug()<<"ENTRO UN MENSAJE COMPLETO";
            //index_pila=0; //reseteamos variables para volverlas a usar en el proximo mensaje
            dlc=0;
            cant_msg++;
            first_byte[cant_msg]=index_pila; //guardo la primer direccion del siguiente mensaje, si es que existe (index_pila ya apunta al proximo)
                                                //osea, si es la primera vez que entra, estoy guardando en el segundo elemento de first_byte, la posicion del 0xAA del segundo mensaje que puede llegar
        }
    }
    return cant_msg;
}

bool readport(char* pila, QSerialPort& serial_port){
    bool newdataflag=false;
   bool newmsgflag=false;
   static uint16_t index_pila=0;
   static uint16_t dlc=0;
   //ver de poner un while, es posible perder datos de esta forma TESTEAR
   while((newdataflag=serial_port.read(pila+index_pila,1))==1){ //devuelve la cantidad de bytes leidos (deberia ser 1 por el limite impuesto)

       qDebug()<<"algo se puede leer: "<<QString::number(pila[index_pila]);


       index_pila++;
       if(index_pila==1){
           if((pila[0]&0xFF)==0xAA)
               qDebug()<<"\nLlego AA\n";
               //timeout.start();
           else
               index_pila=0;
               //conterror++;
       }
       if(index_pila==2){
           if(((pila[1]&0xFF)>>4)==0xC){
               qDebug()<<"Llego b2\n";
               dlc=pila[1]&15;
               //conterror=0;
           }
           else
               index_pila=0;
               //conterror++;
           }

       if(newdataflag==-1){//si es 0 no pasa es pq no hay dato; si es -1 hay un error
           qDebug()<<"\nError en la lectura\n";
           //index_pila=0;
       }

   if((index_pila>=(dlc+5))&&(((pila[dlc+4])&0xFF)==0x55)){//comprobamos que el mensaje llego entero
       index_pila=0; //reseteamos variables para volverlas a usar en el proximo mensaje
       dlc=0;
       qDebug()<<"ENTRO";
       return newmsgflag=true;

   }
   }
   return newmsgflag=false;
}


bool openport2(uint8_t bdr, QSerialPort* serial_port){
    char* pila= new char[7];
    char retval;
    bool com_detected=0;
    QTimer* wait_portinit= new QTimer();
    wait_portinit->setSingleShot(true);
    static int portnumber=0;

    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){//revisamos los puertos habilitados
        //TEST
        portnumber++;
        qDebug()<<portnumber;

        serial_port->setPort(info);
        serial_port->setBaudRate(QSerialPort::Baud115200);
        serial_port->setDataBits(QSerialPort::Data8);
        serial_port->setParity(QSerialPort::NoParity);
        serial_port->setStopBits(QSerialPort::OneStop);
        serial_port->setFlowControl(QSerialPort::NoFlowControl);
        retval = serial_port->open(QSerialPort::ReadWrite); // abrimos el puerto com señalado, retval=1 si hay algo conectado y lo pudo abrir
        if(!retval){            //si no devuelve 1 es porque no se puede abrir=>aumentamos el nro de com y pasamos al siguiente
             serial_port->close();
        }
        else{                                               //si devuelve 1 es porque esta conectado
            qDebug()<<"No hubo problemas abriendo el puerto \nComenzando inicializacion de adaptador";
            sendinit2(*serial_port,bdr); // enviamos secuencia de iniciacion segun un baudrate(CAN) elegido
            wait_portinit->start(30000); //se inicializa un timer para esperar una respuesta
            while(wait_portinit->remainingTime()){ //leemos el puerto y verificamos que el comienzo del mensaje es valido VER
                //se compara el tamaño en bytes del mensaje con el que deberia tener,
                if(readport(pila,*serial_port)){
                    if((pila[4]>>LACAN_BYTE0_RESERVED)==LACAN_ID_BROADCAST){//verificamos que el destino sea broadcast (estamos buscando un HB)
                        LACAN_MSG mje=mensaje_recibido2(pila);//armamos el mensaje
                        if((mje.ID>>LACAN_IDENT_BITS)==LACAN_FUN_HB){//si resulta que el mensaje recibido es un HB, se considera que encontramos el puerto correcto
                            qDebug()<<"\nesta en el com "<<serial_port->portName()<<"\n";
                            com_detected=TRUE;
                            return true;
                        }

                    }
                }
            }

            //if(com_detected)
            if(1){
                //sendinit2(*serial_port,bdr);
                //break;       salgo del for
            }else{
                serial_port->close();
                qDebug()<<"\nEl el com hay algo pero no contesta\n";
            }
        }
    }
    portnumber=0;
    wait_portinit->stop();
    return false;

}
