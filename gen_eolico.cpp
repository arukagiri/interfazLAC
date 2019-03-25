#include "gen_eolico.h"
#include "ui_gen_eolico.h"
#include <QMessageBox>
#include <QtCore>
#include <QtGui>
#include <QTimer>
#include "PC.h"

Gen_Eolico::Gen_Eolico(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Gen_Eolico)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow*>(this->parent());


//Configuracion del CombBox para los Modos
    ui->combo_modo->addItem("Velocidad (0)",QVariant(LACAN_VAR_MOD_VEL));
    ui->combo_modo->addItem("Potencia (1)",QVariant(LACAN_VAR_MOD_POT));
    ui->combo_modo->addItem("Torque (2)",QVariant(LACAN_VAR_MOD_TORQ));
    ui->combo_modo->addItem("MPPT (3)",QVariant(LACAN_VAR_MOD_MPPT));
    connect(ui->combo_modo,SIGNAL(activated(int)),this,SLOT(verificar_mode_changed()));
    on_combo_modo_currentIndexChanged(0);
    mode_changed();


//Inicializacion de Labels
    ui->label_gen_io->setText("----");
    ui->label_gen_vo->setText("----");
    ui->label_gen_ibat->setText("----");
    ui->label_gen_po->setText("----");
    ui->label_gen_vel->setText("----");
    ui->label_gen_tor->setText("----");

    ui->label_gen_isd_ref->setText("----");
    ui->label_gen_lim_ibat_ref->setText("----");
    ui->label_gen_lim_ief_ref->setText("----");
    ui->label_gen_lim_vdc_ref->setText("----");
    ui->label_gen_pot_ref->setText("----");
    ui->label_gen_speed_ref->setText("----");
    ui->label_gen_torque_ref->setText("----");


//TIMER ENCARGADO DE REFRESCAR LOS VALORES Y DE ENVIAR LAS NUEVAS CONSULTAS
    time_2sec = new QTimer();
    connect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));
    time_2sec->start(2000); //velocidad de refresco (en ms)

    send_qry(); //envio las primeras consultas
}

void Gen_Eolico::timer_handler(){
    if(mw->device_is_connected(LACAN_ID_GEN)){
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


//Me fijo que variable es la que llego, y le asigno el valor correspondiente, a la variable propia de la clase
void Gen_Eolico::GENpost_Handler(LACAN_MSG msg){
      recibed_val.var_char[0]=msg.BYTE2;
      recibed_val.var_char[1]=msg.BYTE3;
      recibed_val.var_char[2]=msg.BYTE4;
      recibed_val.var_char[3]=msg.BYTE5;
      switch (msg.BYTE1) {
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
            //prev_speed_ref=speed_ref;
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
        case LACAN_VAR_MOD:
            actual_mode=recibed_val.var_char[0];
            ui->combo_modo->setCurrentIndex(ui->combo_modo->findData(actual_mode));
            //new_mode();    version1
            mode_changed();  //ver si va este o el anterior (cambio el 17/3)
            break;
        default:
            break;
    }
}

//Consulto todas las variables del GENERADOR
void Gen_Eolico::send_qry(){

    mw->LACAN_Query(LACAN_VAR_VO_INST,false,dest);  //gen_vo
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_IO_INST,false,dest);  //gen_io
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_I_BAT_INST,false,dest);   //gen_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_W_INST,false,dest);   //gen_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_TORQ_INST,false,dest);   //gen_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_PO_INST,false,dest);   //gen_po
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

    mw->LACAN_Query(LACAN_VAR_W_SETP,false,dest);   //sped_ref
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_PO_SETP,false,dest);   //po_ref
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_TORQ_SETP,false,dest);   //torq_ref
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_IEF_SETP,false,dest);   //ief
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_ISD_SETP,false,dest);   //isd
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_I_BAT_SETP,false,dest);   //lim_ibat
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->LACAN_Query(LACAN_VAR_VO_SETP,false,dest);   //lim_vdc
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));

    mw->LACAN_Query(LACAN_VAR_MOD,false,dest);   //modo
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
}

