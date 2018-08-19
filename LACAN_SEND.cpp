#include "LACAN_SEND.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QString>
//Implementacion de todas las funciones involucradas en el envio de mensajes segun del tipo que se requiera
//Cada tipo tiene una cierta cantidad de bytes de datos caracteristica, la cual depende de las funciones del mensaje

int16_t LACAN_Error(QSerialPort& serial_port, uint16_t errCode, vector<LACAN_MSG>& msg_log){
    LACAN_MSG msg;
    //se arma la ID del mensaje mediante el identificador de funcion(6 bits mas significativos) y el de dispositivo(5 bits menos significativos)
    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_ERR<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;  //el LACAN_ID_STANDARD_MASK es para que los 5 bits de la izquierda se pongan en 0
    msg.DLC=2;
    msg.BYTE0=LACAN_ID_BROADCAST << LACAN_BYTE0_RESERVED;//direccion de destino, corrida una cierta cantidad de bits reservados
    msg.BYTE1=errCode;//codigo de error a enviar

    serialsend2(serial_port,msg);

    msg_log.push_back(msg);

    return LACAN_SUCCESS; // si el mensaje fue correctamente enviado se devuelve success, ver implementacion de codigos de fracaso
}

//Este tipo de proceso se repite para cada mensaje
int16_t LACAN_Heartbeat(QSerialPort& serial_port, vector<LACAN_MSG>& msg_log){
    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_HB<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=1;
    msg.BYTE0=LACAN_ID_BROADCAST << LACAN_BYTE0_RESERVED;

    serialsend2(serial_port,msg);

    msg_log.push_back(msg);

    return LACAN_SUCCESS;
}


/*Requester: destino
RequestType: si es un do, qry set... en principio no lo vamos a usar porque tenemos un object/code para todos
Object: es el codigo
Resultado: xD*/
int16_t LACAN_Acknowledge(QSerialPort& serial_port, uint16_t requester, uint16_t requestType, uint16_t object, uint16_t result, vector<LACAN_MSG>& msg_log){
    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_ACK<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=3;
    msg.BYTE0=(requester << LACAN_BYTE0_RESERVED)|(requestType & LACAN_BYTE0_RESERVED_MASK);// se usan los bits reservados para enviar el tipo de peticion
    msg.BYTE1=object;
    msg.BYTE2=result;

    serialsend2(serial_port,msg);

    msg_log.push_back(msg);

    return LACAN_SUCCESS;
}

int16_t LACAN_Post(QSerialPort& serial_port, uint16_t destino, uint16_t variable, uint16_t data, vector<LACAN_MSG>& msg_log){
    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_POST<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=3;
    msg.BYTE0=destino << LACAN_BYTE0_RESERVED;
    msg.BYTE1=variable;
    msg.BYTE2=data;

    serialsend2(serial_port,msg);

    msg_log.push_back(msg);

    return LACAN_SUCCESS;
}


int16_t LACAN_Set(QSerialPort& serial_port, uint16_t destino, uint16_t variable, uint16_t data, uint8_t &msg_cod, vector<TIMED_MSG*>& msg_ack,vector<LACAN_MSG>& msg_log){

    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_SET<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=4;
    msg.BYTE0=destino << LACAN_BYTE0_RESERVED;
    msg.BYTE1=msg_cod;	//se implementa un codigo en los mensajes que requieren un ack, asi de esta manera poder identificar a que mensaje hacen referencia
    msg.BYTE2=variable;
    msg.BYTE3=data;
    //se considera que no se acumularan nunca 250 mensajes en espera de acknowledge
    if(msg_cod>=250)
        msg_cod=0;
    else
        msg_cod++;

    serialsend2(serial_port,msg);

    TIMED_MSG new_msg;
    new_msg.msg=msg;
    new_msg.ack_status=PENDACK;
    new_msg.ack_timer.start(WAIT_ACK_TIME);
    msg_ack.push_back(&new_msg);


    msg_log.push_back(msg);

    return LACAN_SUCCESS;
}


int16_t LACAN_Query(QSerialPort& serial_port, uint16_t destino, uint16_t variable, uint8_t &msg_cod, vector<TIMED_MSG*>& msg_ack,vector<LACAN_MSG>& msg_log){
    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_QRY<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=3;
    msg.BYTE0=destino << LACAN_BYTE0_RESERVED;
    msg.BYTE1=msg_cod;
    msg.BYTE2=variable;

    if(msg_cod>=250)
        msg_cod=0;
    else
        msg_cod++;

    serialsend2(serial_port,msg);

    TIMED_MSG new_msg;
    new_msg.msg=msg;
    new_msg.ack_status=PENDACK;
    new_msg.ack_timer.start(WAIT_ACK_TIME);
    msg_ack.push_back(&new_msg);


    //if(start=0)
    msg_log.push_back(msg);



    //if(seralsend()no mando ningun error)


    return LACAN_SUCCESS;
}


int16_t LACAN_Do(QSerialPort& serial_port, uint16_t destino, uint16_t comando, uint8_t &msg_cod, vector<TIMED_MSG*>& msg_ack, vector<LACAN_MSG>& msg_log){

    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_DO<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=3;
    msg.BYTE0=destino << LACAN_BYTE0_RESERVED;
    msg.BYTE1=msg_cod;
    msg.BYTE2=comando;

    if(msg_cod>=250)
        msg_cod=0;
    else
        msg_cod++;

    serialsend2(serial_port,msg);

    TIMED_MSG new_msg;
    new_msg.msg=msg;
    new_msg.ack_status=PENDACK;
    new_msg.ack_timer.start(WAIT_ACK_TIME);
    msg_ack.push_back(&new_msg);

    msg_log.push_back(msg);

    return LACAN_SUCCESS;
}
