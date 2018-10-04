#include "enviar_mensaje.h"
#include "ui_enviar_mensaje.h"
#include <QtCore>
#include <QtGui>
#include "LACAN_SEND.h"
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
    //en el mismo orden que la enum de arriba, si no no anda
    ui->list_MENSAJE->addItem("DO");
    ui->list_MENSAJE->addItem("SET");
    ui->list_MENSAJE->addItem("QUERY");
    ui->list_MENSAJE->addItem("POST");
    ui->list_MENSAJE->addItem("ERROR");
    ui->list_MENSAJE->addItem("HEARTBEAT");
    ui->list_MENSAJE->addItem("ACKNOWLEDGE");

    mw = qobject_cast<MainWindow*>(this->parent());

    ui->list_DESTINO->addItem("Broadcast",QVariant(LACAN_ID_BROADCAST));
    ui->list_DESTINO->addItem("Generador Eolico",QVariant(LACAN_ID_GEN));
    ui->list_DESTINO->addItem("Volante de Inercia",QVariant(LACAN_ID_VOLANTE));
    ui->list_DESTINO->addItem("Boost",QVariant(LACAN_ID_BOOST));


  //  ui->list_DESTINO->setCurrentIndex(mw->dest);
    switch(mw->dest){
    case LACAN_ID_BROADCAST:
        qDebug()<<"broad";
        break;
    case LACAN_ID_GEN:
        ui->list_DESTINO->setCurrentIndex(GEN_EOL);
        qDebug()<<"gen";
        break;
    case LACAN_ID_VOLANTE:
        ui->list_DESTINO->setCurrentIndex(VOL);
        break;
    case LACAN_ID_BOOST:
        ui->list_DESTINO->setCurrentIndex(BOOST);
        break;
    }


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

    ui->list_TIPO->addItem("Maxima");
    ui->list_TIPO->addItem("Minima");
    ui->list_TIPO->addItem("Set Point");


    ui->list_COMANDO->addItem("Start");
    ui->list_COMANDO->addItem("Stop");
    ui->list_COMANDO->addItem("Reset");
    ui->list_COMANDO->addItem("MPPT_Enable");
    ui->list_COMANDO->addItem("MPPT_Disable");
    ui->list_COMANDO->addItem("Acoplar");
    ui->list_COMANDO->addItem("Desacoplar");
    ui->list_COMANDO->addItem("Magnetizar");
    ui->list_COMANDO->addItem("Trip");


    ui->list_ERROR->addItem("Generic Error");
    ui->list_ERROR->addItem("Over Voltage");
    ui->list_ERROR->addItem("Under Voltage");
    ui->list_ERROR->addItem("Over Current");
    ui->list_ERROR->addItem("Bat Over Current");
    ui->list_ERROR->addItem("Over Temperature");
    ui->list_ERROR->addItem("Over Speed");
    ui->list_ERROR->addItem("Under Speed");
    ui->list_ERROR->addItem("No HeartBeat");
    ui->list_ERROR->addItem("Internal Trip");
    ui->list_ERROR->addItem("External Trip");

    ui->list_RESULTADO->addItem("OK");
    ui->list_RESULTADO->addItem("MISSING_PREREQ");
    ui->list_RESULTADO->addItem("RECEIVED");
    ui->list_RESULTADO->addItem("NOT_IMPLEMENTED");
    ui->list_RESULTADO->addItem("OUT_OF_RANGE");
    ui->list_RESULTADO->addItem("BUSY");
    ui->list_RESULTADO->addItem("DENIED");
    ui->list_RESULTADO->addItem("GENERIC_FAILURE");

   // QMap <uint16_t,string> msgmap;
    //msgmap[lacan_]


    connect(ui->list_MENSAJE,SIGNAL(currentTextChanged(QString)),this,SLOT(MENSAJE_changed()));
    DO_selected();
    connect(ui->list_VARIABLE,SIGNAL(currentTextChanged(QString)),this,SLOT(VAR_Changed()));
    connect(ui->list_TIPO,SIGNAL(currentTextChanged(QString)),this,SLOT(TIPO_Changed()));
    connect(ui->list_COMANDO,SIGNAL(currentTextChanged(QString)),this,SLOT(CMD_Changed()));
    //connect(ui->list_DESTINO,SIGNAL(currentTextChanged(QString)),this,SLOT(DEST_Changed()));
    connect(ui->list_ERROR,SIGNAL(currentTextChanged(QString)),this,SLOT(ERR_Changed()));
    connect(ui->list_RESULTADO,SIGNAL(currentTextChanged(QString)),this,SLOT(RESULT_Changed()));



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
    ui->text_VALOR->setDisabled(true);
    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->text_CODIGO->setDisabled(true);
}

