#ifndef LACAN_PRO_H_INCLUDED
#define LACAN_PRO_H_INCLUDED
#include <iostream>
#include <vector>
#include "PC.h"

#define HB_TIME 5000000                 //en microsegundos(5 seg), es el periodo en el cual los integrantes de la red deben enviar sus HB
#define DEAD_HB_TIME HB_TIME*2+500000   //tiempo que debe transcurrir desde el ultimo HB para considerar un nodo inactivo (10.5 seg)
#define DEAD_MSJ_ACK_TIME 30000000      //tiempo para borrar del vector un mensaje desde que recibio su correspondiente ack
#define WAIT_ACK_TIME 500000            //tiempo de espera un ack

using namespace std;

void verificarHB(vector<HB_CONTROL>& hb_con);//verifica los timers activos de HB para mantener un mapa de dispositivos conectados
void verificarACK(vector<TIMED_MSG>& msg_ack);//verifica los ack de mensajes pendientes(a los cuales tiene sentido el ack)
void no_ACK_Handler(void);//FALTA IMPLEMENTAR, instrucciones a realizar cuando no se recibe un ack que se esta esperando luego de un tiempo(TIMEOUT_ACK)
void LACAN_NOTSUP_Handler(uint16_t source, uint16_t& notsup_count, uint16_t& notsup_gen);


#endif // LACAN_PRO_H_INCLUDED
