#include "gen_eolico.h"
#include "ui_gen_eolico.h"
#include <QMessageBox>
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include "PC.h"
#include "LACAN_SEND.h"


//quiero que cuando se abra la ventana quede seleccionado por defecto el boton de cancelar
//eso se hacia desde la gui asignandole el orden de cuando apretas tabs, pero lo tengo que ver
Gen_Eolico::Gen_Eolico(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Gen_Eolico)
{
    ui->setupUi(this);

    mw = qobject_cast<MainWindow*>(this->parent());
    //mw->dest=LACAN_ID_GEN; //2 es la ID del GEN. cuando debugueo me pone 3, en la funcion, no 2
                             //si paras el timer que corre en la pantalla estado de red, esto anda perfecto
                             //creo que cuando mandas los qys de estado de red te pisan este valor

    time_2sec = new QTimer();

//COMBO BOX MODO
    ui->combo_modo->addItem("Velocidad (0)");
    ui->combo_modo->addItem("Potencia (1)");
    ui->combo_modo->addItem("Torque (2)");
    ui->combo_modo->addItem("MPPT (3)");
    actual_mode=ui->combo_modo->currentIndex();

    connect(ui->combo_modo,SIGNAL(activated(int)),this,SLOT(mode_changed()));

//LINES EDIT
    ui->lineEdit_iconv->setText("99999");
    ui->lineEdit_isd_ref->setText("99999");
    ui->lineEdit_lim_ibat->setText("99999");
    ui->lineEdit_lim_ief->setText("99999");
    ui->lineEdit_lim_vdc->setText("99999");
    ui->lineEdit_pot_ref->setText("00000");

    ui->lineEdit_speed_ref->setText("x9999");
    //ui->lineEdit_speed_ref->setText("#9999");
   //    ui->lineEdit_speed_ref->setText("99999;#");

    ui->lineEdit_torque_ref->setText("00000");
    ui->lineEdit_vdc->setText("00000");
    ui->lineEdit_ibat->setText("00000");

    ui->label_gen_io->setText("----");
    ui->label_gen_vo->setText("----");

   /* ui->lineEdit_iconv->setInputMask("99999");         //para insertar solo numeros
    ui->lineEdit_isd_ref->setInputMask("99999");
    ui->lineEdit_lim_ibat->setInputMask("99999");
    ui->lineEdit_lim_ief->setInputMask("99999");
    ui->lineEdit_lim_vdc->setInputMask("99999");
    ui->lineEdit_pot_ref->setInputMask("99999");
    ui->lineEdit_speed_ref->setInputMask("99999");
    ui->lineEdit_torque_ref->setInputMask("99999");
    ui->lineEdit_vdc->setInputMask("99999");
    ui->lineEdit_ibat->setInputMask("99999");*/

    new_mode();                 //para que queden en grises los que correspondan
    set_lineEdit_click(false);  //todo desclickeado


//TIMER
    connect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));
    time_2sec->start(2000);
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
    LACAN_Do(mw,cmd);
    mw->agregar_log_sent();
}

void Gen_Eolico::on_pushButton_stop_clicked()
{
    cmd=LACAN_CMD_STOP;
    mw->dest=LACAN_ID_GEN;
    LACAN_Do(mw,cmd);
    mw->agregar_log_sent();
}


void Gen_Eolico::mode_changed(){
    new_mode();
    set_lineEdit_click(false);
    refresh_values();
}

void Gen_Eolico::new_mode(){
    switch (ui->combo_modo->currentIndex()) {
    case 0:     //Velocidad
        ui->lineEdit_pot_ref->setDisabled(true);
        ui->lineEdit_speed_ref->setEnabled(true);
        ui->lineEdit_torque_ref->setDisabled(true);
        //ui->lineEdit_vdc->setDisabled(true);
        //ui->lineEdit_ibat->setDisabled(true);
        //ui->lineEdit_iconv->setDisabled(true);
        //ui->lineEdit_isd_ref->setDisabled(true);
        //ui->lineEdit_lim_ibat->setDisabled(true);S
        //ui->lineEdit_lim_ief->setDisabled(true);
        //ui->lineEdit_lim_vdc->setDisabled(true);
        break;
    case 1:     //Potencia
        ui->lineEdit_pot_ref->setEnabled(true);
        ui->lineEdit_speed_ref->setDisabled(true);
        ui->lineEdit_torque_ref->setDisabled(true);
        break;
    case 2:     //Torque
        ui->lineEdit_pot_ref->setDisabled(true);
        ui->lineEdit_speed_ref->setDisabled(true);
        ui->lineEdit_torque_ref->setEnabled(true);
        break;
    case 3:     //Potencia
        ui->lineEdit_pot_ref->setDisabled(true);
        ui->lineEdit_speed_ref->setDisabled(true);
        ui->lineEdit_torque_ref->setDisabled(true);
        break;
    default:
        break;
    }
}

void Gen_Eolico::timer_handler(){
    if(mw->gen_connected){
        refresh_values();
        //send_qry();
    }
    else{
        //QMessageBox win_war = QMessageBox::warning(this,"Gen Lost","El generador se ha desconectado de la red");
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this,"Gen Lost","El generador se ha desconectado de la red");
        if(reply){
            this->close();
        }
    }
}

void Gen_Eolico::send_qry(){
    //lista de variables a consultar
    LACAN_Query(mw,LACAN_VAR_VO);
    LACAN_Query(mw,LACAN_VAR_IO);

}

