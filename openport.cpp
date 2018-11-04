#include "openport.h"
#include "ui_openport.h"
#include "mainwindow.h"
#include <QMessageBox>

OpenPort::OpenPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenPort)
{
    ui->setupUi(this);
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){//revisamos los puertos habilitados
        ui->ports_comboBox->addItem(info.portName());
    }
}

OpenPort::~OpenPort()
{
    delete ui;
}

void OpenPort::on_autocon_checkBox_stateChanged(int arg)
{
    if(arg==Qt::Checked){
        ui->ports_comboBox->setEnabled(false);
    }else{
        ui->ports_comboBox->setEnabled(true);
    }
}

void OpenPort::on_pushButton_clicked()
{
    QSerialPort* serial_port=new QSerialPort(ui->ports_comboBox->currentText());
    serial_port->setBaudRate(QSerialPort::Baud115200);
    serial_port->setDataBits(QSerialPort::Data8);
    serial_port->setParity(QSerialPort::NoParity);
    serial_port->setStopBits(QSerialPort::OneStop);
    serial_port->setFlowControl(QSerialPort::NoFlowControl);
    int retval = serial_port->open(QSerialPort::ReadWrite);
    if(!retval){
        int ret = QMessageBox::warning(this, "",
                                       "",
                                       QMessageBox::Ok);
    }
    this->close();
    MainWindow* mw=new MainWindow(*serial_port);
    mw->show();
}
