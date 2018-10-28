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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    uint16_t bdr=0x05; //baudrate, parte del mensaje de inicializacion del adaptador

    //uint16_t index_pila=0;
    //char pila[13]={0};
    //uint16_t dlc;

    QSerialPort *serial_port = new QSerialPort();

    openport2(bdr,serial_port);

    MainWindow w(*serial_port);


    w.show();

    return a.exec();
}
