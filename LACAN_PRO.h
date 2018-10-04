#ifndef LACAN_PRO_H_INCLUDED
#define LACAN_PRO_H_INCLUDED
#include <iostream>
#include <vector>
#include "PC.h"

using namespace std;

//void verificarHB(vector<HB_CONTROL*>& hb_con);//verifica los timers activos de HB para mantener un mapa de dispositivos conectados
//void verificarACK(vector<TIMED_MSG*>& msg_ack);//verifica los ack de mensajes pendientes(a los cuales tiene sentido el ack)
void LACAN_NOTSUP_Handler(uint16_t source, uint16_t& notsup_count, uint16_t& notsup_gen);


#endif // LACAN_PRO_H_INCLUDED
