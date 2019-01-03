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


#define LOG_LIMIT 500

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

     int16_t LACAN_Error(uint16_t errCode);

     int16_t LACAN_Heartbeat();

     int16_t LACAN_Acknowledge(uint16_t code, uint16_t result);

     int16_t LACAN_Post(uint16_t  variable, data_can data);

     int16_t LACAN_Set(uint16_t variable, data_can data, uint8_t show_ack);

     int16_t LACAN_Query(uint16_t variable,uint8_t show_ack);

     int16_t LACAN_Do(uint16_t cmd, uint8_t show_ack);

     void erase_device_ui(uint16_t inactiveDev);

     void add_device_ui(uint16_t reactivatedDev);

     void add_new_device(uint16_t source);

     void change_ERflag(void);

     void agregar_log_sent();

     void agregar_log_rec();

     void LACAN_NOTSUP_Handler(uint16_t source, uint16_t& notsup_count, uint16_t& notsup_gen, uint8_t code);

     int LACAN_Msg_Handler(LACAN_MSG &mje, uint16_t& notsup_count, uint16_t& notsup_gen);

     void LACAN_POST_Handler(uint16_t source,uint16_t variable, uint16_t data);

     void LACAN_ACK_Handler(uint16_t BYTE1);

     void LACAN_HB_Handler(uint16_t source);

     bool device_is_connected(uint8_t id);

signals:
     void postforER_arrived(LACAN_MSG msg);

public slots:

     void handlePortError(QSerialPort::SerialPortError error);

     void add_dev_name(QString);

     void verificarACK();

     void verificarHB();

     void no_ACK_Handler(void); //VER: FALTA IMPLEMENTAR, instrucciones a realizar cuando no se recibe un ack que se esta esperando luego de un tiempo(TIMEOUT_ACK)

     void LACAN_ERR_Handler(uint16_t source,uint16_t err_cod);

private slots:

    void do_stuff();

    void handleRead();

    void on_button_COMANDAR_clicked();

    void on_button_CONSULTAR_clicked();

    void on_button_ENVIAR_MENSAJE_clicked();

    void on_button_ESTADO_RED_clicked();

    void on_button_START_clicked();

    void on_button_STOP_clicked();

    void on_button_ByteSend_clicked();

    void on_pushButton_clicked(bool checked);

    void handleSendTimeout();

private:

    void create_varmap_gen();

    void create_varmap_vol();

    void verificar_destino();//VER

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
    bool NoUSB;
    //VER
    bool gen_connected=true;
    bool vol_connected=false;
    bool boost_connected=false;
    bool show_miss_ack_flag=false;

    uint16_t list_rec_cont = 0;
    uint16_t list_send_cont = 0;
    QMap <QString,LACAN_VAR> varmap_gen;
    QMap <QString,LACAN_VAR> varmap_vol;
    QMap<QString, uint16_t> disp_map;
    HB_CONTROL newdev;
    QTimer* periodicTimer;
    vector<LACAN_MSG*> stack;
};

#endif // MAINWINDOW_H
