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
    vector<LACAN_MSG*> stack;
    MainWindow* mw;
    tiempo senderTimer;
signals:

public slots:
    void loadNewMsg(LACAN_MSG*);
};

#endif // SENDERTHREAD_H
