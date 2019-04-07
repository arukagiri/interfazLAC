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
//Si la otra no anda usar esta bajo el conocimiento de que BLOQUEA UN NUCLEO DEL PROCESADOR
//void SenderThread::run(){
//    uint timeout_uint = 1000000/((CAN_BAUD/8)/BIGGEST_CAN_MSG); //Tiempo minimo necesario del lado del CAN para enviar el mensaje mas largo (en us)
//    uint timeout_rounded = timeout_uint+100; //Se otorga un margen de 100us
//    senderTimer.start();
//    while(true){
//        if(senderTimer.getime()>timeout_rounded){
//            senderTimer.start();
//            emit sendTimeout();
//        }
//    }
//}

// Ni se te ocurra tocar esto
//void SenderThread::run(){
//    uint timeout_uint = 1000000/((CAN_BAUD/8)/BIGGEST_CAN_MSG); //Tiempo necesario del lado del CAN para enviar el mensaje mas largo (en us)
//    uint timeout_rounded = timeout_uint+100; //Se otorga un margen de 100us
//    chrono::microseconds timeout = chrono::microseconds(timeout_rounded);
//    std::condition_variable cv;
//    std::mutex cv_m;
//    std::unique_lock<std::mutex> lk(cv_m);

//    while(true){
//        if(!cv.wait_for(lk, timeout, [](){return i==1;}))
//            emit sendTimeout();
//    }
//}

void SenderThread::run(){
    uint timeout_uint = 1000000/((CAN_BAUD/8)/BIGGEST_CAN_MSG); //Tiempo minimo necesario del lado del CAN para enviar el mensaje mas largo (en us)
    uint timeout_rounded = timeout_uint+100; //Se otorga un margen de 100us
    chrono::microseconds timeout = chrono::microseconds(timeout_rounded);
    std::condition_variable cv;
    std::mutex cv_m;
    std::unique_lock<std::mutex> lk(cv_m);

    while(true){
        if(!cv.wait_for(lk, timeout, [this](){return send==0;}))
            emit sendTimeout();
    }
}
