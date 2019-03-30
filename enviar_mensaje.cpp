#include "enviar_mensaje.h"
#include "ui_enviar_mensaje.h"
#include <QtCore>
#include <QtGui>
#include <QDebug>
#include <QMap>

enum MENSAJES {DO, SET, QRY, POST, ERR, HB, ACK};

enum DESTINOS {BROAD,GEN_EOL,VOL,BOOST};
enum VARIABLES {II, IO, ISD, IEF, PI, PO, VI, VO, W, MOD};
enum TIPO_VAR {MAX,MIN,SETP,INST};
enum TIPO_MOD {MOD_P, MOD_V, MOD_T};
enum COMANDOS {START,STOP,RESET,MPPT_EN,MPPT_DIS,COUP,DECOUP,MAG,TRIP};
enum ERRORES  {GENER,OVERV,UNDERV,OVERI,BAT_OVERI,OVERTEMP,OVERW,UNDERW,NO_HB,INT_TRIP,EXT_TRIP};
enum RESULTADOS {OK,MISS_PREREQ,REC,NOT_IMPLEMENTED,OUT_OF_RANGE,BUSY,DENIED,GEN_FAIL};


Enviar_Mensaje::Enviar_Mensaje(QWidget *parent) :

    QDialog(parent),
    ui(new Ui::Enviar_Mensaje)
{    
    ui->setupUi(this);
    this->setWindowTitle("Enviar mensaje");
    this->setFixedSize(ui->verticalLayout->sizeHint());
    this->setLayout(ui->verticalLayout);


    mw = qobject_cast<MainWindow*>(this->parent());

    ui->list_MENSAJE->addItem("DO");
    ui->list_MENSAJE->addItem("SET");
    ui->list_MENSAJE->addItem("QUERY");
    ui->list_MENSAJE->addItem("POST");
    ui->list_MENSAJE->addItem("ERROR");
    ui->list_MENSAJE->addItem("HEARTBEAT");
    ui->list_MENSAJE->addItem("ACKNOWLEDGE");

    ui->list_DESTINO->addItem("Broadcast", LACAN_ID_BROADCAST);
    ui->list_DESTINO->addItem("Generador", LACAN_ID_GEN);
    ui->list_DESTINO->addItem("Volante", LACAN_ID_VOLANTE);
    ui->list_DESTINO->addItem("Boost", LACAN_ID_BOOST);


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

    ui->list_COMANDO->addItem("Start", LACAN_CMD_START);
    ui->list_COMANDO->addItem("Stop", LACAN_CMD_STOP);
    ui->list_COMANDO->addItem("Reset", LACAN_CMD_RESET);
    ui->list_COMANDO->addItem("MPPT_Enable", LACAN_CMD_MPPT_ENABLE);
    ui->list_COMANDO->addItem("MPPT_Disable", LACAN_CMD_MPPT_DISABLE);
    ui->list_COMANDO->addItem("Acoplar", LACAN_CMD_COUPLE);
    ui->list_COMANDO->addItem("Desacoplar", LACAN_CMD_DECOUPLE);
    ui->list_COMANDO->addItem("Magnetizar", LACAN_CMD_MAGNETIZE);
    ui->list_COMANDO->addItem("Trip", LACAN_CMD_TRIP);

    ui->list_ERROR->addItem("Generic Error", LACAN_ERR_GENERIC_ERR);
    ui->list_ERROR->addItem("Over Voltage", LACAN_ERR_OVERVOLTAGE);
    ui->list_ERROR->addItem("Under Voltage", LACAN_ERR_UNDERVOLTAGE);
    ui->list_ERROR->addItem("Over Current", LACAN_ERR_OVERCURRENT);
    ui->list_ERROR->addItem("Bat Over Current", LACAN_ERR_BAT_OVERCURRENT);
    ui->list_ERROR->addItem("Over Temperature", LACAN_ERR_OVERTEMPERATURE);
    ui->list_ERROR->addItem("Over Speed", LACAN_ERR_OVERSPEED);
    ui->list_ERROR->addItem("Under Speed", LACAN_ERR_UNDERSPEED);
    ui->list_ERROR->addItem("No HeartBeat", LACAN_ERR_NO_HEARTBEAT);
    ui->list_ERROR->addItem("Internal Trip", LACAN_ERR_INTERNAL_TRIP);
    ui->list_ERROR->addItem("External Trip", LACAN_ERR_EXTERNAL_TRIP);

    ui->list_RESULTADO->addItem("OK", LACAN_RES_OK);
    ui->list_RESULTADO->addItem("MISSING_PREREQ", LACAN_RES_MISSING_PREREQ);
    ui->list_RESULTADO->addItem("RECEIVED", LACAN_RES_RECEIVED);
    ui->list_RESULTADO->addItem("NOT_IMPLEMENTED", LACAN_RES_NOT_IMPLEMENTED);
    ui->list_RESULTADO->addItem("OUT_OF_RANGE", LACAN_RES_OUT_OF_RANGE);
    ui->list_RESULTADO->addItem("BUSY", LACAN_RES_BUSY);
    ui->list_RESULTADO->addItem("DENIED", LACAN_RES_DENIED);
    ui->list_RESULTADO->addItem("GENERIC_FAILURE", LACAN_RES_GENERIC_FAILURE);

    ui->spin_valor->setMaximum(1000);

    connect(ui->list_MENSAJE,SIGNAL(currentTextChanged(QString)),this,SLOT(MENSAJE_changed()));
    connect(ui->list_VARIABLE,SIGNAL(currentTextChanged(QString)),this,SLOT(VAR_Changed()));
    connect(ui->list_TIPO,SIGNAL(currentTextChanged(QString)),this,SLOT(TIPO_Changed()));
    connect(ui->list_COMANDO,SIGNAL(currentTextChanged(QString)),this,SLOT(CMD_Changed()));
    connect(ui->list_DESTINO,SIGNAL(currentTextChanged(QString)),this,SLOT(DEST_Changed()));
    connect(ui->list_ERROR,SIGNAL(currentTextChanged(QString)),this,SLOT(ERR_Changed()));
    connect(ui->list_RESULTADO,SIGNAL(currentTextChanged(QString)),this,SLOT(RESULT_Changed()));

    DO_selected();

}

