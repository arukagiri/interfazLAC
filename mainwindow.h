#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QString>
#include "PC.h"
#include <vector>
#include <iostream>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QSerialPort &serial_port0,QWidget *parent = 0);
    ~MainWindow();

     Ui::MainWindow *ui;

     void change_ERflag(void);

     static void set_item(int,int, QString item);

     void agregar_log_sent();

     void agregar_log_rec(vector <LACAN_MSG> msg_log);

     QTimer *t1;


signals:
     void postforER_arrived(LACAN_MSG msg);

public slots:
     //TEST ONLY
     void t1_Handler();
     //<---

     void verificarACK();

     void verificarHB();

private slots:

    void handleRead();

    void on_button_COMANDAR_clicked();

    void on_button_CONSULTAR_clicked();

    void on_button_ENVIAR_MENSAJE_clicked();

    void on_button_ESTADO_RED_clicked();

    void on_button_START_clicked();

    void on_button_STOP_clicked();

public:

    QSerialPort *serial_port;
    uint16_t dest;
    vector <LACAN_MSG> msg_log;
    uint8_t code=0;
    vector <TIMED_MSG*> msg_ack;
    int outlog_cont;
    int inlog_cont;
    bool do_log;
    vector <HB_CONTROL*> hb_con;
    bool ERflag;

};

#endif // MAINWINDOW_H