//se actualizan los valores de los lineEdits que no estan siendo editados en este momento
void Gen_Eolico::refresh_values(){
    if(!speed_ref_click)
        ui->lineEdit_speed_ref->setText(QString::number(speed_ref));
    if(!pot_ref_click)
        ui->lineEdit_pot_ref->setText(QString::number(pot_ref));
    if(!lim_vdc_click)
        ui->lineEdit_lim_vdc->setText(QString::number(lim_vdc));
    if(!vdc_click)
        ui->lineEdit_vdc->setText(QString::number(vdc));
    if(!iconv_click)
        ui->lineEdit_iconv->setText(QString::number(iconv));
    if(!lim_ief_click)
        ui->lineEdit_lim_ief->setText(QString::number(lim_ief));
    if(!torque_ref_click)
        ui->lineEdit_torque_ref->setText(QString::number(torque_ref));
    if(!isd_ref_click)
        ui->lineEdit_isd_ref->setText(QString::number(isd_ref));
    if(!lim_ibat_click)
        ui->lineEdit_lim_ibat->setText(QString::number(lim_ibat));
    if(!ibat_click)
        ui->lineEdit_ibat->setText(QString::number(ibat));

    ui->label_gen_vo->setText(QString::number(gen_vo));
    ui->label_gen_io->setText(QString::number(gen_io));
}


//VER CUAL CORRESPONDE A CADA VARIABLE.............
void Gen_Eolico::GENpost_Handler(LACAN_MSG msg){
        switch (msg.BYTE1) {
        /*case LACAN_VAR_IO:  //esta cual es? IO esta de prueba
            iconv=msg.BYTE2;
        break;*/
        case LACAN_VAR_ISD_SETP:
            isd_ref=msg.BYTE2;
        break;
        case LACAN_VAR_BAT_IMAX:
            lim_ibat=msg.BYTE2;
        break;
        case LACAN_VAR_IEF_MAX:
            lim_ief=msg.BYTE2;
        break;
         /*case LACAN_VAR_IO:
            lim_vdc=msg.BYTE2;
        break;*/
       case LACAN_VAR_PI_SETP:
            pot_ref=msg.BYTE2;
        break;
        case LACAN_VAR_W_SETP:
            speed_ref=msg.BYTE2;
        break;
        case LACAN_VAR_TORQI_SETP:
            torque_ref=msg.BYTE2;
        break;
        /*case LACAN_VAR_IO:
            vdc=msg.BYTE2;
        break;*/
       /* case LACAN_VAR_BAT_I: //o la de setpoint
            ibat=msg.BYTE2;
        break;*/
       case LACAN_VAR_IO:
            gen_io=msg.BYTE2;
        break;
        case LACAN_VAR_VO:
            gen_vo=msg.BYTE2;
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
        actual_mode = ui->combo_modo->currentIndex();
        switch(actual_mode){
                //verificart si el line edit no esta vacio
                //se puede, pero no es necesario, verificar que haya habido un cambio en el valor del setpoint
            case 3: //mppt
                qDebug()<<"caso3";
                //LACAN_Set(mw,LACAN_VAR_MOD_MPPT,true);
            break;
            case 2: //torque
                qDebug()<<"caso2";
                //LACAN_Set(mw,LACAN_VAR_MOD_TORQ,true);
            break;
            case 1: //potencia
                qDebug()<<"caso1";
                //LACAN_Set(mw,LACAN_VAR_MOD_POT,true);
            break;
            case 0: //velocidad      //no me deja poner este primero..
               //LACAN_Set(mw,LACAN_VAR_MOD_VEL,true);
               uint32_t data_int = ui->lineEdit_speed_ref->text().toInt();
               float data_float = ui->lineEdit_speed_ref->text().toFloat();
               if(data_int == data_float)
                   val.var_int = data_int;
               else
                   val.var_float = data_float;
               LACAN_Set(mw,LACAN_VAR_IO,val);
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
    set_lineEdit_click(false);
}



void Gen_Eolico::set_lineEdit_click(bool state){
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

void Gen_Eolico::on_lineEdit_speed_ref_cursorPositionChanged(int arg1, int arg2)
{
   // speed_ref_click=true;
}

void Gen_Eolico::on_lineEdit_speed_ref_textEdited(const QString &arg1)
{
    speed_ref_click=true;
}


void Gen_Eolico::on_lineEdit_pot_ref_textChanged(const QString &arg1)
{
    pot_ref_click=true;
}

void Gen_Eolico::on_lineEdit_lim_vdc_textChanged(const QString &arg1)
{
    lim_vdc_click=true;
}

void Gen_Eolico::on_lineEdit_vdc_textChanged(const QString &arg1)
{
    vdc_click=true;
}

void Gen_Eolico::on_lineEdit_iconv_textChanged(const QString &arg1)
{
    iconv_click=true;
}

void Gen_Eolico::on_lineEdit_lim_ief_textChanged(const QString &arg1)
{
    lim_ief_click=true;
}

void Gen_Eolico::on_lineEdit_torque_ref_textChanged(const QString &arg1)
{
    torque_ref_click=true;
}

void Gen_Eolico::on_lineEdit_isd_ref_textChanged(const QString &arg1)
{
   isd_ref_click=true;
}

void Gen_Eolico::on_lineEdit_lim_ibat_textChanged(const QString &arg1)
{
    lim_ibat_click=true;
}

void Gen_Eolico::on_lineEdit_ibat_textChanged(const QString &arg1)
{
    ibat_click=true;
}

void Gen_Eolico::closeEvent(QCloseEvent *e){
    time_2sec->stop();
    delete time_2sec;
    QDialog::closeEvent(e);
}

