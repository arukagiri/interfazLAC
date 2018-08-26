#ifndef COMANDAR_H
#define COMANDAR_H

#include <QDialog>
#include "mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class Comandar;
}

class Comandar : public QDialog
{
    Q_OBJECT

public:
    explicit Comandar(QSerialPort &serial_port0,vector <TIMED_MSG*> &msg_ack0,uint8_t &code0,vector <LACAN_MSG> &msg_log0, bool do_log0,uint16_t consdest0,QWidget *parent);
    ~Comandar();

private:
    Ui::Comandar *ui;
    QSerialPort *serial_port;

    uint16_t comdest;
    uint16_t var_set;
    uint16_t cmd;
    MainWindow* mw ;
    uint8_t *code;
    vector <TIMED_MSG*> *msg_ack;
    vector <LACAN_MSG> *msg_log;
    bool do_log;
    void set_tipo();

private slots:
    void DO_selected(void);
    void SET_selected(void);
    void on_button_ENVIAR_clicked();
    void SET_VAR_Changed();
    void SET_TIPO_Changed();
    void DO_CMD_Changed();
};

#endif // COMANDAR_H