void Enviar_Mensaje::SET_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_VARIABLE->setEnabled(true);
    ui->list_TIPO->setEnabled(true);

    ui->text_VALOR->setEnabled(true);

    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->text_CODIGO->setDisabled(true);
}


void Enviar_Mensaje::DO_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_COMANDO->setEnabled(true);

    ui->list_VARIABLE->setDisabled(true);
    ui->list_TIPO->setDisabled(true);
    ui->text_VALOR->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->text_CODIGO->setDisabled(true);
}


void Enviar_Mensaje::QRY_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_VARIABLE->setEnabled(true);
    ui->list_TIPO->setEnabled(true);

    ui->text_VALOR->setDisabled(true);
    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->text_CODIGO->setDisabled(true);
}


void Enviar_Mensaje::POST_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_VARIABLE->setEnabled(true);
    ui->list_TIPO->setEnabled(true);

    ui->text_VALOR->setEnabled(true);

    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->text_CODIGO->setDisabled(true);
}


void Enviar_Mensaje::ACK_selected(){

    ui->list_DESTINO->setEnabled(true);

    ui->list_RESULTADO->setEnabled(true);
    ui->text_CODIGO->setEnabled(true);

    ui->list_VARIABLE->setDisabled(true);
    ui->list_TIPO->setDisabled(true);
    ui->text_VALOR->setDisabled(true);
    ui->list_COMANDO->setDisabled(true);
    ui->list_ERROR->setDisabled(true);
}

void Enviar_Mensaje::ERR_selected(){

    ui->list_ERROR->setEnabled(true);

    ui->list_DESTINO->setCurrentIndex(BROAD);
    ui->list_DESTINO->setDisabled(true);

    ui->list_VARIABLE->setDisabled(true);
    ui->list_TIPO->setDisabled(true);
    ui->text_VALOR->setDisabled(true);
    ui->list_COMANDO->setDisabled(true);
    ui->list_RESULTADO->setDisabled(true);
    ui->text_CODIGO->setDisabled(true);
}




void Enviar_Mensaje::VAR_Changed(){
    set_TIPO_VAR();
    TIPO_Changed();
}

