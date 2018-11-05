#include "gen_eolico.h"
#include "ui_gen_eolico.h"
#include <QMessageBox>
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include "PC.h"
#include "LACAN_SEND.h"
#include "lacan_limits_gen.h"

Gen_Eolico::Gen_Eolico(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Gen_Eolico)
{

    ui->setupUi(this);

    mw = qobject_cast<MainWindow*>(this->parent());

//*******************************************************************
//*****               se cambia el destino                   ********
//*******************************************************************
//mw->dest=LACAN_ID_GE
//2 es la ID del GEN. cuando debugueo me pone 3, en la funcion, no 2
//si paras el timer que corre en la pantalla estado de red, esto anda perfecto
 //creo que cuando mandas los qrys de estado de red te pisan este valor
//*********************************************************************

    time_2sec = new QTimer();

//COMBO BOX MODO

    //ui->combo_modo->addItem("Velocidad (0)",QVariant(0));
    //ui->combo_modo->addItem("Potencia (1)",QVariant(1));
    //ui->combo_modo->addItem("Torque (2)",QVariant(2));
    //ui->combo_modo->addItem("MPPT (3)",QVariant(3));

    ui->combo_modo->addItem("Velocidad (0)",QVariant(LACAN_VAR_MOD_VEL));
    ui->combo_modo->addItem("Potencia (1)",QVariant(LACAN_VAR_MOD_POT));
    ui->combo_modo->addItem("Torque (2)",QVariant(LACAN_VAR_MOD_TORQ));
    ui->combo_modo->addItem("MPPT (3)",QVariant(LACAN_VAR_MOD_MPPT));
    on_combo_modo_currentIndexChanged(0);
    mode_changed();

    connect(ui->combo_modo,SIGNAL(activated(int)),this,SLOT(mode_changed()));

//SPIN

    ui->spin_isd_ref->setValue(0);
    ui->spin_lim_ibat->setValue(0);
    ui->spin_lim_ief->setValue(0);
    ui->spin_lim_vdc->setValue(0);
    ui->spin_pot_ref->setValue(0);
    ui->spin_speed_ref->setValue(0);
    ui->spin_torque_ref->setValue(0);

    set_limits_gen();

    new_mode();             //para que queden en grises los que correspondan
    set_spin_click(false);  //todo desclickeado. Cuando algo es clickeado, deja de refrescar, para que pueda escribir

//LABELS
    ui->label_gen_io->setText("----");
    ui->label_gen_vo->setText("----");
    ui->label_gen_ibat->setText("----");
    ui->label_gen_po->setText("----");
    ui->label_gen_vel->setText("----");
    ui->label_gen_tor->setText("----");

//TIMER
    connect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));
    time_2sec->start(10000);      //le pongo 10 para probar, cambiar a 2  <---------------------


    //****************en la inicializacion hay que preguntar el modo tambien
    //y desp de la inicializacion hay que volver ap reguntar?

}

void Gen_Eolico::timer_handler(){
    if(mw->gen_connected){         //***********esto hay que modificarlo con lo de luli
        refresh_values();       //actualiza los valores de la pantalla
        send_qry();             //y vuelve a preguntar con los actuales
    }
    else{   //si no esta conectado, se cierra la pantalla
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this,"Conexion perdida","El generador se ha desconectado de la red. Esta ventana se cerrara inmediatamente");
        if(reply){
            this->close();
        }
    }
}

void Gen_Eolico::on_pushButton_apply_clicked(){
    QMessageBox::StandardButton reply;
    QString str="Esta seguro que desea aplicar los cambios?";
    //str.append(ui->combo_modo->currentText());
    reply = QMessageBox::question(this,"Confirm",str, QMessageBox::Yes | QMessageBox::No );
    if(reply==QMessageBox::Yes){
        enviar_variables_generales(); //envio las variables que no dependen del modo

        //envio el modo actual
        data_can modo;
        modo.var_char[0] = actual_mode;
        modo.var_char[1] = 0;
        modo.var_char[2] = 0;
        modo.var_char[3] = 0;
        LACAN_Set(mw,LACAN_VAR_MOD,modo,1);
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
        mw->agregar_log_sent();

        //Envio la variable de setpoint, dependiendo del modo
        switch(actual_mode){
            case LACAN_VAR_MOD_TORQ: //torque
                qDebug()<<"TORQUE";
                val.var_float=ui->spin_torque_ref->value();
                LACAN_Set(mw,LACAN_VAR_TORQ_SETP,val,1);
                connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
                mw->agregar_log_sent();
                break;
            case LACAN_VAR_MOD_MPPT: //mppt
                qDebug()<<"MPPT";
                //val.var_float=ui->spin_torque_ref->value();
                //LACAN_Set(mw,LACAN_VAR_TORQ_SETP,val,1);
                //connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
                //mw->agregar_log_sent();
                break;
            case LACAN_VAR_MOD_VEL: //velocidad      //no me deja poner este primero..
                qDebug()<<"VELOCIDAD";
                val.var_float=ui->spin_speed_ref->value();
                LACAN_Set(mw,LACAN_VAR_W_SETP,val,1);
                connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
                mw->agregar_log_sent();
                break;
            case LACAN_VAR_MOD_POT: //potencia
                qDebug()<<"POTENCIA";
                val.var_float=ui->spin_pot_ref->value();
                LACAN_Set(mw,LACAN_VAR_PO_SETP,val,1);
                connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
                mw->agregar_log_sent();
                break;
            default:
               break;
                }
        }
    else{
    on_pushButton_cancel_clicked();
    }
}




