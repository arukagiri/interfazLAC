#include "LACAN_PRO.h"


/*void verificarHB(vector<HB_CONTROL*>& hb_con){
    //Encargada de verificar que todos los dispositivos de la red esten activos mediante el HB,
    //cada nodo debe enviar HB cada un cierto tiempo(HB_TIME), si este no se recibe dentro de un periodo de
    //tolerancia (DEAD_HB_TIME), esta funcion se encarga de que el programa lo considere inactivo
    for(vector<HB_CONTROL*>::iterator it_hb=hb_con.begin(); it_hb < hb_con.end(); it_hb++){
        if((it_hb->hb_timer.remainingTime()<= 0) && (it_hb->hb_status==ACTIVE)){
            it_hb->hb_status=INACTIVE;
            cout<<"\nMurio un dispositivo :'( \n";
        }
    }
}


void verificarACK(vector<TIMED_MSG*>& msg_ack){

    //Esta función verifica principalmente que llegue el acknowledge de un mensaje enviado(SET, DO, QUERY), si el mismo
    //no llega luego de un periodo de tiempo(WAIT_ACK_TIME) se setea al mensaje con la condicion ACK_TIMEOUT para que otra
    //parte del programa realice las acciones correspondientes, además un mensaje que recibio su correspondiente acknowledge
    //se sigue almacenando dentro del vector por un tiempo determinado (DEAD_MSJ_ACK_TIME) antes de que sea borrado

    for(vector<TIMED_MSG*>::iterator it_ack=msg_ack.begin();it_ack<msg_ack.end();it_ack++){
        if(it_ack->ack_status==RECEIVED){
            if(it_ack->ack_timer.remainingTime()<=0){
                msg_ack.erase(it_ack);                  //si hace mucho que se mando el mensaje y no se hizo nada lo borramos
            }
        }
        else
        if(it_ack->ack_timer.remainingTime()<=0){
            it_ack->ack_status=ACK_TIMEOUT;
            no_ACK_Handler(); //ver de eliminar el msg despues de procesar esta funcion, o dentro de la misma
        }
    }
}*/


void LACAN_NOTSUP_Handler(uint16_t source, uint16_t& notsup_count, uint16_t& notsup_gen){
    //En el caso de que llegue a la computadora un mensaje que no tiene sentido, como por ejemplo un SET, DO o QUERY,
    //se ejecuta esta funcion para monitorear el flujo de mensajes recibidos no soportados
    notsup_count++;
    if(source&LACAN_ID_GEN)
        notsup_gen++;
}


//Implementacion de las acciones a tomar en caso de que no llegue el ack de un mensaje en el tiempo limite. EN PROCESO
void no_ACK_Handler(void){}
