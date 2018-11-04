#include "comandar.h"
#include "ui_comandar.h"
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include "LACAN_SEND.h"
#include "PC.h"
#include <QDebug>
#include <QWidget>
#include <QMap>

//PARA LA MAIN WINDOWS
Comandar::Comandar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Comandar)
{
    ui->setupUi(this);

    this->setWindowTitle("Comandar");
    this->setFixedSize(ui->verticalLayout_2->sizeHint());
    this->setLayout(ui->verticalLayout_2);

    mw = qobject_cast<MainWindow*>(this->parent());

    switch(mw->dest){
        case LACAN_ID_GEN:
            ui->label_DESTINO->setText("Generador Eolico");

            varmap = mw->varmap_gen;

            ui->list_VARIABLE->addItem("Potencia de Salida");
            ui->list_VARIABLE->addItem("Velocidad Angular");
            ui->list_VARIABLE->addItem("Torque");
            ui->list_VARIABLE->addItem("Tension de Salida");
            ui->list_VARIABLE->addItem("Corriente de ISD");
            ui->list_VARIABLE->addItem("Corriente Eficaz");
            ui->list_VARIABLE->addItem("Corriente de Bateria");
            ui->list_VARIABLE->addItem("Modo");

            ui->list_MOD_SET->addItem("Velocidad",QVariant(LACAN_VAR_MOD_VEL));
            ui->list_MOD_SET->addItem("Potencia",QVariant(LACAN_VAR_MOD_POT));
            ui->list_MOD_SET->addItem("Torque",QVariant(LACAN_VAR_MOD_TORQ));
            ui->list_MOD_SET->addItem("MPPT",QVariant(LACAN_VAR_MOD_MPPT));
            ui->list_MOD_SET->setDisabled(true);
            mode_set = LACAN_VAR_MOD_VEL;   //inicializo con el primero

            ui->list_COMANDO->addItem("Start",QVariant(LACAN_CMD_START));
            ui->list_COMANDO->addItem("Stop",QVariant(LACAN_CMD_STOP));
            ui->list_COMANDO->addItem("Reset",QVariant(LACAN_CMD_RESET));
            cmd = LACAN_CMD_START;

            //var_set=LACAN_VAR_PO_SETP;
            //ui->spin_valor->setMaximum(varmap["Potencia de Salida"].max);
            //ui->spin_valor->setMinimum(varmap["Potencia de Salida"].min);
            //cmd=LACAN_CMD_START;
            //mode_set=LACAN_VAR_MOD_MPPT;
    }

    SET_selected();
    connect(ui->radio_DO,SIGNAL(clicked(bool)),this,SLOT(DO_selected()));
    connect(ui->radio_SET,SIGNAL(clicked(bool)),this,SLOT(SET_selected()));

    connect(ui->list_VARIABLE,SIGNAL(currentTextChanged(QString)),this,SLOT(SET_VAR_Changed()));

    //ui->spin_valor->setInputMask("99999");    //para insertar solo numeros
}

void Comandar::on_button_ENVIAR_clicked()
{
    uint prevsize= mw->msg_ack.size();

    if(ui->radio_DO->isChecked()){
        LACAN_Do(mw,cmd,1);
    }else if(ui->radio_SET->isChecked()){
        data_can data;
        if (ui->list_VARIABLE->currentText() == "Modo"){
        data.var_char[0]=mode_set;
        data.var_char[1]=0;
        data.var_char[2]=0;
        data.var_char[3]=0;
        }
        else{
        SET_ACTUAL_VAR();
        data.var_float=ui->spin_valor->value(); //si esta seleccionado algo que no sea modo, manda el valor de spin
        }
        LACAN_Set(mw,var_set,data,1);
    }else{
        QMessageBox::warning(this,"Ups... Algo salio mal","Ninguna de las dos opciones seleccionadas");
    }

    if(mw->msg_ack.size()>prevsize){
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    }

    mw->agregar_log_sent();

    this->close();
}

void Comandar::DO_selected(){
    ui->list_VARIABLE->setDisabled(true);
    ui->spin_valor->setDisabled(true);
    ui->list_MOD_SET->setDisabled(true);
    ui->list_COMANDO->setEnabled(true);
}

void Comandar::SET_selected(){
    ui->list_COMANDO->setDisabled(true);
    ui->list_VARIABLE->setEnabled(true);
    SET_VAR_Changed();
}

void Comandar::SET_VAR_Changed(){               //habilita y deshabilita los campos dependiendo si es una variable o un modo
    if (ui->list_VARIABLE->currentText() == "Modo"){
        ui->list_MOD_SET->setEnabled(true);
        ui->spin_valor->setDisabled(true);
        var_set=LACAN_VAR_MOD;}
    else{
        ui->list_MOD_SET->setDisabled(true);
        ui->spin_valor->setEnabled(true);
        SET_ACTUAL_VAR();}
}

void Comandar::SET_ACTUAL_VAR(){
   QString var_selectedstr;
   var_selectedstr=ui->list_VARIABLE->currentText();
   qDebug()<<var_selectedstr;
   var_set = varmap[var_selectedstr].setp;
   ui->spin_valor->setMaximum(varmap[var_selectedstr].max);
   ui->spin_valor->setMinimum(varmap[var_selectedstr].min);
}


void Comandar::on_list_MOD_SET_currentIndexChanged(int index)
{
    mode_set = ui->list_MOD_SET->itemData(index).toInt();
}

void Comandar::on_list_COMANDO_currentIndexChanged(int index)
{
    cmd = ui->list_COMANDO->itemData(index).toInt();
}

Comandar::~Comandar()
{
    delete ui;
}
