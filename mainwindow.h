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
#include <QMap>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QSerialPort &serial_port0,QWidget *parent = nullptr);
    ~MainWindow();

     Ui::MainWindow *ui;

     void add_new_device(uint16_t source);

     void change_ERflag(void);

     static void set_item(int,int, QString item);

     void agregar_log_sent(vector <LACAN_MSG> msg_log);

     void agregar_log_rec(vector <LACAN_MSG> msg_log);

     void verificar_destino();

signals:
     void postforER_arrived(LACAN_MSG msg);

public slots:

     void add_dev_name(QString);

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

private:
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
    QMap<QString, uint16_t> disp_map;
    HB_CONTROL newdev;
};

#endif // MAINWINDOW_H