//En esta funcion se envia el set de las variables que son comunes a todos los modos
void Gen_Eolico::enviar_variables_generales(){
    data_can val;

    val.var_float=ui->spin_isd_ref->value();
    LACAN_Set(mw,LACAN_VAR_ISD_SETP,val,1);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();

    val.var_float=ui->spin_lim_ibat->value();
    LACAN_Set(mw,LACAN_VAR_I_BAT_SETP,val,1);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();

    val.var_float=ui->spin_lim_ief->value();
    LACAN_Set(mw,LACAN_VAR_IEF_SETP,val,1);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();

    val.var_float=ui->spin_lim_vdc->value();
    LACAN_Set(mw,LACAN_VAR_VO_SETP,val,1);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

//me fijo que variable es la que llego, y le asigno el valor correspondiente, a la variable propia de la clase
void Gen_Eolico::GENpost_Handler(LACAN_MSG msg){
      recibed_val.var_char[0]=msg.BYTE2;
      recibed_val.var_char[1]=msg.BYTE3;
      recibed_val.var_char[2]=msg.BYTE4;
      recibed_val.var_char[3]=msg.BYTE5;
        switch (msg.BYTE1) {

        case LACAN_VAR_MOD:
            actual_mode=recibed_val.var_char[0];
            ui->combo_modo->setCurrentIndex(actual_mode);
            new_mode();
        break;

        case LACAN_VAR_VO_INST:
            gen_vo = recibed_val.var_float;
        break;
        case LACAN_VAR_IO_INST:
            gen_io = recibed_val.var_float;
        break;
        case LACAN_VAR_PO_INST:
            gen_po = recibed_val.var_float;
        break;
        case LACAN_VAR_W_INST:
            gen_vel = recibed_val.var_float;
        break;
        case LACAN_VAR_TORQ_INST:
            gen_tor = recibed_val.var_float;
        break;
        case LACAN_VAR_I_BAT_INST:
            gen_ibat = recibed_val.var_float;
        break;

        case LACAN_VAR_VO_SETP:
            lim_vdc = recibed_val.var_float;
        break;
        case LACAN_VAR_W_SETP:
            speed_ref=recibed_val.var_float;
        break;
        case LACAN_VAR_TORQ_SETP:
            torque_ref=recibed_val.var_float;
        break;
        case LACAN_VAR_PO_SETP:
            pot_ref = recibed_val.var_float;
        break;
        case LACAN_VAR_I_BAT_SETP: //o la de setpoint
            lim_ibat = recibed_val.var_float;
        break;
        case LACAN_VAR_IEF_SETP:
            lim_ief = recibed_val.var_float;
        break;
        case LACAN_VAR_ISD_SETP:
            isd_ref = recibed_val.var_float;
        break;
    default:
        break;
    }
}

//consulto todas las variables del gen
void Gen_Eolico::send_qry(){

    LACAN_Query(mw,LACAN_VAR_VO_INST);  //gen_veo
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_IO_INST);  //gen_io
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_I_BAT_INST);   //gen_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_W_INST);   //gen_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_TORQ_INST);   //gen_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_PO_INST);   //gen_po
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

    LACAN_Query(mw,LACAN_VAR_W_SETP);   //sped_ref
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_PO_SETP);   //po_ref
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_TORQ_SETP);   //torq_ref
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_IEF_SETP);   //ief
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_ISD_SETP);   //isd
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_I_BAT_SETP);   //lim_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    LACAN_Query(mw,LACAN_VAR_VO_SETP);   //lim_vdc
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

    LACAN_Query(mw,LACAN_VAR_MOD);   //modo
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
}

