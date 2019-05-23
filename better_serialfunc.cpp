#include "better_serialfunc.h"

//Envia un mensaje ya armado por el puerto que se tiene abierto
void serialsend(QSerialPort& serial_port,LACAN_MSG msg){
    char byte2, byteID1, byteID2;
    QByteArray msg_hexa;
    //aa cX Bid2 Bid1 Bdat*X 55
    msg_hexa.append(char(0xAA)); //se agrega el byte de comienzo del mensaje

    byte2 = char(0xC0) + msg.DLC; //se agrega el byte con la indicacion de cantidad de bytes que posee el mensaje
    msg_hexa.append(byte2);

    byteID1 = msg.ID&char(0xFF);       //La ID del mensaje se manda al reves (comportamiento natural del adaptador)
    byteID2 = (msg.ID>>8)&0x7;
    msg_hexa.append(byteID1);
    msg_hexa.append(byteID2);

    //Se agregan los bytes del campo de datos en orden segun sea la longitud del mismo
    if(msg.DLC>0){
        msg_hexa.append(char(msg.BYTE0));
        if(msg.DLC>1){
            msg_hexa.append(char(msg.BYTE1));
            if(msg.DLC>2){
                msg_hexa.append(char(msg.BYTE2));
                if(msg.DLC>3){
                    msg_hexa.append(char(msg.BYTE3));
                    if(msg.DLC>4){
                        msg_hexa.append(char(msg.BYTE4));
                        if(msg.DLC>5){
                            msg_hexa.append(char(msg.BYTE5));
                            if(msg.DLC>6){
                                msg_hexa.append(char(msg.BYTE6));
                                if(msg.DLC>7)
                                    msg_hexa.append(char(msg.BYTE7));
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
void sendinit(QSerialPort& serial_port,uint8_t bdr){
    uint8_t crc=0xFE;
    //suma modular, checksum para corregir posibles errores
    crc=(crc+0xAA+0x55+0x12+bdr+0x01+0xFF+0x07)%0xFF;
    //primeros dos bytes de cabecera
    //4to byte Baud rate, ver opciones
    //5to byte modo ID: standard(0x02 extended)
    //6to y 7mo byte: filtro ID y mask ID 0,0,0,0,0,70,0,0,0,0,0,0,0,0,0,0,0

    QByteArray msg_hexa;
    msg_hexa.append(char(0xAA));
    msg_hexa.append(char(0x55));
    msg_hexa.append(char(0x12));
    msg_hexa.append(char(bdr));
    msg_hexa.append(char(0x01));
    for(int i=0; i<12; i++){
        msg_hexa.append(static_cast<char>(0));//magia de la buena, fuerzo un cast a char de los 0, de otra manera no lo toma o los confunde con un final de string
    }
    msg_hexa.append(char(crc));

    serial_port.write(msg_hexa);
    serial_port.flush();
}

