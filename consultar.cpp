#include "consultar.h"
#include "ui_consultar.h"
#include "PC.h"
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

Consultar::Consultar(QWidget *parent, uint16_t destMw) :

    QDialog(parent),
    ui(new Ui::Consultar)
{
    ui->setupUi(this);
    this->setWindowTitle("Consultar");

    mw = qobject_cast<MainWindow*>(this->parent());
    dest=destMw;

    switch(dest){
    case LACAN_ID_GEN:
        ui->label_DESTINO_QRY->setText("Generador Eolico");

        varmap = mw->varmap_gen;

        ui->list_VARIABLE_QRY->addItem("Potencia de Salida");
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

        varmap = mw->varmap_vol;

        ui->list_VARIABLE_QRY->addItem("Potencia de Salida");
        ui->list_VARIABLE_QRY->addItem("Velocidad Angular");
        ui->list_VARIABLE_QRY->addItem("Velocidad angular Standby");
        ui->list_VARIABLE_QRY->addItem("Torque");
        ui->list_VARIABLE_QRY->addItem("Tension de Salida");
        ui->list_VARIABLE_QRY->addItem("Corriente de ID");
        ui->list_VARIABLE_QRY->addItem("Corriente de Bateria");
        ui->list_VARIABLE_QRY->addItem("Modo");

        consulta=LACAN_VAR_PO_SETP;

        break;
    case LACAN_ID_BROADCAST:
        ui->label_DESTINO_QRY->setText("Broadcast");

        varmap = mw->varmap_broad;

        ui->list_VARIABLE_QRY->addItem("Potencia de Salida");
        ui->list_VARIABLE_QRY->addItem("Tension de Salida");
        ui->list_VARIABLE_QRY->addItem("Corriente de Salida");
        ui->list_VARIABLE_QRY->addItem("Corriente de ISD");
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
    if(var_selectedstr=="Modo"){
        consulta = LACAN_VAR_MOD;
    }else{
        if(ui->list_TIPO_QRY->currentIndex()==0)    //Instantanea
            consulta = varmap[var_selectedstr].instantanea;
        else
            consulta = varmap[var_selectedstr].setp;
    }

    uint prevsize=mw->msg_ack.size();
    mw->LACAN_Query(consulta,1,dest);
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

void Consultar::on_list_VARIABLE_QRY_currentIndexChanged(const QString &arg1)
{
    if(arg1=="Modo"){
        ui->list_TIPO_QRY->setEnabled(false);
    }
    else{
        ui->list_TIPO_QRY->setEnabled(true);
    }
}
