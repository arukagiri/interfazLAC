#ifndef SENDERTHREAD_H
#define SENDERTHREAD_H

#include <QObject>
#include <QThread>
#include <vector>
#include "PC.h"
#include "mainwindow.h"
#include "tiempo.h"

using namespace std;

class SenderThread : public QThread
{
    Q_OBJECT
public:
    explicit SenderThread(QObject *parent = nullptr);
    virtual void run() override;
    tiempo senderTimer;
signals:
    void sendTimeout();
};

#endif // SENDERTHREAD_H
