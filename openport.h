#ifndef OPENPORT_H
#define OPENPORT_H

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class OpenPort;
}

class OpenPort : public QDialog
{
    Q_OBJECT

public:
    explicit OpenPort(QWidget *parent = nullptr);
    ~OpenPort();

private slots:
    void on_autocon_checkBox_stateChanged(int arg1);

    void on_pushButton_clicked();

    void on_update_pushButton_clicked();

private:
    Ui::OpenPort *ui;
};

#endif // OPENPORT_H
