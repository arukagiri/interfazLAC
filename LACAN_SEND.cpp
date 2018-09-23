#include "LACAN_SEND.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QString>
#include <QDebug>
//Implementacion de todas las funciones involucradas en el envio de mensajes segun del tipo que se requiera
//Cada tipo tiene una cierta cantidad de bytes de datos caracteristica, la cual depende de las funciones del mensaje

int16_t LACAN_Error(MainWindow* mw, uint16_t errCode){
    LACAN_MSG msg;
    //se arma la ID del mensaje mediante el identificador de funcion(6 bits mas significativos) y el de dispositivo(5 bits menos significativos)
    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_ERR<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;  //el LACAN_ID_STANDARD_MASK es para que los 5 bits de la izquierda se pongan en 0
    msg.DLC=2;
    msg.BYTE0=LACAN_ID_BROADCAST << LACAN_BYTE0_RESERVED;//direccion de destino, corrida una cierta cantidad de bits reservados
    msg.BYTE1=errCode;//codigo de error a enviar

    serialsend2(*(mw->serial_port),msg);

    mw->msg_log.push_back(msg);

    return LACAN_SUCCESS; // si el mensaje fue correctamente enviado se devuelve success, ver implementacion de codigos de fracaso
}

//Este tipo de proceso se repite para cada mensaje
int16_t LACAN_Heartbeat(MainWindow* mw){
    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_HB<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=1;
    msg.BYTE0=LACAN_ID_BROADCAST << LACAN_BYTE0_RESERVED;

    serialsend2(*(mw->serial_port),msg);

    mw->msg_log.push_back(msg);

    return LACAN_SUCCESS;
}


/*Requester: destino
RequestType: si es un do, qry set... en principio no lo vamos a usar porque tenemos un object/code para todos
Object: es el codigo
Resultado: xD*/
int16_t LACAN_Acknowledge(MainWindow* mw, uint16_t requestType, uint16_t object, uint16_t result){
    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_ACK<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=3;
    msg.BYTE0=(mw->dest << LACAN_BYTE0_RESERVED)|(requestType & LACAN_BYTE0_RESERVED_MASK);// se usan los bits reservados para enviar el tipo de peticion
    msg.BYTE1=object;
    msg.BYTE2=result;

    serialsend2(*(mw->serial_port),msg);

    mw->msg_log.push_back(msg);

    return LACAN_SUCCESS;
}
/*
int16_t LACAN_Post(MainWindow* mw, uint16_t variable, uint16_t data){
    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_POST<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=3;
    msg.BYTE0=mw->dest << LACAN_BYTE0_RESERVED;
    msg.BYTE1=variable;
    msg.BYTE2=data;

    serialsend2(*(mw->serial_port),msg);

    mw->msg_log.push_back(msg);

    return LACAN_SUCCESS;
}

int16_t LACAN_Set(MainWindow *mw, uint16_t variable, uint16_t data){

    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_SET<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=7;
    msg.BYTE0=(mw->dest) << LACAN_BYTE0_RESERVED;
    msg.BYTE1=mw->code;	//se implementa un codigo en los mensajes que requieren un ack, asi de esta manera poder identificar a que mensaje hacen referencia
    msg.BYTE2=variable;
    msg.BYTE3=data;
    //se considera que no se acumularan nunca 250 mensajes en espera de acknowledge
    if(mw->code>=250)
        mw->code=0;
    else
        mw->code++;

    serialsend2(*(mw->serial_port),msg);

    TIMED_MSG new_msg;
    new_msg.msg=msg;
    new_msg.ack_status=PENDACK;
    new_msg.ack_timer.start(WAIT_ACK_TIME);

    mw->msg_ack.push_back(&new_msg);

    mw->msg_log.push_back(msg);

    return LACAN_SUCCESS;
}*/

int16_t LACAN_Post(MainWindow* mw, uint16_t  variable, data_can data){
    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_POST<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=6;
    msg.BYTE0=mw->dest << LACAN_BYTE0_RESERVED;
    msg.BYTE1=variable;
    msg.BYTE2=data.var_char[0];
    msg.BYTE3=data.var_char[1];
    msg.BYTE4=data.var_char[2];
    msg.BYTE5=data.var_char[3];

    serialsend2(*(mw->serial_port),msg);

    mw->msg_log.push_back(msg);

    return LACAN_SUCCESS;
}


int16_t LACAN_Set(MainWindow *mw, uint16_t variable, data_can data){

    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_SET<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=7;
    msg.BYTE0=(mw->dest) << LACAN_BYTE0_RESERVED;
    msg.BYTE1=mw->code;	//se implementa un codigo en los mensajes que requieren un ack, asi de esta manera poder identificar a que mensaje hacen referencia
    msg.BYTE2=variable;
    msg.BYTE3=data.var_char[0];
    msg.BYTE4=data.var_char[1];
    msg.BYTE5=data.var_char[2];
    msg.BYTE6=data.var_char[3];
    //se considera que no se acumularan nunca 250 mensajes en espera de acknowledge
    if(mw->code>=250)
        mw->code=0;
    else
        mw->code++;

    serialsend2(*(mw->serial_port),msg);

    TIMED_MSG new_msg;
    new_msg.msg=msg;
    new_msg.ack_status=PENDACK;
    new_msg.ack_timer.start(WAIT_ACK_TIME);

    mw->msg_ack.push_back(&new_msg);

    mw->msg_log.push_back(msg);

    return LACAN_SUCCESS;
}


int16_t LACAN_Query(MainWindow* mw, uint16_t variable){
    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_QRY<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=3;
    msg.BYTE0=(mw->dest) << LACAN_BYTE0_RESERVED;
    msg.BYTE1=mw->code;
    msg.BYTE2=variable;

    if(mw->code>=250)
        mw->code=0;
    else
        mw->code++;

    serialsend2(*(mw->serial_port),msg);
    TIMED_MSG* new_msg=new TIMED_MSG();
    new_msg->msg=msg;
    new_msg->ack_status=PENDACK;
    new_msg->ack_timer.setSingleShot(true);
    new_msg->ack_timer.start(WAIT_ACK_TIME);

    mw->msg_ack.push_back(new_msg);
    mw->msg_log.push_back(msg);

    return LACAN_SUCCESS;
}


int16_t LACAN_Do(MainWindow* mw, uint16_t cmd){

    LACAN_MSG msg;

    msg.ID=(LACAN_LOCAL_ID | LACAN_FUN_DO<<LACAN_IDENT_BITS)&LACAN_ID_STANDARD_MASK;
    msg.DLC=3;
    msg.BYTE0=(mw->dest)<< LACAN_BYTE0_RESERVED;
    msg.BYTE1=mw->code;
    msg.BYTE2=cmd;

    if((mw->code)>=250)
        mw->code=0;
    else
        mw->code++;

    serialsend2(*(mw->serial_port),msg);

    TIMED_MSG new_msg;
    new_msg.msg=msg;
    new_msg.ack_status=PENDACK;
    new_msg.ack_timer.start(WAIT_ACK_TIME);
    (mw->msg_ack).push_back(&new_msg);

    (mw->msg_log).push_back(msg);

    return LACAN_SUCCESS;
}
