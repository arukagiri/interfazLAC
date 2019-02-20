#ifndef CONSULTAR_H
#define CONSULTAR_H
#include <QDialog>
#include "PC.h"
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
    explicit Consultar(QWidget *parent, uint16_t dest);


    ~Consultar();

private slots:
    void on_button_ENVIAR_QRY_clicked();

private:
    Ui::Consultar *ui;
    uint16_t consulta;
    MainWindow* mw ;
    uint16_t dest;

    QMap <QString,LACAN_VAR> varmap;
};

#endif // CONSULTAR_H