//se actualizan los valores de los spins que no estan siendo editados en este momento
void Gen_Eolico::refresh_values(){
    if(!speed_ref_click)
        ui->spin_speed_ref->setValue(speed_ref);
    if(!pot_ref_click)
        ui->spin_pot_ref->setValue(pot_ref);
    if(!lim_vdc_click)
        ui->spin_lim_vdc->setValue(lim_vdc);
    if(!lim_ief_click)
        ui->spin_lim_ief->setValue(lim_ief);
    if(!torque_ref_click)
        ui->spin_torque_ref->setValue(torque_ref);
    if(!isd_ref_click)
        ui->spin_isd_ref->setValue(isd_ref);
    if(!lim_ibat_click)
        ui->spin_lim_ibat->setValue(lim_ibat);

    ui->label_gen_vo->setText(QString::number(gen_vo));
    ui->label_gen_io->setText(QString::number(gen_io));
    ui->label_gen_ibat->setText(QString::number(gen_ibat));
    ui->label_gen_po->setText(QString::number(gen_po));
    ui->label_gen_tor->setText(QString::number(gen_tor));
    ui->label_gen_vel->setText(QString::number(gen_vel));
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

void Gen_Eolico::on_pushButton_cancel_clicked(){
    ui->combo_modo->setCurrentIndex(actual_mode);   //se vuelve al modo actual
    new_mode();                                     //habilitar los campos correspondientes al modo actual
}

void Gen_Eolico::mode_changed(){
    new_mode();
    set_spin_click(false);  //poner los valores reales en el spin
    refresh_values();       //todo desclickeado
}

//te dice si los spin esta clickeado o no (state=true o state=false)
void Gen_Eolico::set_spin_click(bool state){
    speed_ref_click=state;
    pot_ref_click=state;
    lim_vdc_click=state;
    iconv_click=state;
    lim_ief_click=state;
    torque_ref_click=state;
    isd_ref_click=state;
    lim_ibat_click=state;
}

//habilita y deshabilita los campos, dependiendo el modo actual
void Gen_Eolico::new_mode(){

    qDebug()<<"Numero de Modo: "<<ui->combo_modo->currentIndex();
    switch (actual_mode) {
    case LACAN_VAR_MOD_VEL:     //Velocidad
        qDebug()<<"Entro a velocidad";
        ui->spin_pot_ref->setDisabled(true);
        ui->spin_speed_ref->setEnabled(true);
        ui->spin_torque_ref->setDisabled(true);
        break;
    case LACAN_VAR_MOD_POT:     //Potencia
        qDebug()<<"Entro a potencia";
        ui->spin_pot_ref->setEnabled(true);
        ui->spin_speed_ref->setDisabled(true);
        ui->spin_torque_ref->setDisabled(true);
        break;
    case LACAN_VAR_MOD_TORQ:     //Torque
        qDebug()<<"Entro a Torke";
        ui->spin_pot_ref->setDisabled(true);
        ui->spin_speed_ref->setDisabled(true);
        ui->spin_torque_ref->setEnabled(true);
        break;
    case LACAN_VAR_MOD_MPPT:     //MPPT

        qDebug()<<"Entro a Mppt";
        ui->spin_pot_ref->setDisabled(true);
        ui->spin_speed_ref->setDisabled(true);
        ui->spin_torque_ref->setDisabled(true);
        break;
    default:
        break;
    }
}

//setea los limites de los spinbox, al construir la clase
void Gen_Eolico::set_limits_gen(){
    ui->spin_lim_ibat->setMaximum(LACAN_VAR_GEN_IBAT_MAX);
    ui->spin_lim_ibat->setMinimum(LACAN_VAR_GEN_IBAT_MIN);
    ui->spin_lim_ibat->setDecimals(3);

    ui->spin_lim_vdc->setMaximum(LACAN_VAR_GEN_VO_MAX);
    ui->spin_lim_vdc->setMinimum(LACAN_VAR_GEN_VO_MIN);
    ui->spin_lim_vdc->setDecimals(3);

    ui->spin_speed_ref->setMaximum(LACAN_VAR_GEN_W_MAX);
    ui->spin_speed_ref->setMinimum(LACAN_VAR_GEN_W_MIN);
    ui->spin_speed_ref->setDecimals(3);

    ui->spin_pot_ref->setMaximum(LACAN_VAR_GEN_PO_MAX);
    ui->spin_pot_ref->setMinimum(LACAN_VAR_GEN_PO_MIN);
    ui->spin_pot_ref->setDecimals(3);

    ui->spin_torque_ref->setMaximum(LACAN_VAR_GEN_TORQ_MAX);
    ui->spin_torque_ref->setMinimum(LACAN_VAR_GEN_TORQ_MIN);
    ui->spin_torque_ref->setDecimals(3);

    ui->spin_lim_ief->setMaximum(LACAN_VAR_GEN_IEF_MAX);
    ui->spin_lim_ief->setMinimum(LACAN_VAR_GEN_IEF_MIN);
    ui->spin_lim_ief->setDecimals(3);

    ui->spin_isd_ref->setMaximum(LACAN_VAR_GEN_ISD_MAX);
    ui->spin_isd_ref->setMinimum(LACAN_VAR_GEN_ISD_MIN);
    ui->spin_isd_ref->setDecimals(3);

}

void Gen_Eolico::closeEvent(QCloseEvent *e){
    time_2sec->stop();
    delete time_2sec;
    QDialog::closeEvent(e);
}

Gen_Eolico::~Gen_Eolico()
{
    delete ui;
}

void Gen_Eolico::on_spin_lim_ibat_valueChanged(double arg1)
{
    lim_ibat = true;
}


void Gen_Eolico::on_spin_lim_vdc_valueChanged(double arg1)
{
    lim_vdc_click = true;
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



void Gen_Eolico::on_combo_modo_currentIndexChanged(int index)
{
   actual_mode = ui->combo_modo->itemData(index).toInt();
}
