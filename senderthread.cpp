#include "senderthread.h"
#include "better_serialfunc.h"
#include <QApplication>
#include <QDebug>

#include <iostream>
#include <atomic>
#include <condition_variable>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

std::atomic<int> i{0};

SenderThread::SenderThread(QObject *parent) : QThread(parent)
{
}

//void SenderThread::run(){
//    long int timeout=500;   //microsegundos
//    senderTimer.start();
//    while(true){
//        if(senderTimer.getime()>timeout){
//            senderTimer.start();
//            emit sendTimeout();
//        }
//    }
//}

void SenderThread::run(){
    chrono::microseconds timeout=500us;
    std::condition_variable cv;
    std::mutex cv_m;
    std::unique_lock<std::mutex> lk(cv_m);

    while(true){
        if(!cv.wait_for(lk, timeout, [](){return i==1;}))
            emit sendTimeout();
    }
}
