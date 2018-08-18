#include "serialfunc.h"

#include <iostream>

using namespace std;

//Envia un mensaje ya armado por el puerto que se tiene abierto
void serialsend(serialib &LS,LACAN_MSG msg){
    uint16_t byte2, byteID1, byteID2;
    //aa cX Bid2 Bid1 Bdat*X 55
    LS.WriteChar(0xaa); //se envia el byte de comienzo del mensaje

    byte2=0xC0 + msg.DLC; //se envia el byte con la indicacion de cantidad de bytes que posee el mensaje
    LS.WriteChar(byte2);

    byteID1=(uint16_t)msg.ID&0xFF;       //La ID del mensaje se manda al reves (comportamiento natural del adaptador)
    byteID2=(uint16_t)(msg.ID>>8)&0x7;
    LS.WriteChar(byteID1);
    LS.WriteChar(byteID2);

    //Se envian los bytes del campo de datos en orden segun sea la longitud del mismo
    if(msg.DLC>0){
        LS.WriteChar(msg.BYTE0);
        if(msg.DLC>1){
            LS.WriteChar(msg.BYTE1);
            if(msg.DLC>2){
                LS.WriteChar(msg.BYTE2);
                if(msg.DLC>3){
                    LS.WriteChar(msg.BYTE3);
                    if(msg.DLC>4){
                        LS.WriteChar(msg.BYTE4);
                        if(msg.DLC>5){
                            LS.WriteChar(msg.BYTE5);
                            if(msg.DLC>6){
                                LS.WriteChar(msg.BYTE6);
                                if(msg.DLC>7)
                                    LS.WriteChar(msg.BYTE7);
                                }
                            }
                        }
                    }
                }
            }
        }
    //Se envia el indicador del final del mensaje
    LS.WriteChar(0x55);

}
//Envia la secuencia de inicializacion del adaptador, se configuran parametros propios de la comunicacion
//En un principio solo se contempla la posible modificacion del baud rate del CAN
void sendinit(serialib &LS,int bdr){

    int i;
    int crc=0xFE;

    LS.WriteChar(0xaa);//dos bytes de cabecera
    LS.WriteChar(0x55);
    LS.WriteChar(0x12);//?
    LS.WriteChar(bdr);//Baud rate, ver opciones
    LS.WriteChar(0x01);//modo ID: standard(0x02 extended)
    /*VER
    LS.WriteChar(0xFF);//Filtro ID
    LS.WriteChar(0x07);//Mask ID*/
    for(i=0;i<12;i++)
        LS.WriteChar(0x00);
    crc=(crc+0xaa+0x55+0x12+bdr+0x1+0xFF+0x07)%0xFF;        //suma modular, checksum para corregir posibles errores
    LS.WriteChar(crc);


}

LACAN_MSG mensaje_recibido(char *pila){
    LACAN_MSG mje;
    mje.DLC=pila[1]&DLC_MASK;//Me quedo unicamente con el DLC (la primer mitad del byte es 0xC)
    //Como los bytes de ID se mandan al reves, tenemos la parte menos significativa del campo de funcion
    //en el primer byte (primeros 3 bits) y la mas significativa en el segundo (ultimos 3 bits)
    uint16_t fun=((pila[2]&BOTTOM_FUN_MASK)>>FUN_MOV_BOTTOM)|((pila[3]&UPPER_FUN_MASK)<<FUN_MOV_UPPER);
    uint16_t source=pila[2]&LACAN_IDENT_MASK;
    mje.ID=(fun<<FUN_MOV_FORSOURCE)|source;//armamos la ID de la forma en la cual esta diseñado CAN para facil entendimiento y utilizacion
    //Se almacenan los datos en la struct diseñada para el mensaje, el switch se comporta como cascada (sin breaks)
    switch(mje.DLC){
    case(8):
        mje.BYTE7=pila[11];
    case(7):
        mje.BYTE6=pila[10];
    case(6):
        mje.BYTE5=pila[9];
    case(5):
        mje.BYTE4=pila[8];
    case(4):
        mje.BYTE3=pila[7];
    case(3):
        mje.BYTE2=pila[6];
    case(2):
        mje.BYTE1=pila[5];
    case(1):
        mje.BYTE0=pila[4];
    }
    return mje;
}

