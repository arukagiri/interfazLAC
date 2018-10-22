#include "gen_eolico.h"
#include "ui_gen_eolico.h"
#include <QMessageBox>
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include "PC.h"
#include "LACAN_SEND.h"
#include "lacan_limits.h"



Gen_Eolico::Gen_Eolico(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Gen_Eolico)
{

    ui->setupUi(this);

    mw = qobject_cast<MainWindow*>(this->parent());

     //**********************************************************************
    //*****               se cambia el destino                      ********
     //*******************************************************************
    //mw->dest=LACAN_ID_GE
     //2 es la ID del GEN. cuando debugueo me pone 3, en la funcion, no 2
                             //si paras el timer que corre en la pantalla estado de red, esto anda perfecto
                             //creo que cuando mandas los qrys de estado de red te pisan este valor
//*************************************************************************************

    time_2sec = new QTimer();

//COMBO BOX MODO
    ui->combo_modo->addItem("Velocidad (0)",QVariant(0));
    ui->combo_modo->addItem("Potencia (1)",QVariant(1));
    ui->combo_modo->addItem("Torque (2)",QVariant(2));
    ui->combo_modo->addItem("MPPT (3)",QVariant(3));
    actual_mode=ui->combo_modo->currentIndex();

    connect(ui->combo_modo,SIGNAL(activated(int)),this,SLOT(mode_changed()));

//SPIN
    ui->spin_iconv->setValue(0);
    ui->spin_isd_ref->setValue(0);
    ui->spin_lim_ibat->setValue(0);
    ui->spin_lim_ief->setValue(0);
    ui->spin_lim_vdc->setValue(0);
    ui->spin_pot_ref->setValue(0);

    set_limits_gen();

    new_mode();                 //para que queden en grises los que correspondan
    set_spin_click(false);  //todo desclickeado

//LABELS
    ui->label_gen_io->setText("----");
    ui->label_gen_vo->setText("----");

//TIMER
    connect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));
    time_2sec->start(2000);


    //****************en la inicializacion hay que preguntar el modo tambien
    //y desp de la inicializacion hay que volver ap reguntar?

}

Gen_Eolico::~Gen_Eolico()
{
    delete ui;
}

//VER SI ANDA EL TEMA DEL CODE (mw->code)
void Gen_Eolico::on_pushButton_comandar_clicked()
{
   //Comandar *comwin = new Comandar(LACAN_ID_GEN,this);
   //Comandar *comwin = new Comandar(LACAN_ID_GEN,mw);+
   mw->dest=LACAN_ID_GEN;
   Comandar *comwin = new Comandar(mw);
   comwin->setModal(true);
   comwin->show();
}


//VER SI ANDA EL TEMA DEL CODE (mw->code)
void Gen_Eolico::on_pushButton_start_clicked()
{
    cmd=LACAN_CMD_START;
    mw->dest=LACAN_ID_GEN;
    LACAN_Do(mw,cmd,1);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

void Gen_Eolico::on_pushButton_stop_clicked()
{
    cmd=LACAN_CMD_STOP;
    mw->dest=LACAN_ID_GEN;
    LACAN_Do(mw,cmd,1);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}


void Gen_Eolico::mode_changed(){
    new_mode();
    set_spin_click(false);
    refresh_values();
}

void Gen_Eolico::new_mode(){
    switch (ui->combo_modo->currentIndex()) {
    case 0:     //Velocidad
        ui->spin_pot_ref->setDisabled(true);
        ui->spin_speed_ref->setEnabled(true);
        ui->spin_torque_ref->setDisabled(true);
        break;
    case 1:     //Potencia
        ui->spin_pot_ref->setEnabled(true);
        ui->spin_speed_ref->setDisabled(true);
        ui->spin_torque_ref->setDisabled(true);
        break;
    case 2:     //Torque
        ui->spin_pot_ref->setDisabled(true);
        ui->spin_speed_ref->setDisabled(true);
        ui->spin_torque_ref->setEnabled(true);
        break;
    case 3:     //Potencia
        ui->spin_pot_ref->setDisabled(true);
        ui->spin_speed_ref->setDisabled(true);
        ui->spin_torque_ref->setDisabled(true);
        break;
    default:
        break;
    }
}

void Gen_Eolico::timer_handler(){
    if(mw->gen_connected){         //***********esto hay que modificarlo con lo de luli
        refresh_values();
        //send_qry();
    }
    else{
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this,"Conexion perdida","El generador se ha desconectado de la red. Esta ventana se cerrara inmediatamente");
        if(reply){
            this->close();
        }
    }
}

void Gen_Eolico::send_qry(){
    //lista de variables a consultar
    LACAN_Query(mw,LACAN_VAR_VO);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_IO);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

    LACAN_Query(mw,LACAN_VAR_ISD_SETP);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_BAT_IMAX);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_IEF_MAX);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_PI_SETP);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_W_SETP);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_TORQI_SETP);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
//  LACAN_Query(mw,LACAN_VAR_PI_SETP);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
//  LACAN_Query(mw,LACAN_VAR_BAT_I);
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
//    LACAN_Query(mw,);
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
//    LACAN_Query(mw,);
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

    //hay que preguntar el modo??
}

