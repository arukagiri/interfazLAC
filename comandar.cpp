#include "comandar.h"
#include "ui_comandar.h"
#include <QtCore>
#include <QtGui>
#include <QMessageBox>
#include "LACAN_SEND.h"
#include "PC.h"
#include <QDebug>
#include <QWidget>

enum VARIABLES {II, IO, ISD, IEF, PI, PO, VI, VO, W, MOD};
enum TIPO_VAR {MAX,MIN,SETP};
enum TIPO_MOD {MOD_P, MOD_V, MOD_T};
enum COMANDOS {START,STOP,RESET,MPPT_EN,MPPT_DIS};

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

    ui->list_COMANDO->setDisabled(true);

    qDebug()<<mw->dest;
    switch(mw->dest){
        case LACAN_ID_GEN:
            ui->label_DESTINO->setText("Generador Eolico");

            ui->list_VARIABLE->addItem("Corriente de Entrada");
            ui->list_VARIABLE->addItem("Corriente de Salida");
            ui->list_VARIABLE->addItem("Corriente de ISD");
            ui->list_VARIABLE->addItem("Corriente Eficaz");
            ui->list_VARIABLE->addItem("Potencia de Entrada");
            ui->list_VARIABLE->addItem("Potencia de Salida");
            ui->list_VARIABLE->addItem("Tension de Entrada");
            ui->list_VARIABLE->addItem("Tension de Salida");
            ui->list_VARIABLE->addItem("Velocidad Angular");
            ui->list_VARIABLE->addItem("Modo");

            ui->list_TIPO_SET->addItem("Maxima");
            ui->list_TIPO_SET->addItem("Minima");
            ui->list_TIPO_SET->addItem("Set Point");


            ui->list_COMANDO->addItem("Start");
            ui->list_COMANDO->addItem("Stop");
            ui->list_COMANDO->addItem("Reset");
            ui->list_COMANDO->addItem("MPPT_Enable");
            ui->list_COMANDO->addItem("MPPT_Disable");
    }
    connect(ui->radio_DO,SIGNAL(clicked(bool)),this,SLOT(DO_selected()));
    connect(ui->radio_SET,SIGNAL(clicked(bool)),this,SLOT(SET_selected()));

    connect(ui->list_VARIABLE,SIGNAL(currentTextChanged(QString)),this,SLOT(SET_VAR_Changed()));
    connect(ui->list_TIPO_SET,SIGNAL(currentTextChanged(QString)),this,SLOT(SET_TIPO_Changed()));
    connect(ui->list_COMANDO,SIGNAL(currentTextChanged(QString)),this,SLOT(DO_CMD_Changed()));

    var_set=LACAN_VAR_II_MAX;
    cmd=LACAN_CMD_START;

    //ui->text_VALOR_COMANDO->setInputMask("99999");    //para insertar solo numeros
}

