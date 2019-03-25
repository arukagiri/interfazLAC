#ifndef READERTHREAD_H
#define READERTHREAD_H

#include <QObject>
#include <QThread>
#include "PC.h"
#include <QSerialPort>

class ReaderThread : public QThread
{
    Q_OBJECT
    QSerialPort* thread_serial_port;
public:
    ReaderThread(QSerialPort& serial_port);
    //virtual void run() override;
signals:
    void receivedMsg(LACAN_MSG msg);
public slots:
    void handleRead();
};

#endif // READERTHREAD_H
