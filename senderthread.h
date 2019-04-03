#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QObject>
#include <QThread>
#include <vector>
#include "PC.h"
#include "tiempo.h"

using namespace std;

class SenderThread : public QThread
{
    Q_OBJECT
public:
    explicit SenderThread(QObject *parent);
    virtual void run() override;
    tiempo senderTimer;
    std::atomic<int> send{1}; //Cambiar esta variable a 0 provoca que el thread deje de señalizar el envio de mensajes
signals:
    void sendTimeout();
};

#endif // SENDERTHREAD_H
