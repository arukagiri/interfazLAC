#include "volante.h"
#include "ui_volante.h"
#include <QMessageBox>
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include "PC.h"
#include <QtMath>

volante::volante(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::volante)
{
    ui->setupUi(this);

    this->setWindowTitle("Volante de Inercia");

    this->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    mw = qobject_cast<MainWindow*>(this->parent());

    send_queries = true;

//Inicializacion de Labels
    ui->label_vol_io->setText("----");
    ui->label_vol_vo->setText("----");
    ui->label_vol_ibat->setText("----");
    ui->label_vol_po->setText("----");
    ui->label_vol_vel->setText("----");
    ui->label_vol_tor->setText("----");
    ui->label_vol_ener->setText("----");

    blockAllSpinSignals(true);

    ui->spin_vol_isd_ref->setMinimum(LACAN_VAR_VOL_ISD_MIN);
    ui->spin_vol_isd_ref->setMaximum(LACAN_VAR_VOL_ISD_MAX);
    ui->spin_vol_sbyspeed_ref->setMinimum(LACAN_VAR_VOL_STANDBY_W_MIN);
    ui->spin_vol_sbyspeed_ref->setMaximum(LACAN_VAR_VOL_STANDBY_W_MAX);

//TIMER ENCARGADO DE REFRESCAR LOS VALORES Y DE ENVIAR LAS NUEVAS CONSULTAS
    time_2sec = new QTimer();
    connect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));
    time_2sec->start(2000); //velocidad de refresco (en ms)

    send_qry_variables(); //envio las primeras consultas
    send_qry_references();
    referenceChanged=false;

    editHotKey = new QShortcut(QKeySequence(tr("Ctrl+E", "Edit")), this);
    connect(editHotKey, SIGNAL(activated()), this, SLOT(changeEditState()));

    ui->label_edit->setDisabled(true);
}

volante::~volante()
{
    delete ui;
    disconnect(editHotKey, SIGNAL(activated()), this, SLOT(changeEditState()));
    delete editHotKey;
    disconnect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));
    delete time_2sec;
}

void volante::timer_handler(){
    static uint count = 0;

    if(mw->device_is_connected(LACAN_ID_VOLANTE)){
//    if(true){
        if(send_queries){
            ui->pushButton_start->blockSignals(false);
            refresh_values();       //actualiza los valores de la pantalla
            count++;
            send_qry_variables();
            if(count%5==0 || referenceChanged){
                send_qry_references();
                referenceChanged = false;
                count = 0;
            }
        }

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
            vol_vel = recibed_val.var_float;
        break;
        case LACAN_VAR_TORQ_INST:
            vol_tor = recibed_val.var_float;
        break;
        case LACAN_VAR_I_BAT_INST:
            vol_ibat = recibed_val.var_float;
        break;
        case LACAN_VAR_STANDBY_W_SETP:
            standby_ref=recibed_val.var_float;
        break;
        case LACAN_VAR_ISD_SETP:
            id_ref=recibed_val.var_float;
        break;
        case LACAN_VAR_MOD:
            actual_mode=recibed_val.var_char[0];
        break;
    default:
        break;
    }
    refresh_values();
}

void volante::send_qry_variables(){
    mw->LACAN_Query(LACAN_VAR_VO_INST,false,dest);  //vol_vo
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_IO_INST,false,dest);  //vol_io
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_I_BAT_INST,false,dest);   //vol_ibat
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_W_INST,false,dest);   //vol_vel
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_TORQ_INST,false,dest);   //vol_ibat
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_PO_INST,false,dest);   //vol_po
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

    mw->LACAN_Query(LACAN_VAR_MOD,false,dest);   //modo
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
}

void volante::send_qry_references(){
    mw->LACAN_Query(LACAN_VAR_ISD_SETP,false,dest);   //id_ref
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_STANDBY_W_SETP,false,dest);   //standby_ref
//    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
}

