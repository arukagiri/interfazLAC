#ifndef BETTER_SERIALFUNC_H
#define BETTER_SERIALFUNC_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTextStream>
#include "PC.h"

void serialsend2(QSerialPort& serial_port, LACAN_MSG msg);
void sendinit2(QSerialPort& serial_port,uint8_t bdr);
void openport2(uint8_t bdr, QSerialPort *serial_port);
LACAN_MSG mensaje_recibido2(char *pila);
bool readport2(char *pila, QSerialPort& serial_port);

#endif // BETTER_SERIALFUNC_H
