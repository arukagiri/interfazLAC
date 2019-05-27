#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QObject>
#include <QThread>
#include <vector>
#include "PC.h"

using namespace std;

class SenderThread : public QThread
{
    Q_OBJECT
public:
    explicit SenderThread(QObject *parent = 0);
    virtual void run() override;
    std::atomic<int> send{1}; //Cambiar esta variable a 0 provoca que el thread deje de señalizar el envio de mensajes
    bool mustRun = false;
signals:
    void sendTimeout();
public slots:
    void changeMustRun(bool stop);
};

#endif // SENDERTHREAD_H
