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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    OpenPort* op=new OpenPort();

    op->show();

    return a.exec();
}