void volante::refresh_values(){
    if(double(id_ref) > refValue)
        ui->spin_vol_isd_ref->setEnabled(true);
    if(double(standby_ref) > refValue)
        ui->spin_vol_sbyspeed_ref->setEnabled(true);

    ui->spin_vol_sbyspeed_ref->setValue(double(standby_ref));
    ui->spin_vol_isd_ref->setValue(double(id_ref));

    double speedrev = double(vol_vel)*(2*M_PI/60);
    vol_ener = float(0.5 * J * speedrev * speedrev);

    if(double(vol_vo)>refValue)
        ui->label_vol_vo->setText(QString::number(double(vol_vo),'f',2));
    if(double(vol_io)>refValue)
        ui->label_vol_io->setText(QString::number(double(vol_io),'f',2));
    if(double(vol_ibat)>refValue)
        ui->label_vol_ibat->setText(QString::number(double(vol_ibat),'f',2));
    if(double(vol_po)>refValue)
        ui->label_vol_po->setText(QString::number(double(vol_po),'f',2));
    if(double(vol_tor)>refValue)
        ui->label_vol_tor->setText(QString::number(double(vol_tor),'f',2));
    if(double(vol_vel)>refValue)
        ui->label_vol_vel->setText(QString::number(double(vol_vel),'f',2));
    if(double(vol_ener)>refValue)
        ui->label_vol_ener->setText(QString::number(double(vol_ener),'f',2));
    if(double(actual_mode)>refValue){
        switch(actual_mode){
        case LACAN_VAR_MOD_PREARRANQUE:
            ui->label_modo->setText("PREARRANQUE");
            break;
        case LACAN_VAR_MOD_INICIO:
            ui->label_modo->setText("INICIO");
            break;
        case LACAN_VAR_MOD_ARRANQUE:
            ui->label_modo->setText("ARRANQUE");
            break;
        case LACAN_VAR_MOD_COMPENSACION:
            ui->label_modo->setText("CMPENSACION");
            break;
        case LACAN_VAR_MOD_LIMITACION:
            ui->label_modo->setText("LIMITACION");
            break;
        case LACAN_VAR_MOD_APAGADO:
            ui->label_modo->setText("APAGADO");
            break;
        case LACAN_VAR_MOD_RECUPERACION:
            ui->label_modo->setText("RECUPERACION");
            break;
        case LACAN_VAR_MOD_PROTEGIDO:
            ui->label_modo->setText("PROTEGIDO");
            break;
        default:
            ui->label_modo->setText("DESCONCIDO");
        }
    }
}

void volante::on_pushButton_start_clicked()
{
    cmd=LACAN_CMD_START;
    mw->LACAN_Do(cmd,false,dest);
    assert(mw->msg_ack.back());
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

void volante::on_pushButton_stop_clicked()
{
    cmd=LACAN_CMD_STOP;
    mw->LACAN_Do(cmd,false,dest);
    assert(mw->msg_ack.back());
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

void volante::on_pushButton_shutdown_clicked()
{
    cmd=LACAN_CMD_SHUTDOWN;
    mw->LACAN_Do(cmd,false,dest);
    assert(mw->msg_ack.back());
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

void volante::on_pushButton_comandar_clicked()
{
    Comandar *comwin = new Comandar(mw,dest);
    comwin->setModal(true);
    comwin->show();
}

void volante::closeEvent(QCloseEvent *e){
    time_2sec->stop();

    emit volWindowsClosed();

    QDialog::closeEvent(e);
}

void volante::processEditingFinished(QDoubleSpinBox* spin, uint16_t var, float prevValue)
{
    blockAllSpinSignals(true);
    spin->clearFocus();
    data_can data;
    float value = float(spin->value());
    QMessageBox::StandardButton reply;
    QString str = "El valor a enviar es: ";
    str.append(QString::number(double(value)));
    str.append(". Confirma que desea enviar este valor?");
    reply=QMessageBox::question(this,"Valor a enviar",str,QMessageBox::Yes|QMessageBox::No);

    if(reply==QMessageBox::Yes){
        data.var_float = value; //si esta seleccionado algo que no sea modo, manda el valor de spin
        mw->LACAN_Set(var, data, 1, dest);
        mw->agregar_log_sent();
        referenceChanged = true;
    }
    blockAllSpinSignals(false);
    spin->setValue(double(prevValue));
    ui->edit_checkBox->setCheckState(Qt::CheckState::Unchecked);
}

void volante::blockAllSpinSignals(bool b){
    ui->spin_vol_isd_ref->blockSignals(b);
    ui->spin_vol_sbyspeed_ref->blockSignals(b);
}

void volante::on_spin_vol_sbyspeed_ref_editingFinished()
{
    processEditingFinished(ui->spin_vol_sbyspeed_ref, LACAN_VAR_STANDBY_W_SETP, standby_ref);
}

void volante::on_spin_vol_isd_ref_editingFinished()
{
    processEditingFinished(ui->spin_vol_isd_ref, LACAN_VAR_ISD_SETP, id_ref);
}

void volante::on_edit_checkBox_stateChanged(int check)
{
    if(check){
        send_queries = false;

        ui->pushButton_start->blockSignals(true);

        ui->pushButton_comandar->setDisabled(true);
        ui->pushButton_start->setDisabled(true);
        ui->pushButton_stop->setDisabled(true);
        ui->pushButton_shutdown->setDisabled(true);

        ui->spin_vol_sbyspeed_ref->clearFocus();
        ui->spin_vol_isd_ref->clearFocus();

        blockAllSpinSignals(false);

        ui->spin_vol_sbyspeed_ref->setReadOnly(false);
        ui->spin_vol_isd_ref->setReadOnly(false);        

        ui->label_edit->setEnabled(true);

    }else{
        send_queries = true;

        ui->pushButton_comandar->setDisabled(false);
        ui->pushButton_start->setDisabled(false);
        ui->pushButton_stop->setDisabled(false);
        ui->pushButton_shutdown->setDisabled(false);

        blockAllSpinSignals(true);

        ui->spin_vol_sbyspeed_ref->setReadOnly(true);
        ui->spin_vol_isd_ref->setReadOnly(true);

        ui->label_edit->setDisabled(true);
    }
}

void volante::changeEditState()
{
    ui->edit_checkBox->toggle();
}

