#ifndef CONSULTAR_H
#define CONSULTAR_H
#include "serialib.h"
#include <QDialog>
#include "PC.h"
#include "LACAN_SEND.h"
#include "mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
namespace Ui {
class Consultar;
}

class Consultar : public QDialog
{
    Q_OBJECT

public:
    explicit Consultar(QSerialPort &serial_port0,vector <TIMED_MSG*> &msg_ack0,uint8_t &code0,vector <LACAN_MSG> &msg_log0, bool do_log0, uint16_t consdest0 = 0, QWidget *parent = 0);


    ~Consultar();

private slots:
    void on_button_ENVIAR_QRY_clicked();

    void VARIABLE_CONSULTA_Changed();
    void TIPO_CONSULTA_Changed();

private:
    Ui::Consultar *ui;

    QSerialPort *serial_port;

    uint16_t consdest;
    uint16_t consulta;
    uint8_t *code;
    vector <TIMED_MSG*> *msg_ack;
    vector <LACAN_MSG> *msg_log;
    MainWindow* mw ;
    bool do_log;
    void set_TIPO_CONSULTA();
};

#endif // CONSULTAR_H
