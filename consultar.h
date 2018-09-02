#ifndef CONSULTAR_H
#define CONSULTAR_H
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
    explicit Consultar(QWidget *parent = 0);


    ~Consultar();

private slots:
    void on_button_ENVIAR_QRY_clicked();

    void VARIABLE_CONSULTA_Changed();
    void TIPO_CONSULTA_Changed();

private:
    Ui::Consultar *ui;
    uint16_t consulta;
    MainWindow* mw ;
    void set_TIPO_CONSULTA();
};

#endif // CONSULTAR_H
