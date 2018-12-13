#include "senderthread.h"
#include "better_serialfunc.h"
#include <QApplication>
#include <QDebug>

SenderThread::SenderThread(QObject *parent) : QThread(parent)
{
    mw = qobject_cast<MainWindow*>(this->parent());
    senderTimer.start();
}

void SenderThread::run(){
    long int timeout=500;
    int cont=0;
    while(true){
        qApp->processEvents();
        if(stack.size()!=0 && senderTimer.getime()>timeout){
            cont++;
            if(cont>59999){
                qDebug()<<"TERMINOO";
            }
            senderTimer.start();
            //serialsend2(*(mw->serial_port),*(stack.back()));
            stack.pop_back();
        }
    }
}

void SenderThread::loadNewMsg(LACAN_MSG* newMsg){
    stack.push_back(newMsg);
}
