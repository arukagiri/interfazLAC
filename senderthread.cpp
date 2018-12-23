#include "senderthread.h"
#include "better_serialfunc.h"
#include <QApplication>
#include <QDebug>

SenderThread::SenderThread(QObject *parent) : QThread(parent)
{
}

void SenderThread::run(){
    long int timeout=500;   //microsegundos
    senderTimer.start();
    while(true){
        if(senderTimer.getime()>timeout){
            senderTimer.start();
            emit sendTimeout();
        }
    }
}

