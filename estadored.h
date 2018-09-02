#ifndef ESTADORED_H
#define ESTADORED_H

#include <QDialog>
#include "PC.h"
#include "mainwindow.h"
#include <QTime>

namespace Ui {
class EstadoRed;
}

class EstadoRed : public QDialog
{
    Q_OBJECT

public:
    explicit EstadoRed(QSerialPort &serial_port0,vector <TIMED_MSG*> &msg_ack0,uint8_t &code0,vector <LACAN_MSG> &msg_log0, bool do_log0,QWidget *parent);
    ~EstadoRed();
protected:
    virtual void closeEvent(QCloseEvent *e) override;
signals:
    void postforGEN_arrived(LACAN_MSG msg);

private slots:
    void ERpost_Handler(LACAN_MSG msg);

    void on_button_gen_clicked();

    void timer_handler();

    void var_changed(uint16_t var,uint16_t data);

private:
    Ui::EstadoRed *ui;

    QSerialPort *serial_port;

    uint8_t *code;
    vector <TIMED_MSG*> *msg_ack;
    vector <LACAN_MSG> *msg_log;
    bool do_log;
    MainWindow* mw ;
    QTimer *time_2sec;

    float gen_v;
    float gen_i;
    float boost_v;
    float boost_i;
    float vol_v;
    float vol_i;

    void refresh_values(void);
    void send_qry(void);



};

#endif // ESTADORED_H
