#include "consultar.h"
#include "ui_consultar.h"
#include "LACAN_SEND.h"
#include "PC.h"
#include "serialib.h"
#include <QDebug>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"

enum VARIABLES {II, IO, ISD, IEF, PI, PO, VI, VO, W, MOD};
enum TIPO_VAR {MAX,MIN,SETP,INST};
enum TIPO_MOD {MOD_P, MOD_V, MOD_T};


Consultar::Consultar(QSerialPort &serial_port0,vector <TIMED_MSG*> &msg_ack0,uint8_t &code0,vector <LACAN_MSG> &msg_log0, bool do_log0,uint16_t consdest0, QWidget *parent) :

    QDialog(parent),
    ui(new Ui::Consultar)
{
    ui->setupUi(this);
    this->setWindowTitle("Consultar");
    this->setFixedSize(ui->verticalLayout_2->sizeHint());
    this->setLayout(ui->verticalLayout_2);

    serial_port=&serial_port0;
    consdest=consdest0;
    code=&code0;
    msg_ack=&msg_ack0;
    msg_log=&msg_log0;
    do_log=do_log0;
    mw = qobject_cast<MainWindow*>(this->parent());


    switch(consdest){
    case LACAN_ID_GEN:
        ui->label_DESTINO_QRY->setText("Generador Eolico");

        ui->list_VARIABLE_QRY->addItem("Corriente de Entrada");
        ui->list_VARIABLE_QRY->addItem("Corriente de Salida");
        ui->list_VARIABLE_QRY->addItem("Corriente de ISD");
        ui->list_VARIABLE_QRY->addItem("Corriente Eficaz");
        ui->list_VARIABLE_QRY->addItem("Potencia de Entrada");
        ui->list_VARIABLE_QRY->addItem("Potencia de Salida");
        ui->list_VARIABLE_QRY->addItem("Tension de Entrada");
        ui->list_VARIABLE_QRY->addItem("Tension de Salida");
        ui->list_VARIABLE_QRY->addItem("Velocidad Angular");
        ui->list_VARIABLE_QRY->addItem("Modo");

        ui->list_TIPO_QRY->addItem("Maxima");
        ui->list_TIPO_QRY->addItem("Minima");
        ui->list_TIPO_QRY->addItem("Set Point");
        ui->list_TIPO_QRY->addItem("Instantanea");

        connect(ui->list_VARIABLE_QRY,SIGNAL(currentTextChanged(QString)),this,SLOT(VARIABLE_CONSULTA_Changed()));
        connect(ui->list_TIPO_QRY,SIGNAL(currentTextChanged(QString)),this,SLOT(TIPO_CONSULTA_Changed()));
        break;

    case LACAN_ID_BOOST:
        ui->label_DESTINO_QRY->setText("Boost");

        break;
    case LACAN_ID_VOLANTE:
        ui->label_DESTINO_QRY->setText("Volante de Inercia");

        break;
    case LACAN_ID_BROADCAST:
        ui->label_DESTINO_QRY->setText("Broadcast");

        break;

    }
}

Consultar::~Consultar()
{
    delete ui;
}

