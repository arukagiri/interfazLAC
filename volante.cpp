#include "volante.h"
#include "ui_volante.h"

volante::volante(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::volante)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow*>(this->parent());


    //COMBO BOX MODO
    ui->combo_modo->addItem("Volante de Inercia (0)",QVariant(LACAN_VAR_MOD));
    ui->combo_modo->addItem("Variador de Velocidad (1)",QVariant(LACAN_VAR_MOD_INER));
    on_combo_modo_currentIndexChanged(0);
    mode_changed();
    connect(ui->combo_modo,SIGNAL(activated(int)),this,SLOT(mode_changed()));

    //SPIN  
    ui->spin_speed_ref->setValue(0);
    ui->spin_id_ref->setValue(0);
    ui->spin_standbay_speed_ref->setValue(0);
    set_limits_vol();       //aplico los limites a los spin box
    new_mode();             //para que queden en grises los que correspondan
    set_spin_click(false);  //todo desclickeado. Cuando algo es clickeado, deja de refrescar, para que pueda escribir

    //LABELS
    ui->label_vol_io->setText("----");
    ui->label_vol_vo->setText("----");
    ui->label_vol_ibat->setText("----");
    ui->label_vol_po->setText("----");
    ui->label_vol_vel->setText("----");
    ui->label_vol_tor->setText("----");
    ui->label_vol_ener->setText("----");


    //TIMER
    time_2sec = new QTimer();
    connect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));
    time_2sec->start(10000);      //le pongo 10 para probar, cambiar a 2  <---------------------

}

void volante::timer_handler(){
    if(mw->device_is_connected(LACAN_ID_VOLANTE)){        //***********esto hay que modificarlo con lo de luli
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

void volante::on_pushButton_apply_clicked()
{

}

void volante::enviar_variables_generales(){
    data_can val;

    //val.var_float=ui->spin_id_ref->value();
    //LACAN_Set(mw,LACAN_VAR_ID,val,1);
    //connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    //mw->agregar_log_sent();
}

void volante::VOLpost_Handler(LACAN_MSG msg){
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

    default:
        break;
    }
}

void volante::send_qry(){

    mw->LACAN_Query(LACAN_VAR_VO_INST,false,dest);  //vol_veo
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
    if(!speed_ref_click)
        ui->spin_speed_ref->setValue(speed_ref);
    if(!id_ref_click)
        ui->spin_id_ref->setValue(id_ref);
    if(!standby_click)
        ui->spin_standbay_speed_ref->setValue(standby_ref);

    ui->label_vol_vo->setText(QString::number(vol_vo));
    ui->label_vol_io->setText(QString::number(vol_io));
    ui->label_vol_ibat->setText(QString::number(vol_ibat));
    ui->label_vol_po->setText(QString::number(vol_po));
    ui->label_vol_tor->setText(QString::number(vol_tor));
    ui->label_vol_vel->setText(QString::number(vol_vel));
    ui->label_vol_vel->setText(QString::number(vol_ener));  //ver donde se hace el calculo de esto ********
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
    //Comandar *comwin = new Comandar(LACAN_ID_GEN,this);
    //Comandar *comwin = new Comandar(LACAN_ID_GEN,mw);

    Comandar *comwin = new Comandar(mw,dest);
    comwin->setModal(true);
    comwin->show();
}

void volante::on_pushButton_cancel_clicked()
{
    ui->combo_modo->setCurrentIndex(actual_mode);   //se vuelve al modo actual
    new_mode();                                     //habilitar los campos correspondientes al modo actual
}

void volante::mode_changed(){
    new_mode();
    set_spin_click(false);  //poner los valores reales en el spin
    refresh_values();       //todo desclickeado
}

//te dice si los spin esta clickeado o no (state=true o state=false)
void volante::set_spin_click(bool state){
    speed_ref_click = state;
    id_ref_click = state;
    standby_click = state;
}

//habilita y deshabilita los campos, dependiendo el modo actual
void volante::new_mode(){

    qDebug()<<"Numero de Modo: "<<ui->combo_modo->currentIndex();
    switch (actual_mode) {
    case LACAN_VAR_MOD_VEL:     //Velocidad
        qDebug()<<"Entro a velocidad";
        ui->spin_standbay_speed_ref->setDisabled(true);
        ui->spin_speed_ref->setEnabled(true);
        break;
    case LACAN_VAR_MOD_INER:     //Inercia
        qDebug()<<"Entro a inercia";
        ui->spin_standbay_speed_ref->setEnabled(true);
        ui->spin_speed_ref->setDisabled(true);
        break;
    default:
        break;
    }
}

//setea los limites de los spinbox, al construir la clase
void volante::set_limits_vol(){

    ui->spin_speed_ref->setMaximum(LACAN_VAR_VOL_W_MAX);
    ui->spin_speed_ref->setMinimum(LACAN_VAR_VOL_W_MIN);
    ui->spin_speed_ref->setDecimals(3);

   /* ui->spin_id_ref->setMaximum();
    ui->spin_id_ref->setMinimum();
    ui->spin_id_ref->setDecimals(3);

    ui->spin_standbay_speed_ref->setMaximum();
    ui->spin_standbay_speed_ref->setMinimum();
    ui->spin_standbay_speed_ref->setDecimals(3);*/
}

void volante::on_spin_speed_ref_valueChanged(double arg1)
{
    speed_ref_click = true;
}

void volante::on_spin_standbay_speed_ref_valueChanged(double arg1)
{
    standby_click = true;
}

void volante::on_spin_id_ref_valueChanged(double arg1)
{
    id_ref_click = true;
}

void volante::on_combo_modo_currentIndexChanged(int index)
{
   actual_mode = ui->combo_modo->itemData(index).toInt();
}

void volante::closeEvent(QCloseEvent *e){
    time_2sec->stop();
    delete time_2sec;
    QDialog::closeEvent(e);
}

volante::~volante()
{
    delete ui;
}
