#include "openport.h"
#include "ui_openport.h"
#include "mainwindow.h"
#include <QMessageBox>
#include "better_serialfunc.h"

OpenPort::OpenPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenPort)
{
    ui->setupUi(this);

    this->setWindowTitle("Config COM");
    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

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
    QSerialPort* serial_port=new QSerialPort();
    int retval=false;
    if(ui->autocon_checkBox->isChecked()){
        //sale del foreach con retval=1 solo si encontro un puerto que se puede abrir
        foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){//revisamos los puertos habilitados
            serial_port=new QSerialPort(info);
            serial_port->setBaudRate(SERIAL_BAUD);
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
        serial_port->setBaudRate(SERIAL_BAUD);;
        serial_port->setDataBits(QSerialPort::Data8);
        serial_port->setParity(QSerialPort::NoParity);
        serial_port->setStopBits(QSerialPort::OneStop);
        serial_port->setFlowControl(QSerialPort::NoFlowControl);
        retval = serial_port->open(QSerialPort::ReadWrite);
    }
    uint8_t bdr=CAN_BAUD_HEXA; //250Kbps
    //uint8_t bdr2=0x09; //50Kbps
    sendinit(*serial_port,bdr);

    this->close();

    if(!retval){
        QMessageBox::warning(this, "Ups",
                                       "No se pudo conectar con el puerto",
                                       QMessageBox::Ok);
        //return;
    }

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