void Consultar::on_button_ENVIAR_QRY_clicked()
{

    int prevsize=msg_ack->size();
    LACAN_Query(*serial_port,consdest,consulta, *code, *msg_ack, *msg_log);
    //verifico que haya un elemento nuevo en el vector para no tratar de conectar dos veces un mismo elemento
    if(msg_ack->size()>prevsize){
        connect(&(msg_ack->back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    }

    mw->agregar_log_sent(*msg_log);

    this->close();
}



void Consultar::VARIABLE_CONSULTA_Changed(){
    set_TIPO_CONSULTA();
    TIPO_CONSULTA_Changed();
}

void Consultar::TIPO_CONSULTA_Changed(){
    switch(ui->list_VARIABLE_QRY->currentIndex()){
        case II:
            switch (ui->list_TIPO_QRY->currentIndex()) {
                case MAX:
                    consulta=LACAN_VAR_II_MAX;
                    break;
                case MIN:
                    consulta=LACAN_VAR_II_MIN;
                    break;
                case SETP:
                    consulta=LACAN_VAR_II_SETP;
                    break;
                case INST:
                    consulta=LACAN_VAR_II;
                    break;
            }
            break;
        case IO:
            switch (ui->list_TIPO_QRY->currentIndex()) {
                case MAX:
                    consulta=LACAN_VAR_IO_MAX;
                    break;
                case MIN:
                    consulta=LACAN_VAR_IO_MIN;
                    break;
                case SETP:
                    consulta=LACAN_VAR_IO_SETP;
                    break;
                case INST:
                    consulta=LACAN_VAR_IO;
                    break;
                   }
            break;
        case ISD:
            switch (ui->list_TIPO_QRY->currentIndex()) {
                case MAX:
                    consulta=LACAN_VAR_ISD_MAX;
                    break;
                case MIN:
                    consulta=LACAN_VAR_ISD_MIN;
                    break;
                case SETP:
                    consulta=LACAN_VAR_ISD_SETP;
                    break;
                case INST:
                    consulta=LACAN_VAR_ISD;
                    break;
                   }
            break;
        case IEF:
            switch (ui->list_TIPO_QRY->currentIndex()) {
                case MAX:
                    consulta=LACAN_VAR_IEF_MAX;
                    break;
                case MIN:
                    consulta=LACAN_VAR_IEF_MIN;
                    break;
                case INST:
                    consulta=LACAN_VAR_IEF;
                    break;
                   }
            break;
        case PI:
            switch (ui->list_TIPO_QRY->currentIndex()) {
                case MAX:
                    consulta=LACAN_VAR_PI_MAX;
                    break;
                case MIN:
                    consulta=LACAN_VAR_PI_MIN;
                    break;
                case SETP:
                    consulta=LACAN_VAR_PI_SETP;
                    break;
                case INST:
                    consulta=LACAN_VAR_PI;
                    break;
                   }
            break;
        case PO:
            switch (ui->list_TIPO_QRY->currentIndex()) {
                case MAX:
                    consulta=LACAN_VAR_PO_MAX;
                    break;
                case MIN:
                    consulta=LACAN_VAR_PO_MIN;
                    break;
                case SETP:
                    consulta=LACAN_VAR_PO_SETP;
                    break;
                case INST:
                    consulta=LACAN_VAR_PO;
                    break;
                       }
            break;
        case VI:
            switch (ui->list_TIPO_QRY->currentIndex()) {
                case MAX:
                    consulta=LACAN_VAR_VI_MAX;
                    break;
                case MIN:
                    consulta=LACAN_VAR_VI_MIN;
                    break;
                case SETP:
                    consulta=LACAN_VAR_VI_SETP;
                    break;
            case INST:
                consulta=LACAN_VAR_VI;
                break;
                   }
            break;
        case VO:
            switch (ui->list_TIPO_QRY->currentIndex()) {
                case MAX:
                    consulta=LACAN_VAR_VO_MAX;
                    break;
                case MIN:
                    consulta=LACAN_VAR_VO_MIN;
                    break;
                case SETP:
                    consulta=LACAN_VAR_VO_SETP;
                    break;
            case INST:
                consulta=LACAN_VAR_VO;
                break;
                   }
            break;
        case W:
            switch (ui->list_TIPO_QRY->currentIndex()) {
                case MAX:
                    consulta=LACAN_VAR_W_MAX;
                    break;
                case MIN:
                    consulta=LACAN_VAR_W_MIN;
                    break;
                case SETP:
                    consulta=LACAN_VAR_W_SETP;
                    break;
            case INST:
                consulta=LACAN_VAR_W;
                break;
            }
        break;
        case MOD:
            switch (ui->list_TIPO_QRY->currentIndex()){

                case MOD_P:
                    consulta=LACAN_VAR_MOD_POT;
                    break;
                case MOD_V:
                    consulta=LACAN_VAR_MOD_VEL;
                    break;
                case MOD_T:
                    consulta=LACAN_VAR_MOD_TORQ;
                    break;
        }
        break;
    }
}

void Consultar::set_TIPO_CONSULTA(){
    switch (ui->list_VARIABLE_QRY->currentIndex()) {
    case MOD:
        ui->list_TIPO_QRY->clear();
        ui->list_TIPO_QRY->addItem("Potencia");
        ui->list_TIPO_QRY->addItem("Velocidad");
        ui->list_TIPO_QRY->addItem("Torque");
        break;
    default:
        ui->list_TIPO_QRY->clear();
        ui->list_TIPO_QRY->addItem("Maxima");
        ui->list_TIPO_QRY->addItem("Minima");
        ui->list_TIPO_QRY->addItem("Set Point");
        ui->list_TIPO_QRY->addItem("Instantanea");
        break;
    }
}