void Enviar_Mensaje::TIPO_Changed(){
    switch(ui->list_VARIABLE->currentIndex()){
        case II:
            switch (ui->list_TIPO->currentIndex()) {
                case MAX:
                    var=LACAN_VAR_II_MAX;
                    break;
                case MIN:
                    var=LACAN_VAR_II_MIN;
                    break;
                case SETP:
                    var=LACAN_VAR_II_SETP;
                    break;
                case INST:
                    var=LACAN_VAR_II;
                    break;
                   }
            break;
        case IO:
            switch (ui->list_TIPO->currentIndex()) {
                case MAX:
                    var=LACAN_VAR_IO_MAX;
                    break;
                case MIN:
                    var=LACAN_VAR_IO_MIN;
                    break;
                case SETP:
                    var=LACAN_VAR_IO_SETP;
                    break;
                case INST:
                    var=LACAN_VAR_IO;
                    break;
                   }
            break;
        case ISD:
            switch (ui->list_TIPO->currentIndex()) {
                case MAX:
                    var=LACAN_VAR_ISD_MAX;
                    break;
                case MIN:
                    var=LACAN_VAR_ISD_MIN;
                    break;
                case SETP:
                    var=LACAN_VAR_ISD_SETP;
                    break;
                case INST:
                    var=LACAN_VAR_ISD;
                    break;
                   }
            break;
        case IEF:
            switch (ui->list_TIPO->currentIndex()) {
                case MAX:
                    var=LACAN_VAR_IEF_MAX;
                    break;
                case MIN:
                    var=LACAN_VAR_IEF_MIN;
                    break;
                case SETP:
                    var=LACAN_VAR_IEF_SETP;
                    break;
                case INST:
                    var=LACAN_VAR_IEF;
                    break;
                   }
            break;
        case PI:
            switch (ui->list_TIPO->currentIndex()) {
                case MAX:
                    var=LACAN_VAR_PI_MAX;
                    break;
                case MIN:
                    var=LACAN_VAR_PI_MIN;
                    break;
                case SETP:
                    var=LACAN_VAR_PI_SETP;
                    break;
                case INST:
                    var=LACAN_VAR_PI;
                    break;
                   }
            break;
        case PO:
            switch (ui->list_TIPO->currentIndex()) {
                case MAX:
                    var=LACAN_VAR_PO_MAX;
                    break;
                case MIN:
                    var=LACAN_VAR_PO_MIN;
                    break;
                case SETP:
                    var=LACAN_VAR_PO_SETP;
                    break;
                case INST:
                    var=LACAN_VAR_PO;
                    break;
                   }
            break;
        case VI:
            switch (ui->list_TIPO->currentIndex()) {
                case MAX:
                    var=LACAN_VAR_VI_MAX;
                    break;
                case MIN:
                    var=LACAN_VAR_VI_MIN;
                    break;
                case SETP:
                    var=LACAN_VAR_VI_SETP;
                    break;
                case INST:
                    var=LACAN_VAR_VI;
                    break;
                   }
            break;
        case VO:
            switch (ui->list_TIPO->currentIndex()) {
                case MAX:
                    var=LACAN_VAR_VO_MAX;
                    break;
                case MIN:
                    var=LACAN_VAR_VO_MIN;
                    break;
                case SETP:
                    var=LACAN_VAR_VO_SETP;
                    break;
                case INST:
                    var=LACAN_VAR_VO;
                    break;
                   }
            break;
        case W:
            switch (ui->list_TIPO->currentIndex()) {
                case MAX:
                    var=LACAN_VAR_W_MAX;
                    break;
                case MIN:
                    var=LACAN_VAR_W_MIN;
                    break;
                case SETP:
                    var=LACAN_VAR_W_SETP;
                    break;
                case INST:
                    var=LACAN_VAR_W;
                    break;
        }
        break;
        case MOD:
            switch (ui->list_TIPO->currentIndex()){

                case MOD_P:
                    var=LACAN_VAR_MOD_POT;
                    break;
                case MOD_V:
                    var=LACAN_VAR_MOD_VEL;
                    break;
                case MOD_T:
                    var=LACAN_VAR_MOD_TORQ;
                    break;
        }
        break;
    }
}




void Enviar_Mensaje::set_TIPO_VAR(){
    switch (ui->list_VARIABLE->currentIndex()) {
    case MOD:
        ui->text_VALOR->setDisabled(true);
        ui->list_TIPO->clear();
        ui->list_TIPO->addItem("Potencia");
        ui->list_TIPO->addItem("Velocidad");
        ui->list_TIPO->addItem("Torque");
        break;
    default:
        switch (ui->list_MENSAJE->currentIndex()) {
        case SET:
            ui->text_VALOR->setEnabled(true);
            ui->list_TIPO->clear();
            ui->list_TIPO->addItem("Maxima");
            ui->list_TIPO->addItem("Minima");
            ui->list_TIPO->addItem("Set Point");
            break;
        case POST:
            ui->text_VALOR->setEnabled(true);
            ui->list_TIPO->clear();
            ui->list_TIPO->addItem("Maxima");
            ui->list_TIPO->addItem("Minima");
            ui->list_TIPO->addItem("Set Point");
            ui->list_TIPO->addItem("Instantanea");
            break;
        case QRY:
            ui->text_VALOR->setDisabled(true);
            ui->list_TIPO->clear();
            ui->list_TIPO->addItem("Maxima");
            ui->list_TIPO->addItem("Minima");
            ui->list_TIPO->addItem("Set Point");
            ui->list_TIPO->addItem("Instantanea");
            break;
        }
    }
}



void Enviar_Mensaje::CMD_Changed(){
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
        case COUP:
            cmd=LACAN_CMD_COUPLE;
            break;
        case DECOUP:
            cmd=LACAN_CMD_DECOUPLE;
            break;
        case MAG:
            cmd=LACAN_CMD_MAGNETIZE;
            break;
        case TRIP:
            cmd=LACAN_CMD_TRIP;
            break;
    }
}

/*void Enviar_Mensaje::DEST_Changed(){

    switch(ui->list_DESTINO->currentIndex()){
        case BROAD:
            mw->dest=LACAN_ID_BROADCAST;
            break;
        case GEN_EOL:
            mw->dest=LACAN_ID_GEN;
            break;
        case VOL:
            mw->dest=LACAN_ID_VOLANTE;
            break;
        case BOOST:
            mw->dest=LACAN_ID_BOOST;
            break;
        default:
            break;

}
}*/

