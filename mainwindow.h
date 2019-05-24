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
#include "senderthread.h"
#include "readerthread.h"
#include <deque>

#define LOG_LIMIT 500

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

     int16_t LACAN_Error(uint16_t errCode);

     int16_t LACAN_Heartbeat();

     int16_t LACAN_Acknowledge(uint16_t code, uint16_t result, uint16_t dest);

     int16_t LACAN_Post(uint16_t  variable, data_can data, uint16_t dest);

     int16_t LACAN_Set(uint16_t variable, data_can data, uint8_t show_ack, uint16_t dest);

     int16_t LACAN_Query(uint16_t variable,uint8_t show_ack, uint16_t dest);

     int16_t LACAN_Do(uint16_t cmd, uint8_t show_ack, uint16_t dest);

     void erase_device_ui(uint16_t inactiveDev);

     void add_device_ui(uint16_t reactivatedDev);

     void add_new_device(uint16_t source);

     void change_ERflag(bool);

     void agregar_log_sent();

     void agregar_log_rec();

     void LACAN_NOTSUP_Handler(uint16_t source, uint16_t& notsup_count, uint16_t& notsup_gen, uint8_t code);

     int LACAN_Msg_Handler(LACAN_MSG &mje, uint16_t& notsup_count, uint16_t& notsup_gen);

     void LACAN_POST_Handler(uint16_t source,uint16_t variable, data_can data);

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

     void no_ACK_Handler(void);

     void LACAN_ERR_Handler(uint16_t source,uint16_t err_cod);

private slots:

    void do_stuff();

    void handleProcessedMsg(LACAN_MSG msg);

    void on_button_COMANDAR_clicked();

    void on_button_CONSULTAR_clicked();

    void on_button_ENVIAR_MENSAJE_clicked();

    void on_button_ESTADO_RED_clicked();

    void on_button_ByteSend_clicked();

    void handleSendTimeout();

    void on_logButton_clicked();

    void on_refreshButton_clicked();

    void on_searchBar_textChanged(const QString &filter);

    void on_checkBox_stateChanged(int arg1);

    void on_sent_searchBar_textChanged(const QString &filter);

    void on_received_searchBar_textChanged(const QString &filter);

    void on_pushButton_clicked();

    void on_pushButton_gen_enable_clicked();

    void on_pushButton_gen_disable_clicked();

private:

    void filter_on_sent_searchBar(QString filter);

    void filter_on_rec_searchBar(QString filter);

    void create_varmap_broadcast();

    void create_varmap_gen();

    void create_varmap_vol();

    uint16_t verificar_destino();

    uint16_t cmd_enable;

public:
    ReaderThread* readerth;
    SenderThread* msgSender;
    QSerialPort *serial_port;
    //uint16_t dest;
    vector <LACAN_MSG> msg_log;
    uint8_t code=0;
    vector <TIMED_MSG*> msg_ack;
    int outlog_cont;
    int inlog_cont;
    bool do_log;
    vector <HB_CONTROL*> hb_con;
    bool ERflag;
    bool NoUSB;
    bool show_miss_ack_flag=false;
    bool filter_both_lists;

    uint16_t list_rec_cont = 0;
    uint16_t list_send_cont = 0;
    QMap <QString,LACAN_VAR> varmap_gen;
    QMap <QString,LACAN_VAR> varmap_vol;
    QMap <QString,LACAN_VAR> varmap_broad;
    QMap<QString, uint16_t> disp_map;
    HB_CONTROL newdev;
    QTimer* periodicTimer;
    deque<LACAN_MSG*> stack;
};

#endif // MAINWINDOW_H
