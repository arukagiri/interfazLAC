#include "consultar.h"
#include "ui_consultar.h"
#include "LACAN_SEND.h"
#include "PC.h"
#include <QDebug>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

enum VARIABLES {II, IO, ISD, IEF, PI, PO, VI, VO, W, MOD};
enum TIPO_VAR {MAX,MIN,SETP,INST};
enum TIPO_MOD {MOD_P, MOD_V, MOD_T};


Consultar::Consultar(QWidget *parent) :

    QDialog(parent),
    ui(new Ui::Consultar)
{
    ui->setupUi(this);
    this->setWindowTitle("Consultar");
    //this->setFixedSize(ui->verticalLayout_2->sizeHint());
    //this->setLayout(ui->verticalLayout_2);

    mw = qobject_cast<MainWindow*>(this->parent());


    switch(mw->dest){
    case LACAN_ID_GEN:
        ui->label_DESTINO_QRY->setText("Generador Eolico");

        varmap = mw->varmap_gen;

        ui->list_VARIABLE_QRY->addItem("Potencia de Salida");
        ui->list_VARIABLE_QRY->addItem("Velocidad Angular");
        ui->list_VARIABLE_QRY->addItem("Torque");
        ui->list_VARIABLE_QRY->addItem("Tension de Salida");
        ui->list_VARIABLE_QRY->addItem("Corriente de ISD");
        ui->list_VARIABLE_QRY->addItem("Corriente Eficaz");
        ui->list_VARIABLE_QRY->addItem("Corriente de Bateria");
        ui->list_VARIABLE_QRY->addItem("Modo");

        consulta=LACAN_VAR_PO_SETP;

        break;

    case LACAN_ID_BOOST:
        ui->label_DESTINO_QRY->setText("Boost");

        break;
    case LACAN_ID_VOLANTE:
        ui->label_DESTINO_QRY->setText("Volante de Inercia");

        break;
    case LACAN_ID_BROADCAST:
        ui->label_DESTINO_QRY->setText("Broadcast");
        ui->label_DESTINO_QRY->setText("Generador Eolico");

        varmap = mw->varmap_gen;

        ui->list_VARIABLE_QRY->addItem("Potencia de Salida");
        ui->list_VARIABLE_QRY->addItem("Velocidad Angular");
        ui->list_VARIABLE_QRY->addItem("Torque");
        ui->list_VARIABLE_QRY->addItem("Tension de Salida");
        ui->list_VARIABLE_QRY->addItem("Corriente de ISD");
        ui->list_VARIABLE_QRY->addItem("Corriente Eficaz");
        ui->list_VARIABLE_QRY->addItem("Corriente de Bateria");
        ui->list_VARIABLE_QRY->addItem("Modo");

        consulta=LACAN_VAR_PO_SETP;

        break;
    }

    ui->list_TIPO_QRY->addItem("Instantanea",QVariant(0));
    ui->list_TIPO_QRY->addItem("SetPoint",QVariant(1));
}


void Consultar::on_button_ENVIAR_QRY_clicked()
{
    QString var_selectedstr;
    var_selectedstr=ui->list_VARIABLE_QRY->currentText();
    qDebug()<<var_selectedstr;
    if(ui->list_TIPO_QRY->currentIndex()==0)    //Instantanea
        consulta = varmap[var_selectedstr].instantanea;
    else
        consulta = varmap[var_selectedstr].setp;

    int prevsize=mw->msg_ack.size();
    LACAN_Query(mw,consulta,1);
    //verifico que haya un elemento nuevo en el vector para no tratar de conectar dos veces un mismo elemento
    if(mw->msg_ack.size()>prevsize){

    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    }
    mw->agregar_log_sent();

    this->close();
}


Consultar::~Consultar()
{
    delete ui;
}
