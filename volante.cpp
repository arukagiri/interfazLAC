#include "volante.h"
#include "ui_volante.h"
#include <QMessageBox>
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include "PC.h"

volante::volante(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::volante)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow*>(this->parent());


//Configuracion del CombBox para los Modos
    ui->combo_modo->addItem("Volante de Inercia (0)",QVariant(LACAN_VAR_MOD));
    ui->combo_modo->addItem("Variador de Velocidad (1)",QVariant(LACAN_VAR_MOD_INER));
    connect(ui->combo_modo,SIGNAL(activated(int)),this,SLOT(verificar_mode_changed()));
    on_combo_modo_currentIndexChanged(0);
    mode_changed();


//Inicializacion de Labels
    ui->label_vol_io->setText("----");
    ui->label_vol_vo->setText("----");
    ui->label_vol_ibat->setText("----");
    ui->label_vol_po->setText("----");
    ui->label_vol_vel->setText("----");
    ui->label_vol_tor->setText("----");
    ui->label_vol_ener->setText("----");

    ui->label_speed_ref->setText("----");
    ui->label_id_ref->setText("----");
    ui->label_standbay_speed_ref->setText("----");


//TIMER ENCARGADO DE REFRESCAR LOS VALORES Y DE ENVIAR LAS NUEVAS CONSULTAS
    time_2sec = new QTimer();
    connect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));
    time_2sec->start(2000); //velocidad de refresco (en ms)

    send_qry(); //envio las primeras consultas
}

void volante::timer_handler(){
    if(mw->device_is_connected(LACAN_ID_VOLANTE)){
        refresh_values();       //actualiza los valores de la pantalla
        send_qry();             //y vuelve a preguntar con los actuales
    }
    else{   //si no esta conectado, se cierra la pantalla
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this,"Conexion perdida","El volante se ha desconectado de la red. Esta ventana se cerrara inmediatamente");
        if(reply){
            this->close();
        }
    }
}




void volante::VOLpost_Handler(LACAN_MSG msg){
    recibed_val.var_char[0]=msg.BYTE2;
    recibed_val.var_char[1]=msg.BYTE3;
    recibed_val.var_char[2]=msg.BYTE4;
    recibed_val.var_char[3]=msg.BYTE5;
    switch (msg.BYTE1) {
        case LACAN_VAR_VO_INST:
            vol_vo = recibed_val.var_float;
        break;
        case LACAN_VAR_IO_INST:
            vol_io = recibed_val.var_float;
        break;
        case LACAN_VAR_PO_INST:
            vol_po = recibed_val.var_float;
        break;
        case LACAN_VAR_W_INST:
            vol_vel = recibed_val.var_float;    //calcular energia ****************************************************************************
        break;
        case LACAN_VAR_TORQ_INST:
            vol_tor = recibed_val.var_float;
        break;
        case LACAN_VAR_I_BAT_INST:
            vol_ibat = recibed_val.var_float;
        break;

        case LACAN_VAR_W_SETP:
            speed_ref=recibed_val.var_float;
        break;
        case LACAN_VAR_STANDBY_W_SETP:
            standby_ref=recibed_val.var_float;
        break;
        case LACAN_VAR_ID_SETP:
            id_ref=recibed_val.var_float;
        break;
        case LACAN_VAR_MOD:
            actual_mode=recibed_val.var_char[0];
            ui->combo_modo->setCurrentIndex(ui->combo_modo->findData(actual_mode));
            mode_changed();
    default:
        break;
    }
}

void volante::send_qry(){

    mw->LACAN_Query(LACAN_VAR_VO_INST,false,dest);  //vol_vo
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_IO_INST,false,dest);  //vol_io
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_I_BAT_INST,false,dest);   //vol_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_W_INST,false,dest);   //vol_vel
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_TORQ_INST,false,dest);   //vol_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_PO_INST,false,dest);   //vol_po
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

    mw->LACAN_Query(LACAN_VAR_W_SETP,false,dest);   //sped_ref
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_ID_SETP,false,dest);   //id_ref
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_STANDBY_W_SETP,false,dest);   //standby_ref
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

    mw->LACAN_Query(LACAN_VAR_MOD,false,dest);   //modo
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
}

void volante::refresh_values(){

    ui->label_speed_ref->setText(QString::number(speed_ref,'f',2));
    ui->label_id_ref->setText(QString::number(id_ref,'f',2));
    ui->label_standbay_speed_ref->setText(QString::number(standby_ref,'f',2));

    ui->label_vol_vo->setText(QString::number(vol_vo,'f',2));
    ui->label_vol_io->setText(QString::number(vol_io,'f',2));
    ui->label_vol_ibat->setText(QString::number(vol_ibat,'f',2));
    ui->label_vol_po->setText(QString::number(vol_po,'f',2));
    ui->label_vol_tor->setText(QString::number(vol_tor,'f',2));
    ui->label_vol_vel->setText(QString::number(vol_vel,'f',2));
    ui->label_vol_vel->setText(QString::number(vol_ener,'f',2));  //ver donde se hace el calculo de esto ********
}

void volante::on_pushButton_start_clicked()
{
    cmd=LACAN_CMD_START;
    mw->LACAN_Do(cmd,false,dest);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

void volante::on_pushButton_stop_clicked()
{
    cmd=LACAN_CMD_STOP;
    mw->LACAN_Do(cmd,false,dest);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

void volante::on_pushButton_comandar_clicked()
{
    Comandar *comwin = new Comandar(mw,dest);
    comwin->setModal(true);
    comwin->show();
}

void volante::verificar_mode_changed(){
    QMessageBox::StandardButton reply;
    QString str="¿Esta seguro que desea cambiar al modo ";
    str.append(ui->combo_modo->currentText());
    str.append(" ?");
    reply = QMessageBox::question(this,"Confirm",str, QMessageBox::Yes | QMessageBox::No );
    if(reply==QMessageBox::Yes){
        data_can modo;
        modo.var_char[0] = actual_mode;
        modo.var_char[1] = 0;
        modo.var_char[2] = 0;
        modo.var_char[3] = 0;
        mw->LACAN_Set(LACAN_VAR_MOD,modo,false,dest);
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
        mw->agregar_log_sent();
    }
    else{
        actual_mode=previous_mode;
    }
}

void volante::mode_changed(){
    new_mode();
    refresh_values();
}

//habilita y deshabilita los campos, dependiendo el modo actual
void volante::new_mode(){
    switch (actual_mode) {
    case LACAN_VAR_MOD_VEL:     //Velocidad
        ui->label_standbay_speed_ref->setDisabled(true);
        ui->label_speed_ref->setEnabled(true);
        break;
    case LACAN_VAR_MOD_INER:     //Inercia
        ui->label_standbay_speed_ref->setEnabled(true);
        ui->label_speed_ref->setDisabled(true);
        break;
    default:
        break;
    }
}

void volante::closeEvent(QCloseEvent *e){
    time_2sec->stop();
    delete time_2sec;

    emit volWindowsClosed();

    QDialog::closeEvent(e);
}

volante::~volante()
{
    delete ui;
}

void volante::on_combo_modo_currentIndexChanged(int index)
{
   previous_mode = actual_mode;     //guardo el modo anterior por si el usuario cancela el cambio
   actual_mode = ui->combo_modo->itemData(index).toInt();
}