//PARA LAS VENTANAS DE CADA DISPOSITIVO
/*Comandar::Comandar(uint16_t destino, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Comandar)
{
    ui->setupUi(this);

    this->setWindowTitle("Comandar");
    this->setFixedSize(ui->verticalLayout_2->sizeHint());
    this->setLayout(ui->verticalLayout_2);

    ui->list_COMANDO->setDisabled(true);

    switch(destino){
        case LACAN_ID_GEN:
            ui->label_DESTINO->setText("Generador Eolico");

            ui->list_VARIABLE->addItem("Corriente de Entrada");
            ui->list_VARIABLE->addItem("Corriente de Salida");
            ui->list_VARIABLE->addItem("Corriente de ISD");
            ui->list_VARIABLE->addItem("Corriente Eficaz");
            ui->list_VARIABLE->addItem("Potencia de Entrada");
            ui->list_VARIABLE->addItem("Potencia de Salida");
            ui->list_VARIABLE->addItem("Tension de Entrada");
            ui->list_VARIABLE->addItem("Tension de Salida");
            ui->list_VARIABLE->addItem("Velocidad Angular");
            ui->list_VARIABLE->addItem("Modo");

            ui->list_TIPO_SET->addItem("Maxima");
            ui->list_TIPO_SET->addItem("Minima");
            ui->list_TIPO_SET->addItem("Set Point");


            ui->list_COMANDO->addItem("Start");
            ui->list_COMANDO->addItem("Stop");
            ui->list_COMANDO->addItem("Reset");
            ui->list_COMANDO->addItem("MPPT_Enable");
            ui->list_COMANDO->addItem("MPPT_Disable");
    }
    connect(ui->radio_DO,SIGNAL(clicked(bool)),this,SLOT(DO_selected()));
    connect(ui->radio_SET,SIGNAL(clicked(bool)),this,SLOT(SET_selected()));

    connect(ui->list_VARIABLE,SIGNAL(currentTextChanged(QString)),this,SLOT(SET_VAR_Changed()));
    connect(ui->list_TIPO_SET,SIGNAL(currentTextChanged(QString)),this,SLOT(SET_TIPO_Changed()));
    connect(ui->list_COMANDO,SIGNAL(currentTextChanged(QString)),this,SLOT(DO_CMD_Changed()));

    var_set=LACAN_VAR_II_MAX;
    cmd=LACAN_CMD_START;

    ui->text_VALOR_COMANDO->setInputMask("99999");    //para insertar solo numeros
}
*/

void Comandar::DO_selected(){
    ui->list_VARIABLE->setDisabled(true);
    ui->text_VALOR_COMANDO->setDisabled(true);
    ui->list_COMANDO->setEnabled(true);

}

void Comandar::SET_selected(){
    ui->list_COMANDO->setDisabled(true);
    ui->list_VARIABLE->setEnabled(true);
    ui->text_VALOR_COMANDO->setEnabled(true);
}


Comandar::~Comandar()
{
    delete ui;
}

void Comandar::on_button_ENVIAR_clicked()
{
    data_can data;
    uint32_t data_int = ui->text_VALOR_COMANDO->text().toInt();
    float data_float = ui->text_VALOR_COMANDO->text().toFloat();
    if(data_int == data_float)
        data.var_int = data_int;
    else
        data.var_float = data_float;

    int prevsize= mw->msg_ack.size();

    if(ui->radio_DO->isChecked()){
        LACAN_Do(mw,cmd);
    }else if(ui->radio_SET->isChecked()){
        LACAN_Set(mw,var_set,data);
    }else{
        QMessageBox::warning(this,"Ups... Algo salio mal","Ninguna de las dos opciones seleccionadas");
    }
    if(mw->msg_ack.size()>prevsize){
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    }

    mw->agregar_log_sent();

    this->close();
}

