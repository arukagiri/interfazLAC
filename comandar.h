#ifndef COMANDAR_H
#define COMANDAR_H

#include <QDialog>
#include "mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include "stdint.h"
#include <QString>

namespace Ui {
class Comandar;
}

class Comandar : public QDialog
{
    Q_OBJECT

public:
    explicit Comandar(QWidget *parent);
    //explicit Comandar(uint16_t destino, QWidget *parent);
    ~Comandar();

private:
    Ui::Comandar *ui;
    uint16_t var_set;
    uint16_t cmd;
    uint16_t mode_set;
    MainWindow* mw;
    void set_tipo();

    QMap <QString,LACAN_VAR> varmap;

private slots:
    void DO_selected(void);
    void SET_selected(void);
    void on_button_ENVIAR_clicked();
    void SET_VAR_Changed();
    void SET_TIPO_Changed();
    void DO_CMD_Changed();
};

#endif // COMANDAR_H
