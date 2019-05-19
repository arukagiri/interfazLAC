#include "senderthread.h"
#include "better_serialfunc.h"
#include <QApplication>
#include <iostream>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <chrono>
#include "PC.h"

using namespace std::chrono_literals;

std::atomic<int> i{0};

SenderThread::SenderThread(QObject *parent) : QThread(parent)
{
}

void SenderThread::run(){
    double timeout_uint = 1000000/((CAN_BAUD/8)/BIGGEST_CAN_MSG); //Tiempo minimo necesario del lado del CAN para enviar el mensaje mas largo (en us)
    uint timeout_rounded = uint(timeout_uint*1.1); //Se otorga un margen del 10%

    chrono::microseconds timeout = chrono::microseconds(timeout_rounded);
    std::condition_variable cv;
    std::mutex cv_m;
    std::unique_lock<std::mutex> lk(cv_m);

    while(true){
        if(!cv.wait_for(lk, timeout, [this](){return send==0;}))
            emit sendTimeout();
    }
}
