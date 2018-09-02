#include "gen_eolico.h"
#include "ui_gen_eolico.h"
#include <QMessageBox>
#include <QtCore>
#include <QtGui>
#include <QString>
#include <QTimer>

Gen_Eolico::Gen_Eolico(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Gen_Eolico)
{
    ui->setupUi(this);

    time_2sec = new QTimer();

//COMBO BOX MODO
    ui->combo_modo->addItem("Velocidad (0)");
    ui->combo_modo->addItem("Potencia (1)");
    ui->combo_modo->addItem("Torque (2)");
    ui->combo_modo->addItem("Potencia (3)");
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

    ui->lineEdit_iconv->setInputMask("99999");         //para insertar solo numeros
    ui->lineEdit_isd_ref->setInputMask("99999");
    ui->lineEdit_lim_ibat->setInputMask("99999");
    ui->lineEdit_lim_ief->setInputMask("99999");
    ui->lineEdit_lim_vdc->setInputMask("99999");
    ui->lineEdit_pot_ref->setInputMask("99999");
    ui->lineEdit_speed_ref->setInputMask("99999");
    ui->lineEdit_torque_ref->setInputMask("99999");
    ui->lineEdit_vdc->setInputMask("99999");
    ui->lineEdit_ibat->setInputMask("99999");

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

void Gen_Eolico::mode_changed(){
    new_mode();
    set_lineEdit_click(false);
    refresh_values();
}



void Gen_Eolico::new_mode(){
    switch (ui->combo_modo->currentIndex()) {
    case 0:     //Velocidad
        ui->lineEdit_iconv->setDisabled(true);
        ui->lineEdit_isd_ref->setDisabled(true);
        ui->lineEdit_lim_ibat->setDisabled(true);
        ui->lineEdit_lim_ief->setDisabled(true);
        ui->lineEdit_lim_vdc->setDisabled(true);
        ui->lineEdit_pot_ref->setDisabled(true);
        ui->lineEdit_speed_ref->setEnabled(true);
        ui->lineEdit_torque_ref->setDisabled(true);
        ui->lineEdit_vdc->setDisabled(true);
        ui->lineEdit_ibat->setDisabled(true);
        break;
    case 1:     //Potencia
        ui->lineEdit_iconv->setDisabled(true);
        ui->lineEdit_isd_ref->setDisabled(true);
        ui->lineEdit_lim_ibat->setDisabled(true);
        ui->lineEdit_lim_ief->setDisabled(true);
        ui->lineEdit_lim_vdc->setDisabled(true);
        ui->lineEdit_pot_ref->setEnabled(true);
        ui->lineEdit_speed_ref->setDisabled(true);
        ui->lineEdit_torque_ref->setDisabled(true);
        ui->lineEdit_vdc->setDisabled(true);
        ui->lineEdit_ibat->setDisabled(true);
        break;
    case 2:     //Torque
        ui->lineEdit_iconv->setDisabled(true);
        ui->lineEdit_isd_ref->setDisabled(true);
        ui->lineEdit_lim_ibat->setDisabled(true);
        ui->lineEdit_lim_ief->setDisabled(true);
        ui->lineEdit_lim_vdc->setDisabled(true);
        ui->lineEdit_pot_ref->setDisabled(true);
        ui->lineEdit_speed_ref->setDisabled(true);
        ui->lineEdit_torque_ref->setEnabled(true);
        ui->lineEdit_vdc->setDisabled(true);
        ui->lineEdit_ibat->setDisabled(true);
        break;
    case 3:     //Potencia
        ui->lineEdit_iconv->setDisabled(true);
        ui->lineEdit_isd_ref->setDisabled(true);
        ui->lineEdit_lim_ibat->setDisabled(true);
        ui->lineEdit_lim_ief->setDisabled(true);
        ui->lineEdit_lim_vdc->setDisabled(true);
        ui->lineEdit_pot_ref->setEnabled(true);
        ui->lineEdit_speed_ref->setDisabled(true);
        ui->lineEdit_torque_ref->setDisabled(true);
        ui->lineEdit_vdc->setDisabled(true);
        ui->lineEdit_ibat->setDisabled(true);
        break;
    default:
        break;
    }
}

void Gen_Eolico::timer_handler(){
    refresh_values();
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
        break;
       case LACAN_VAR_IO:
            pot_ref=msg.BYTE2;
        break;*/
        case LACAN_VAR_IO:
            speed_ref=msg.BYTE2;
        break;
        /*case LACAN_VAR_IO:
            torque_ref=msg.BYTE2;
        break;
        case LACAN_VAR_IO:
            vdc=msg.BYTE2;
        break;
        case LACAN_VAR_IO:
            ibat=msg.BYTE2;
        break;*/
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
