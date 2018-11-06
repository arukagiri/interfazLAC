#include "openport.h"
#include "ui_openport.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include "better_serialfunc.h"

OpenPort::OpenPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenPort)
{
    ui->setupUi(this);
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){//revisamos los puertos habilitados
        ui->ports_comboBox->addItem(info.portName());
    }
    ui->autocon_checkBox->setWhatsThis("Habilita la posibilidad de realizar la conexion automaticamente, "
                                       "\nsiendo el adaptador el unico dispositivo conectado y que no se encuentra abierto");
}

OpenPort::~OpenPort()
{
    delete ui;
}

void OpenPort::on_autocon_checkBox_stateChanged(int arg)
{
    if(arg==Qt::Checked){
        ui->ports_comboBox->setEnabled(false);
        ui->update_pushButton->setEnabled(false);
    }else{
        ui->ports_comboBox->setEnabled(true);
        ui->update_pushButton->setEnabled(true);
    }
}

void OpenPort::on_pushButton_clicked()
{
    QSerialPort* serial_port;
    int retval=false;
    if(ui->autocon_checkBox->isChecked()){
        //sale del foreach con retval=1 solo si encontro un puerto que se puede abrir
        foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){//revisamos los puertos habilitados
            serial_port=new QSerialPort(info);
            serial_port->setBaudRate(QSerialPort::Baud115200);
            serial_port->setDataBits(QSerialPort::Data8);
            serial_port->setParity(QSerialPort::NoParity);
            serial_port->setStopBits(QSerialPort::OneStop);
            serial_port->setFlowControl(QSerialPort::NoFlowControl);
            retval = serial_port->open(QSerialPort::ReadWrite);
            if(retval){
               break;
            }
        }
    }else{
        serial_port=new QSerialPort(ui->ports_comboBox->currentText());
        serial_port->setBaudRate(QSerialPort::Baud115200);
        serial_port->setDataBits(QSerialPort::Data8);
        serial_port->setParity(QSerialPort::NoParity);
        serial_port->setStopBits(QSerialPort::OneStop);
        serial_port->setFlowControl(QSerialPort::NoFlowControl);
        retval = serial_port->open(QSerialPort::ReadWrite);
    }
    uint8_t bdr=0x05;
    sendinit2(*serial_port,bdr);

    if(!retval){
        int ret = QMessageBox::warning(this, "Ups",
                                       "No se pudo conectar con el puerto",
                                       QMessageBox::Ok);
    }
    this->close();
    MainWindow* mw=new MainWindow(*serial_port);
    mw->show();
}



void OpenPort::on_update_pushButton_clicked()
{
    ui->ports_comboBox->clear();
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){//revisamos los puertos habilitados
        ui->ports_comboBox->addItem(info.portName());
    }
}