void Comandar::SET_VAR_Changed(){
    set_tipo();
    SET_TIPO_Changed();
}
void Comandar::SET_TIPO_Changed(){
    switch(ui->list_VARIABLE->currentIndex()){
        case II:
            switch (ui->list_TIPO_SET->currentIndex()) {
                case MAX:
                    var_set=LACAN_VAR_II_MAX;
                    break;
                case MIN:
                    var_set=LACAN_VAR_II_MIN;
                    break;
                case SETP:
                    var_set=LACAN_VAR_II_SETP;
                    break;
                   }
            break;
        case IO:
            switch (ui->list_TIPO_SET->currentIndex()) {
                case MAX:
                    var_set=LACAN_VAR_IO_MAX;
                    break;
                case MIN:
                    var_set=LACAN_VAR_IO_MIN;
                    break;
                case SETP:
                    var_set=LACAN_VAR_IO_SETP;
                    break;
                   }
            break;
        case ISD:
            switch (ui->list_TIPO_SET->currentIndex()) {
                case MAX:
                    var_set=LACAN_VAR_ISD_MAX;
                    break;
                case MIN:
                    var_set=LACAN_VAR_ISD_MIN;
                    break;
                case SETP:
                    var_set=LACAN_VAR_ISD_SETP;
                    break;
                   }
            break;
        case IEF:
            switch (ui->list_TIPO_SET->currentIndex()) {
                case MAX:
                    var_set=LACAN_VAR_IEF_MAX;
                    break;
                case MIN:
                    var_set=LACAN_VAR_IEF_MIN;
                    break;
                case SETP:
                    var_set=LACAN_VAR_IEF_SETP;
                    break;
                   }
            break;
        case PI:
            switch (ui->list_TIPO_SET->currentIndex()) {
                case MAX:
                    var_set=LACAN_VAR_PI_MAX;
                    break;
                case MIN:
                    var_set=LACAN_VAR_PI_MIN;
                    break;
                case SETP:
                    var_set=LACAN_VAR_PI_SETP;
                    break;
                   }
            break;
        case PO:
            switch (ui->list_TIPO_SET->currentIndex()) {
                case MAX:
                    var_set=LACAN_VAR_PO_MAX;
                    break;
                case MIN:
                    var_set=LACAN_VAR_PO_MIN;
                    break;
                case SETP:
                    var_set=LACAN_VAR_PO_SETP;
                    break;
                   }
            break;
        case VI:
            switch (ui->list_TIPO_SET->currentIndex()) {
                case MAX:
                    var_set=LACAN_VAR_VI_MAX;
                    break;
                case MIN:
                    var_set=LACAN_VAR_VI_MIN;
                    break;
                case SETP:
                    var_set=LACAN_VAR_VI_SETP;
                    break;
                   }
            break;
        case VO:
            switch (ui->list_TIPO_SET->currentIndex()) {
                case MAX:
                    var_set=LACAN_VAR_VO_MAX;
                    break;
                case MIN:
                    var_set=LACAN_VAR_VO_MIN;
                    break;
                case SETP:
                    var_set=LACAN_VAR_VO_SETP;
                    break;
                   }
            break;
        case W:
            switch (ui->list_TIPO_SET->currentIndex()) {
                case MAX:
                    var_set=LACAN_VAR_W_MAX;
                    break;
                case MIN:
                    var_set=LACAN_VAR_W_MIN;
                    break;
                case SETP:
                    var_set=LACAN_VAR_W_SETP;
                    break;
        }
        break;
        case MOD:
            switch (ui->list_TIPO_SET->currentIndex()){

                case MOD_P:
                    var_set=LACAN_VAR_MOD_POT;
                    break;
                case MOD_V:
                    var_set=LACAN_VAR_MOD_VEL;
                    break;
                case MOD_T:
                    var_set=LACAN_VAR_MOD_TORQ;
                    break;
        }
        break;
    }
}

void Comandar::DO_CMD_Changed(){

    switch(ui->list_COMANDO->currentIndex()){
        case START:
            cmd=LACAN_CMD_START;
            break;
        case STOP:
            cmd=LACAN_CMD_STOP;
            break;
        case RESET:
            cmd=LACAN_CMD_RESET;
            break;
        case MPPT_EN:
            cmd=LACAN_CMD_MPPT_ENABLE;
            break;
        case MPPT_DIS:
            cmd=LACAN_CMD_MPPT_DISABLE;
            break;
    }
}



void Comandar::set_tipo(){
    switch (ui->list_VARIABLE->currentIndex()) {
    case MOD:
        ui->list_TIPO_SET->clear();
        ui->list_TIPO_SET->addItem("Potencia");
        ui->list_TIPO_SET->addItem("Velocidad");
        ui->list_TIPO_SET->addItem("Torque");
        break;
    default:
        ui->list_TIPO_SET->clear();
        ui->list_TIPO_SET->addItem("Maxima");
        ui->list_TIPO_SET->addItem("Minima");
        ui->list_TIPO_SET->addItem("Set Point");
        break;
    }
}