//Se actualizan todos los valores del GENERADOR
void Gen_Eolico::refresh_values(){

    //Variables de SET
    ui->label_gen_torque_ref->setText(QString::number(torque_ref,'f',2));
    ui->label_gen_speed_ref->setText(QString::number(speed_ref,'f',2));
    ui->label_gen_pot_ref->setText(QString::number(pot_ref,'f',2));
    ui->label_gen_lim_vdc_ref->setText(QString::number(lim_vdc,'f',2));
    ui->label_gen_lim_ief_ref->setText(QString::number(lim_ief,'f',2));
    ui->label_gen_isd_ref->setText(QString::number(isd_ref,'f',2));
    ui->label_gen_lim_ibat_ref->setText(QString::number(lim_ibat,'f',2));

    //Variables de Salida
    ui->label_gen_vo->setText(QString::number(gen_vo,'f',2));
    ui->label_gen_io->setText(QString::number(gen_io,'f',2));
    ui->label_gen_ibat->setText(QString::number(gen_ibat,'f',2));
    ui->label_gen_po->setText(QString::number(gen_po,'f',2));
    ui->label_gen_tor->setText(QString::number(gen_tor,'f',2));
    ui->label_gen_vel->setText(QString::number(gen_vel,'f',2));
}


//VER SI ANDA EL TEMA DEL CODE (mw->code)
//entre  "mw->LACAN_Do(cmd,false,dest);" donde se crea el vecotor de ack que espera la respuesta  y
// "connect(&(mw->msg_ack.back()->ack_timer)..." que uso el .back (osea el ultimo que asumo que se creo en el envio anteior)
//quiza podria llegar otro ack y hacer pura caca
void Gen_Eolico::on_pushButton_start_clicked()
{
    cmd=LACAN_CMD_START;
    mw->LACAN_Do(cmd,false,dest);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

void Gen_Eolico::on_pushButton_stop_clicked()
{
    cmd=LACAN_CMD_STOP;
    mw->LACAN_Do(cmd,false,dest);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}


void Gen_Eolico::on_pushButton_comandar_clicked()
{
   Comandar *comwin = new Comandar(mw,dest);
   comwin->setModal(true);
   comwin->show();
}

void Gen_Eolico::verificar_mode_changed(){
    QMessageBox::StandardButton reply;
    QString str="¿Esta seguro que desea cambiar al modo ";
    str.append(ui->combo_modo->currentText());
    str.append(" ?");
    reply = QMessageBox::question(this,"Confirm",str, QMessageBox::Yes | QMessageBox::No );
    if(reply==QMessageBox::Yes){  
        //version2
        data_can modo;
        modo.var_char[0] = actual_mode;
        modo.var_char[1] = 0;
        modo.var_char[2] = 0;
        modo.var_char[3] = 0;
        mw->LACAN_Set(LACAN_VAR_MOD,modo,false,dest);
        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
        mw->agregar_log_sent();

        //version1
        //mode_changed();
    }
    else{
        //version2
        actual_mode=previous_mode;

        //version1
        //actual_mode=previous_mode;
        //ui->combo_modo->setCurrentIndex(ui->combo_modo->findData(previous_mode));
    }
}

void Gen_Eolico::mode_changed(){
    new_mode();             //poner los valores reales en el spin
    refresh_values();       //todo desclickeado
}

//habilita y deshabilita los campos, dependiendo el modo actual
void Gen_Eolico::new_mode(){

    qDebug()<<"Numero de Modo: "<<ui->combo_modo->currentIndex();
    switch (actual_mode) {
    case LACAN_VAR_MOD_VEL:     //Velocidad
        qDebug()<<"Entro a velocidad";
        ui->label_pot_ref->setDisabled(true);
        ui->label_speed_ref->setEnabled(true);
        ui->label_torque_ref->setDisabled(true);
        break;
    case LACAN_VAR_MOD_POT:     //Potencia
        qDebug()<<"Entro a potencia";
        ui->label_pot_ref->setEnabled(true);
        ui->label_speed_ref->setDisabled(true);
        ui->label_torque_ref->setDisabled(true);
        break;
    case LACAN_VAR_MOD_TORQ:     //Torque
        qDebug()<<"Entro a Torke";
        ui->label_pot_ref->setDisabled(true);
        ui->label_speed_ref->setDisabled(true);
        ui->label_torque_ref->setEnabled(true);
        break;
    case LACAN_VAR_MOD_MPPT:     //MPPT

        qDebug()<<"Entro a Mppt";
        ui->label_pot_ref->setDisabled(true);
        ui->label_speed_ref->setDisabled(true);
        ui->label_torque_ref->setDisabled(true);
        break;
    default:
        break;
    }
}

void Gen_Eolico::on_combo_modo_currentIndexChanged(int index)
{
   previous_mode = actual_mode;     //guardo el modo anterior por si el usuario cancela el cambio
   actual_mode = ui->combo_modo->itemData(index).toInt();
}

void Gen_Eolico::closeEvent(QCloseEvent *e){
    //time_2sec->stop();
    //delete time_2sec;
    //    QDialog::closeEvent(e);
}

Gen_Eolico::~Gen_Eolico()
{
    delete ui;
}

