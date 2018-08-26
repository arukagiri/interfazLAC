#ifndef LACAN_SEND_H_INCLUDED
#define LACAN_SEND_H_INCLUDED
#include <vector>
#include "PC.h"
#include <iostream>
#include <stdint.h>
#include "better_serialfunc.h"

using namespace std;

int16_t LACAN_Set(QSerialPort& serial_port, uint16_t destino, uint16_t variable, uint16_t data, uint8_t &msg_cod, vector<TIMED_MSG*>& msg_ack, vector<LACAN_MSG>& msg_log);
int16_t LACAN_Do(QSerialPort& serial_port, uint16_t destino, uint16_t comando, uint8_t &msg_cod, vector<TIMED_MSG*>& msg_ack, vector<LACAN_MSG>& msg_log);
int16_t LACAN_Post(QSerialPort& serial_port, uint16_t destino, uint16_t variable, uint16_t data, vector<LACAN_MSG>& msg_log);
int16_t LACAN_Query(QSerialPort& serial_port, uint16_t destino, uint16_t variable, uint8_t &msg_cod, vector<TIMED_MSG*>& msg_ack, vector<LACAN_MSG>& msg_log);
int16_t LACAN_Heartbeat(QSerialPort& serial_port,vector<LACAN_MSG>& msg_log);
int16_t LACAN_Error(QSerialPort& serial_port, uint16_t errCode, vector<LACAN_MSG>& msg_log);
int16_t LACAN_Acknowledge(QSerialPort& serial_port
                          , uint16_t requester, uint16_t requestType, uint16_t object, uint16_t result, vector<LACAN_MSG>& msg_log);


#endif // LACAN_SEND_H_INCLUDED