//Se encarga de leer el puerto, busca un nuevo dato, si ReadChar no lo encuentra regresa automaticamente
//Verifica los primeros 12bits para verificar que es un mensaje valido, no se contempla la verificacion del final del mensaje
/*VER si hace falta pasar index_pila, quiza podria ser estatica dentro de readport, haciendo la verificacion del mensaje ahi*/
void readport(char *pila, uint16_t &index_pila,uint16_t &dlc,serialib &LS){
    bool newdataflag;
    if((newdataflag=LS.ReadChar(pila+index_pila))==1){     //devuelve 1 si hay nuevo dato
        index_pila++;
        if(index_pila==1){
            if(pila[0]&0xAA)
                cout<<"\nLlego AA\n";
                //timeout.start();
            else
                index_pila=0;
                //conterror++;
        }
        if(index_pila==2){
            if((pila[1]>>4)&0xC){
                cout<<"Llego b2\n";
                dlc=pila[1]&15;
                //conterror=0;
            }
            else
                index_pila=0;
                //conterror++;
            }
    }
    else
        if(newdataflag=='-1')                   //si es 0 no pasa es pq no hay dato; si es -1 hay un error
            cout<<"\nError en la lectura\n";
}


void openport(serialib &LS,int bdr,char *pila, uint16_t &index_pila){
    char com[]="COM1";
    char nrocom='1';
    char retval;
    int i;
    uint16_t dlc=0;
    bool com_detected=0;
    tiempo wait_portinit;

    for(i=1;i<10;i++){                      //revisamos si esta conectado el adaptador del COM1 al 10
       retval = LS.Open(com,115200); // abrimos el puerto com señalado, retval=1 si hay algo conectado y lo pudo abrir
       if(retval!=1){            //si no devuelve 1 es porque no esta conectado=>aumentamos el nro de com y pasamos al siguiente
            cout<<"en el com "<<com<<" no hay nada\n";
            LS.Close();
            nrocom ++;
            com[3]=nrocom;
        }
        else{                                               //si devuelve 1 es porque esta conectado
            sendinit(LS,bdr); // enviamos secuencia de iniciacion segun un baudrate elegido
            wait_portinit.start(); //se inicializa un timer para esperar una respuesta
            while(wait_portinit.getime()<6000000){
                readport(pila,index_pila,dlc,LS); //leemos el puerto y verificamos que el comienzo del mensaje es valido VER
                //se compara el tamaño en bytes del mensaje con el que deberia tener,
                if(index_pila>=(dlc+5)){
                    /*ver la posibilidad de hacer esto en readport*/
                    if(pila[dlc+4]&0x55){//comprobamos que el mensaje llego entero
                        index_pila=0; //reseteamos variables para volverlas a usar en el proximo mensaje
                        dlc=0;
                    /*hasta aca*/
                        if((pila[4]>>LACAN_BYTE0_RESERVED)==LACAN_ID_BROADCAST){//verificamos que el destino sea broadcast (estamos buscando un HB)
                            LACAN_MSG mje=mensaje_recibido(pila);//armamos el mensaje
                            if((mje.ID>>LACAN_IDENT_BITS)==LACAN_FUN_HB){//si resulta que el mensaje recibido es un HB, se considera que encontramos el puerto correcto
                                cout<<"\nesta en el com "<<com<<"\n";
                                com_detected=1;
                                break; //salgo de while
                            }

                        }
                    }
                }
            }
            //if(com_detected)
            if(1)
                break;       //salgo del for
            else{
                LS.Close();
                cout<<"\nEl el com "<<com<<" hay algo pero no contesta\n";
                nrocom ++;
                com[3]=nrocom;
            }
        }
    }
    if(i>9) //Se considera que Windows no va a asignar un puerto COM por encima del 10
        cout<<"No ta conectado\n";

}


