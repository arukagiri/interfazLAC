#ifndef BETTER_SERIALFUNC_H
#define BETTER_SERIALFUNC_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTextStream>
#include "PC.h"
#include <iostream>

using namespace std;

void serialsend2(QSerialPort& serial_port, LACAN_MSG msg);
void sendinit2(QSerialPort& serial_port,uint8_t bdr);
bool openport2(uint8_t bdr, QSerialPort *serial_port);
LACAN_MSG mensaje_recibido2(char *pila);
//ELIMINAR
bool readport(char *pila, QSerialPort& serial_port);

uint16_t readport2(vector<char> pila, uint16_t *first_byte, QSerialPort& serial_port);

#endif // BETTER_SERIALFUNC_H
