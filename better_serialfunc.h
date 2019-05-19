#ifndef BETTER_SERIALFUNC_H
#define BETTER_SERIALFUNC_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTextStream>
#include "PC.h"
#include <iostream>

using namespace std;

void serialsend(QSerialPort& serial_port, LACAN_MSG msg);
void sendinit(QSerialPort& serial_port,uint8_t bdr);

#endif // BETTER_SERIALFUNC_H
