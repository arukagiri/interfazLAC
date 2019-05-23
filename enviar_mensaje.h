#ifndef ENVIAR_MENSAJE_H
#define ENVIAR_MENSAJE_H

#include <QDialog>
#include <stdint.h>
#include "mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#define CANTIDAD_DISPOSITIVOS 2

namespace Ui {
class Enviar_Mensaje;
}

class Enviar_Mensaje : public QDialog
{
    Q_OBJECT

public:
    explicit Enviar_Mensaje(QWidget *parent);

    ~Enviar_Mensaje();

private:

    Ui::Enviar_Mensaje *ui;
    MainWindow* mw ;

    //estos de aca abajo podrian hacerse en una sola variable y que se vayan pisando... pero alta paja
    uint16_t var;
    uint16_t cmd;
    uint16_t err_cod;
    uint16_t res;
    uint16_t dest;
    void set_TIPO_VAR();

    QMap<QString,LACAN_VAR> varmap;
    QMap<QString,uint16_t> d_map;

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
    void on_button_ENVIAR_MENSAJE_clicked();
};

#endif // ENVIAR_MENSAJE_H
