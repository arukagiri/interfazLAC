#ifndef LACAN_REC_H_INCLUDED
#define LACAN_REC_H_INCLUDED
#include <iostream>
#include "PC.h"
#include <vector>
#include "LACAN_PRO.h"
#include <stdint.h>
#include <QMap>

using namespace std;

int LACAN_Msg_Handler(LACAN_MSG &mje, vector<HB_CONTROL*>& hb_con, vector<TIMED_MSG*>& msg_ack, uint16_t& notsup_count, uint16_t& notsup_gen, QMap<QString,uint16_t> disp_map);
void LACAN_HB_Handler(uint16_t source, vector<HB_CONTROL*>& hb_con, QMap<QString,uint16_t> disp_map);
void LACAN_ACK_Handler(uint16_t BYTE1, vector<TIMED_MSG*>& msg_ack);

void LACAN_POST_Handler(uint16_t source,uint16_t variable, uint16_t data);


#endif // LACAN_REC_H_INCLUDED
