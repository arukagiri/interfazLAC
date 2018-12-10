#include "mainwindow.h"
#include <QApplication>
#include "LACAN_REC.h"
#include "LACAN_SEND.h"
#include <better_serialfunc.h>
#include "PC.h"
#include "tiempo.h"
#include "LACAN_PRO.h"
#include "mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "openport.h"
#include "enviador.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    OpenPort* op=new OpenPort();

    enviador envio;

    envio.start();

    op->show();
    /*
    uint16_t bdr=0x05; //baudrate, parte del mensaje de inicializacion del adaptador

    QSerialPort *serial_port = new QSerialPort();

    openport2(bdr,serial_port);
    */

    return a.exec();
}