void Enviar_Mensaje::RESULT_Changed(){
    switch(ui->list_RESULTADO->currentIndex()){
    case OK:
        res=LACAN_RES_OK;
        break;
    case MISS_PREREQ:
        res=LACAN_RES_MISSING_PREREQ;
        break;
    case REC:
        res=LACAN_RES_RECEIVED;
        break;
    case NOT_IMPLEMENTED:
        res=LACAN_RES_NOT_IMPLEMENTED;
        break;
    case OUT_OF_RANGE:
        res=LACAN_RES_OUT_OF_RANGE;
        break;
    case BUSY:
        res=LACAN_RES_BUSY;
        break;
    case DENIED:
        res=LACAN_RES_DENIED ;
        break;
    case GEN_FAIL:
        res=LACAN_RES_GENERIC_FAILURE;
        break;

        default:
            break;

}}

void Enviar_Mensaje::ERR_Changed(){
    switch(ui->list_ERROR->currentIndex()){
        case GENER:
            err_cod=LACAN_ERR_GENERIC_ERR;
            break;
        case OVERV:
            err_cod=LACAN_ERR_OVERVOLTAGE;
            qDebug()<<"ENTRO A OVERVOLTAGE";
            break;
        case UNDERV:
            err_cod=LACAN_ERR_UNDERVOLTAGE;
            break;
        case OVERI:
            err_cod=LACAN_ERR_OVERCURRENT;
            break;
        case BAT_OVERI:
            err_cod=LACAN_ERR_BAT_OVERCURRENT;
            break;
        case OVERTEMP:
            err_cod=LACAN_ERR_OVERTEMPERATURE;
            break;
        case OVERW:
            err_cod=LACAN_ERR_OVERSPEED;
            break;
        case UNDERW:
            err_cod=LACAN_ERR_UNDERSPEED;
            break;
        case NO_HB:
            err_cod=LACAN_ERR_NO_HEARTBEAT;
            break;
        case INT_TRIP:
            err_cod=LACAN_ERR_INTERNAL_TRIP;
            break;
        case EXT_TRIP:
            err_cod=LACAN_ERR_EXTERNAL_TRIP;
            break;
        default:
            break;

}
}

void Enviar_Mensaje::on_button_ENVIAR_MENSAJE_clicked()
{
    //Verifico si el valor que se va a mandar es un entero o un float
   /*data_can data;
    uint32_t data_int = ui->text_VALOR->text().toInt();
    float data_float = ui->text_VALOR->text().toFloat();
    if(data_int == data_float){
        qDebug()<<"Son iguales";
        data.var_int = data_int;}
    else{
        qDebug()<<"Son distintos";
        data.var_float = data_float;}*/

 /*   data_can data;
    uint32_t data_int = ui->text_VALOR->text().toInt();
    float data_float = ui->text_VALOR->text().toFloat();
    if(data_int == data_float){
        //si entra aca es porque es in int, lo caesteamo a float
        data.var_float =float(data_int);
    }
    else
        data.var_float = data_float;    //si no, es un float y lo gaurdamo como esta*/

    data_can data;
    float data_float = ui->text_VALOR->text().toFloat();
    data.var_float =float(data_float);

    float probando=data.var_float;
    qDebug()<<"=============";
    qDebug()<<probando;
    qDebug()<<"=============";


    uint16_t ack_cod = uint16_t(ui->text_CODIGO->text().toInt());
    uint prevsize=mw->msg_ack.size();

    switch(ui->list_MENSAJE->currentIndex()){
    case DO:
        LACAN_Do(mw, cmd);
        break;
    case SET:
        LACAN_Set(mw,var,data);
        break;
    case QRY:
        LACAN_Query(mw,var);
        break;
    case POST:
        LACAN_Post(mw,var,data);
        break;
    case HB:
        LACAN_Heartbeat(mw);
        break;
    case ERR:
        LACAN_Error(mw,err_cod);
        break;
    case ACK:
        LACAN_Acknowledge(mw,0,ack_cod,res);      //ver req type
        break;
    }


    if(mw->msg_ack.size()>prevsize){
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    }


    mw->agregar_log_sent();

    this->close();
}



Enviar_Mensaje::~Enviar_Mensaje()
{
    delete ui;
}

void Enviar_Mensaje::on_list_DESTINO_currentIndexChanged(int index)
{
    mw->dest=ui->list_DESTINO->itemData(index).toInt();
}