//se actualizan los valores de los spins que no estan siendo editados en este momento
void Gen_Eolico::refresh_values(){
    if(!speed_ref_click)
        ui->spin_speed_ref->setValue(speed_ref);
    if(!pot_ref_click)
        ui->spin_pot_ref->setValue(pot_ref);
    if(!lim_vdc_click)
        ui->spin_lim_vdc->setValue(lim_vdc);
    if(!vdc_click)
        ui->spin_vdc->setValue(vdc);
    if(!iconv_click)
        ui->spin_iconv->setValue(iconv);
    if(!lim_ief_click)
        ui->spin_lim_ief->setValue(lim_ief);
    if(!torque_ref_click)
        ui->spin_torque_ref->setValue(torque_ref);
    if(!isd_ref_click)
        ui->spin_isd_ref->setValue(isd_ref);
    if(!lim_ibat_click)
        ui->spin_lim_ibat->setValue(lim_ibat);
    if(!ibat_click)
        ui->spin_ibat->setValue(ibat);

    ui->label_gen_vo->setText(QString::number(gen_vo));
    ui->label_gen_io->setText(QString::number(gen_io));
}


void Gen_Eolico::GENpost_Handler(LACAN_MSG msg){
      recibed_val.var_char[0]=msg.BYTE2;
      recibed_val.var_char[1]=msg.BYTE3;
      recibed_val.var_char[2]=msg.BYTE4;
      recibed_val.var_char[3]=msg.BYTE5;
        switch (msg.BYTE1) {
        /*case LACAN_VAR_IO:  //esta cual es? IO esta de prueba
            iconv=recibed_val.var_float;
        break;*/
        case LACAN_VAR_ISD_SETP:
            isd_ref=recibed_val.var_float;
        break;
        case LACAN_VAR_BAT_IMAX:
            lim_ibat=recibed_val.var_float;
        break;
        case LACAN_VAR_IEF_MAX:
            lim_ief=recibed_val.var_float;
        break;
         /*case LACAN_VAR_IO:
            lim_vdc=recibed_val.var_float;
        break;*/
       case LACAN_VAR_PI_SETP:
            pot_ref=recibed_val.var_float;
        break;
        case LACAN_VAR_W_SETP:
            speed_ref=recibed_val.var_float;

        break;
        case LACAN_VAR_TORQI_SETP:
            torque_ref=recibed_val.var_float;
        break;
       /* case LACAN_VAR_BAT_I: //o la de setpoint
            ibat=recibed_val.var_float;
        break;*/
       case LACAN_VAR_IO:
            gen_io=recibed_val.var_float;
        break;
        case LACAN_VAR_VO:
            gen_vo=recibed_val.var_float;
        break;
    default:
        break;
    }
}


void Gen_Eolico::on_pushButton_apply_clicked(){
    QMessageBox::StandardButton reply;
    QString str="Esta seguro que desea aplicar los cambios?";
    //str.append(ui->combo_modo->currentText());
    reply = QMessageBox::question(this,"Confirm",str, QMessageBox::Yes | QMessageBox::No );
    if(reply==QMessageBox::Yes){
        enviar_variables_generales();
        actual_mode = ui->combo_modo->currentIndex();
        switch(actual_mode){
            case 3: //mppt
                qDebug()<<"caso3";
                //LACAN_Set(mw,LACAN_VAR_MOD_MPPT,true);
                //connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
            break;
            case 2: //torque
                qDebug()<<"caso2";
                //LACAN_Set(mw,LACAN_VAR_MOD_TORQ,true);
                //connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
            break;
            case 1: //potencia
                qDebug()<<"caso1";
                //LACAN_Set(mw,LACAN_VAR_MOD_POT,true);
                //connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
            break;
            case 0: //velocidad      //no me deja poner este primero..

                //LACAN_Set(mw,LACAN_VAR_MOD_VEL,true);
            //connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

            /* uint32_t data_int = ui->spin_speed_ref->value();
               float data_float = ui->spin_speed_ref->value();
               if(data_int == data_float)   //para ver si estas mandando un int o un float
                   val.var_int = data_int;
               else
                   val.var_float = data_float;*/
               val.var_float=ui->spin_speed_ref->value();
               LACAN_Set(mw,LACAN_VAR_IO,val,1);
               connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
               qDebug()<<"Se mando el set";
               mw->agregar_log_sent();
            break;
                }
        }
    else{
    on_pushButton_cancel_clicked();
    }
}

void Gen_Eolico::on_pushButton_cancel_clicked(){
    ui->combo_modo->setCurrentIndex(actual_mode);
    new_mode();
    refresh_values();
    set_spin_click(false);
}

void Gen_Eolico::set_spin_click(bool state){
    speed_ref_click=state;
    pot_ref_click=state;
    lim_vdc_click=state;
    vdc_click=state;
    iconv_click=state;
    lim_ief_click=state;
    torque_ref_click=state;
    isd_ref_click=state;
    lim_ibat_click=state;
    ibat_click=state;
}

