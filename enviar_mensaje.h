#ifndef ENVIAR_MENSAJE_H
#define ENVIAR_MENSAJE_H

#include <QDialog>
#include <stdint.h>
#include "mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class Enviar_Mensaje;
}

class Enviar_Mensaje : public QDialog
{
    Q_OBJECT

public:
    explicit Enviar_Mensaje(QSerialPort &serial_port0,vector <TIMED_MSG*> &msg_ack0,uint8_t &code0,vector <LACAN_MSG> &msg_log0, bool do_log0,uint16_t dest0,QWidget *parent);

    ~Enviar_Mensaje();

private:

    Ui::Enviar_Mensaje *ui;

    QSerialPort *serial_port;

    uint16_t dest;
    MainWindow* mw ;
    uint8_t *code;
    vector <TIMED_MSG*> *msg_ack;
    vector <LACAN_MSG> *msg_log;
    bool do_log;

    //estos de aca abajo podrian hacerse en una sola variable y que se vayan pisando... pero alta paja
    uint16_t var;
    uint16_t cmd;
    uint16_t err_cod;
    uint16_t res;

    void set_TIPO_VAR();


private slots:
    void MENSAJE_changed(void);

    void DO_selected(void);
    void SET_selected(void);
    void ACK_selected(void);
    void ERR_selected(void);
    void QRY_selected(void);
    void POST_selected(void);
    void HB_selected(void);

    void VAR_Changed();
    void TIPO_Changed();
    void CMD_Changed();
    void DEST_Changed();
    void ERR_Changed();
    void RESULT_Changed();

    void on_button_ENVIAR_MENSAJE_clicked();
};

#endif // ENVIAR_MENSAJE_H
