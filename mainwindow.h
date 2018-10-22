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
#include "better_serialfunc.h"

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

     void add_new_device(uint16_t source);

     void change_ERflag(void);

     static void set_item(int,int, QString item);

     void agregar_log_sent();

     void agregar_log_rec(vector <LACAN_MSG> msg_log);

     void verificar_destino();

     void erase_device_ui(uint16_t source);

     void LACAN_NOTSUP_Handler(uint16_t source, uint16_t& notsup_count, uint16_t& notsup_gen, uint8_t code);

     int LACAN_Msg_Handler(LACAN_MSG &mje, vector<HB_CONTROL*>& hb_con, vector<TIMED_MSG*>& msg_ack, uint16_t& notsup_count, uint16_t& notsup_gen, QMap<QString,uint16_t> disp_map, MainWindow *mw);


signals:
     void postforER_arrived(LACAN_MSG msg);

public slots:

     void add_dev_name(QString);

     void verificarACK();

     void verificarHB();

     void no_ACK_Handler(void); //FALTA IMPLEMENTAR, instrucciones a realizar cuando no se recibe un ack que se esta esperando luego de un tiempo(TIMEOUT_ACK)

     void LACAN_ERR_Handler(uint16_t source,uint16_t err_cod);

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

    //bool gen_connected=0;
    bool gen_connected=1;
    bool vol_connected=0;
    bool boost_connected=0;

    QMap <QString,uint16_t> varmap;

    QMap<QString, uint16_t> disp_map;
    HB_CONTROL newdev;
};

#endif // MAINWINDOW_H