Enviar_Mensaje::~Enviar_Mensaje()
{
    delete ui;
}

void Enviar_Mensaje::MENSAJE_changed(){
    switch (ui->list_MENSAJE->currentIndex()) {
    case DO:
        DO_selected();
        break;
    case SET:
        set_TIPO_VAR();
        SET_selected();
        break;
    case QRY:
        set_TIPO_VAR();
        QRY_selected();
        break;
    case POST:
        set_TIPO_VAR();
        POST_selected();
        break;
    case ERR:
        ERR_selected();
        break;
    case HB:
        HB_selected();
        break;
    case ACK:
        ACK_selected();
        break;
    default:
        break;
    }
}

void Enviar_Mensaje::HB_selected(){

    ui->list_DESTINO->setCurrentIndex(BROAD);
    ui->list_DESTINO->setDisabled(true);

    ui->list_VARIABLE->setDisabled(true);
    ui->list_TIPO->setDisabled(true);
    ui->spin_valor->setDisabled(true);
    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->spin_codigo->setDisabled(true);
}

void Enviar_Mensaje::SET_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_VARIABLE->setEnabled(true);
    ui->list_TIPO->setEnabled(true);

    ui->spin_valor->setEnabled(true);

    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->spin_codigo->setDisabled(true);
}


void Enviar_Mensaje::DO_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_COMANDO->setEnabled(true);

    ui->list_VARIABLE->setDisabled(true);
    ui->list_TIPO->setDisabled(true);
    ui->spin_valor->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->spin_codigo->setDisabled(true);
}


void Enviar_Mensaje::QRY_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_VARIABLE->setEnabled(true);
    ui->list_TIPO->setEnabled(true);

    ui->spin_valor->setDisabled(true);
    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->spin_codigo->setDisabled(true);
}


void Enviar_Mensaje::POST_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_VARIABLE->setEnabled(true);
    ui->list_TIPO->setEnabled(true);

    ui->spin_valor->setEnabled(true);

    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->spin_codigo->setDisabled(true);
}


void Enviar_Mensaje::ACK_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_RESULTADO->setEnabled(true);
    ui->spin_codigo->setEnabled(true);

    ui->list_VARIABLE->setDisabled(true);
    ui->list_TIPO->setDisabled(true);
    ui->spin_valor->setDisabled(true);
    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
}

void Enviar_Mensaje::ERR_selected(){

    ui->list_ERROR->setEnabled(true);

    ui->list_DESTINO->setCurrentIndex(BROAD);
    ui->list_DESTINO->setDisabled(true);

    ui->list_VARIABLE->setDisabled(true);
    ui->list_TIPO->setDisabled(true);
    ui->spin_valor->setDisabled(true);
    ui->list_COMANDO->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->spin_codigo->setDisabled(true);
}




void Enviar_Mensaje::VAR_Changed(){
    set_TIPO_VAR();
}

void Enviar_Mensaje::set_TIPO_VAR(){
    switch (ui->list_VARIABLE->currentIndex()) {
    case MOD:
        ui->spin_valor->setDisabled(true);
        ui->list_TIPO->clear();
        ui->list_TIPO->addItem("Potencia");
        ui->list_TIPO->addItem("Velocidad");
        ui->list_TIPO->addItem("Torque");
        break;
    default:
        switch (ui->list_MENSAJE->currentIndex()) {
        case SET:
            ui->spin_valor->setEnabled(true);
            ui->list_TIPO->clear();
            ui->list_TIPO->addItem("Set Point");
            break;
        case POST:
            ui->spin_valor->setEnabled(true);
            ui->list_TIPO->clear();
            ui->list_TIPO->addItem("Set Point");
            ui->list_TIPO->addItem("Instantanea");
            break;
        case QRY:
            ui->spin_valor->setDisabled(true);
            ui->list_TIPO->clear();
            ui->list_TIPO->addItem("Set Point");
            ui->list_TIPO->addItem("Instantanea");
            break;
        }
    }
}

void Enviar_Mensaje::on_button_ENVIAR_MENSAJE_clicked()
{
    data_can data;
    data.var_float = ui->spin_valor->value();

    uint16_t ack_cod = ui->spin_codigo->value();
    uint prevsize=mw->msg_ack.size();

    cmd=ui->list_COMANDO->currentIndex();
    QString varString = ui->list_MENSAJE->currentText()+ui->list_TIPO->currentText();
    var=varmap[varString];
    dest=ui->list_DESTINO->currentIndex();

    switch(ui->list_MENSAJE->currentIndex()){
    case DO:
        mw->LACAN_Do(cmd,1,dest);
        break;
    case SET:
        mw->LACAN_Set(var,data,1,dest);
        break;
    case QRY:
        mw->LACAN_Query(var,1,dest);
        break;
    case POST:
        mw->LACAN_Post(var,data,dest);
        break;
    case HB:
        mw->LACAN_Heartbeat();
        break;
    case ERR:
        mw->LACAN_Error(err_cod);
        break;
    case ACK:
        mw->LACAN_Acknowledge(ack_cod,res,dest);
        break;
    }


    if(mw->msg_ack.size()>prevsize){
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    }


    mw->agregar_log_sent();

    this->close();
}
