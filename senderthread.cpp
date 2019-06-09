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

//El nombre "SenderThread" es parcialmente correcto, mediante este thread se coordina
//el envio de mensajes pendientes, pero realmente (por cuestiones de precision y ejecucion
//de un loop parcialmente bloqueante) funciona unicamente de timer de precision, con el cual
//la MainWindow puede enviar los mensajes pendientes

std::atomic<int> i{0};

SenderThread::SenderThread(QObject *parent) : QThread(parent)
{
}

void SenderThread::run(){
    mustRun=true;
    double timeout_uint = 1000000/((CAN_BAUD/8)/BIGGEST_CAN_MSG); //Tiempo minimo necesario del lado del CAN para enviar el mensaje mas largo (en us)
    uint timeout_rounded = uint(timeout_uint*1.1); //Se otorga un margen del 10%

    chrono::microseconds timeout = chrono::microseconds(timeout_rounded);
    std::condition_variable cv;
    std::mutex cv_m;
    std::unique_lock<std::mutex> lk(cv_m);

    while(mustRun){
        //Mediante una condicion variable logramos que este loop no sea completamente bloqueante y asi evitar un consumo
        //excesivo de CPU. Tener en cuenta que este nivel de manejo de threads es complicado, es decir, si no esta
        //completamente seguro, NO TOQUE LA SIGUIENTE LINEA
        if(!cv.wait_for(lk, timeout, [this](){return send==0;})){
            QCoreApplication::processEvents();//Se obliga a procesar señales en caso de que se quiera cambiar la bandera MustRun desde afuera
            emit sendTimeout();//Se envia la señal utilizada para vaciar la lista de mensajes
        }
    }
}

//Utilizada para detener el thread al final de la ejecucion del programa, para poder asi borrarlo
void SenderThread::changeMustRun(bool stop){
    mustRun=!stop;
}