//en esta funcion se envia el set de las variables que son comunes a todos los modos
void Gen_Eolico::enviar_variables_generales(){

    val.var_float=ui->spin_isd_ref->value();
    LACAN_Set(mw,LACAN_VAR_ISD_SETP,val,1);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
    val.var_float=ui->spin_lim_ibat->value();
    LACAN_Set(mw,LACAN_VAR_BAT_IMAX,val,1);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
    val.var_float=ui->spin_lim_ief->value();
    LACAN_Set(mw,LACAN_VAR_IEF_MAX,val,1);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();

  /*  val.var_float=ui->spin_ibat->value();
    LACAN_Set(mw,   ,val);
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
    val.var_float=ui->spin_lim_vdc->value();
    LACAN_Set(mw,   ,val);
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
    val.var_float=ui->spin_vdc->value();
    LACAN_Set(mw,   ,val);
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
    val.var_float=ui->spin_iconv->value();
    LACAN_Set(mw,   ,val);
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();*/
}

void Gen_Eolico::closeEvent(QCloseEvent *e){
    time_2sec->stop();
    delete time_2sec;
    QDialog::closeEvent(e);
}

void Gen_Eolico::on_spin_ibat_valueChanged(double arg1)
{
    ibat_click = true;
}

void Gen_Eolico::on_spin_lim_ibat_valueChanged(double arg1)
{
    lim_ibat_click = true;
}

void Gen_Eolico::on_spin_lim_vdc_valueChanged(double arg1)
{
    lim_vdc_click = true;
}

void Gen_Eolico::on_spin_vdc_valueChanged(double arg1)
{
    vdc_click = true;
}

void Gen_Eolico::on_spin_speed_ref_valueChanged(double arg1)
{
    speed_ref_click = true;
}


void Gen_Eolico::on_spin_pot_ref_valueChanged(double arg1)
{
    pot_ref_click = true;
}

void Gen_Eolico::on_spin_iconv_valueChanged(double arg1)
{
    iconv_click = true;
}

void Gen_Eolico::on_spin_lim_ief_valueChanged(double arg1)
{
    lim_ief_click = true;
}

void Gen_Eolico::on_spin_torque_ref_valueChanged(double arg1)
{
    torque_ref_click = true;
}

void Gen_Eolico::on_spin_isd_ref_valueChanged(double arg1)
{
    isd_ref_click = true;
}


void Gen_Eolico::set_limits_gen(){
    ui->spin_ibat->setMaximum(LACAN_VAR_GEN_BAT_I_SETP_LIM_MAX);
    ui->spin_ibat->setMinimum(LACAN_VAR_GEN_BAT_I_SETP_LIM_MIN);
    ui->spin_ibat->setDecimals(3);

    ui->spin_lim_ibat->setMaximum(LACAN_VAR_GEN_BAT_IMAX_LIM_MAX);
    ui->spin_lim_ibat->setMinimum(LACAN_VAR_GEN_BAT_IMAX_LIM_MIN);
    ui->spin_lim_ibat->setDecimals(3);

    //ui->spin_vdc->setMaximum();
    //ui->spin_vdc->setMinimum();
    ui->spin_vdc->setDecimals(3);

    //ui->spin_lim_vdc->setMaximum();
   // ui->spin_lim_vdc->setMinimum();
    ui->spin_lim_vdc->setDecimals(3);

    ui->spin_speed_ref->setMaximum(LACAN_VAR_GEN_W_SETP_LIM_MAX);
    ui->spin_speed_ref->setMinimum(LACAN_VAR_GEN_W_SETP_LIM_MIN);
    ui->spin_speed_ref->setDecimals(3);

    ui->spin_pot_ref->setMaximum(LACAN_VAR_GEN_PO_SETP_LIM_MAX);
    ui->spin_pot_ref->setMinimum(LACAN_VAR_GEN_PO_SETP_LIM_MIN);
    ui->spin_pot_ref->setDecimals(3);

    ui->spin_torque_ref->setMaximum(LACAN_VAR_GEN_TORQI_SETP_LIM_MAX);
    ui->spin_torque_ref->setMinimum(LACAN_VAR_GEN_TORQI_SETP_LIM_MIN);
    ui->spin_torque_ref->setDecimals(3);

    ui->spin_lim_ief->setMaximum(LACAN_VAR_GEN_IEF_MAX_LIM_MAX);
    ui->spin_lim_ief->setMinimum(LACAN_VAR_GEN_IEF_MAX_LIM_MIN);
    ui->spin_lim_ief->setDecimals(3);

    //ui->spin_iconv->setMaximum();
    //ui->spin_iconv->setMinimum();
    ui->spin_iconv->setDecimals(3);

    ui->spin_isd_ref->setMaximum(LACAN_VAR_GEN_ISD_SETP_LIM_MAX);
    ui->spin_isd_ref->setMinimum(LACAN_VAR_GEN_ISD_SETP_LIM_MIN);
    ui->spin_isd_ref->setDecimals(3);

}
