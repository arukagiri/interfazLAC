#include "estadored.h"
#include "ui_estadored.h"
#include "gen_eolico.h"
#include <QLabel>
#include "PC.h"
#include <QTimer>
#include "volante.h"
#include "boost.h"
#include "lacan_detect.h"
#include <QString>

EstadoRed::EstadoRed(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EstadoRed)
{
    //Como estado de red a su vez envia mensajes cuya respuesta debe mostrarse en si misma es necesaria
    //una flag que indique que dicha ventana se encuentra abierta y que ademas las respuestas entrantes corresponderan
    //a los mensajes que ella envio
    mw = qobject_cast<MainWindow*>(this->parent());
    mw->change_ERflag(true);
    connect(mw, SIGNAL(postforER_arrived(LACAN_MSG)), this, SLOT(ERpost_Handler(LACAN_MSG)));

    ui->setupUi(this);

    this->setWindowTitle("Estado de Red");

    time_2sec = new QTimer();

    ui->label_gen_vo->setText("----");
    ui->label_gen_io->setText("----");
    ui->label_gen_velocidad->setText("----");
    ui->label_gen_torque->setText("----");
    ui->label_gen_modo->setText("Modo: ----");

    ui->label_vol_vo->setText("----");
    ui->label_vol_io->setText("----");
    ui->label_vol_velocidad->setText("----");
    ui->label_vol_torque->setText("----");
    ui->label_vol_modo->setText("Modo: ----");

    ui->label_boost_vi->setText("----");
    ui->label_boost_ii->setText("----");
    ui->label_boost_vo->setText("----");
    ui->label_boost_io->setText("----");
    ui->label_boost_modo->setText("Modo: ----");

    send_qry();
    set_states();
    connect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));
    send_queries = true;
    time_2sec->start(2000);

}

void EstadoRed::refresh_values(){

    if(mw->device_is_connected(LACAN_ID_GEN)){
        ui->label_gen_vo->setText(QString::number(double(gen_vo),'f',2));
        ui->label_gen_io->setText(QString::number(double(gen_io),'f',2));
        ui->label_gen_velocidad->setText(QString::number(double(gen_vel),'f',2));
        ui->label_gen_torque->setText(QString::number(double(gen_tor),'f',2));
        ui->label_gen_modo->setText(detect_mode(gen_mod));
    }
    else{
        ui->label_gen_vo->setText("----");
        ui->label_gen_io->setText("----");
        ui->label_gen_velocidad->setText("----");
        ui->label_gen_torque->setText("----");
        ui->label_gen_modo->setText("Modo: ----");
    }

    if(mw->device_is_connected(LACAN_ID_VOLANTE)){
        ui->label_vol_vo->setText(QString::number(double(vol_vo),'f',2));
        ui->label_vol_io->setText(QString::number(double(vol_io),'f',2));
        ui->label_vol_velocidad->setText(QString::number(double(vol_vel),'f',2));
        ui->label_vol_torque->setText(QString::number(double(vol_tor),'f',2));
        ui->label_vol_modo->setText(detect_mode(vol_mod));
    }
    else{
        ui->label_vol_vo->setText("----");
        ui->label_vol_io->setText("----");
        ui->label_vol_velocidad->setText("----");
        ui->label_vol_torque->setText("----");
        ui->label_vol_modo->setText("Modo: ----");
    }

}

void EstadoRed::send_qry(){

    uint16_t dest=0;
    if(mw->device_is_connected(LACAN_ID_GEN)){
        dest=LACAN_ID_GEN;
        mw->LACAN_Query(LACAN_VAR_VO_INST, false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
        mw->LACAN_Query(LACAN_VAR_IO_INST, false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
        mw->LACAN_Query(LACAN_VAR_W_INST, false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
        mw->LACAN_Query(LACAN_VAR_TORQ_INST, false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
        mw->LACAN_Query(LACAN_VAR_MOD, false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    }

    if(mw->device_is_connected(LACAN_ID_VOLANTE)){
        dest=LACAN_ID_VOLANTE;
        mw->LACAN_Query(LACAN_VAR_VO_INST, false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()),  SLOT(verificarACK()));
        mw->LACAN_Query(LACAN_VAR_IO_INST, false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()),  SLOT(verificarACK()));
        mw->LACAN_Query(LACAN_VAR_W_INST, false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()),  SLOT(verificarACK()));
        mw->LACAN_Query(LACAN_VAR_TORQ_INST, false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()),  SLOT(verificarACK()));
        mw->LACAN_Query(LACAN_VAR_MOD,false,dest);
//        connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()),  SLOT(verificarACK()));
    }
}

void EstadoRed::timer_handler(){
    if(send_queries){
        send_qry();
    }
    refresh_values();
    set_states();
}

void EstadoRed::set_states(){
    if(mw->device_is_connected(LACAN_ID_GEN)){ui->button_gen->setEnabled(true);}
    else{ui->button_gen->setDisabled(true);}

    if(mw->device_is_connected(LACAN_ID_VOLANTE)){ui->button_vol->setEnabled(true);}
    else{ui->button_vol->setDisabled(true);}

    if(mw->device_is_connected(LACAN_ID_BOOST)){ui->button_boost->setEnabled(true);}
    else{ui->button_boost->setDisabled(true);}

}

void EstadoRed::ERpost_Handler(LACAN_MSG msg){

    uint16_t source=msg.ID&LACAN_IDENT_MASK;
    uint8_t variable=msg.BYTE1;

    recibed_val.var_char[0]=msg.BYTE2;
    recibed_val.var_char[1]=msg.BYTE3;
    recibed_val.var_char[2]=msg.BYTE4;
    recibed_val.var_char[3]=msg.BYTE5;

    switch (source) {
        case LACAN_ID_GEN:
            //se la paso a la pantalla del gen, si esta abierta
            emit postforGEN_arrived(msg);

            switch (variable) {
                case LACAN_VAR_IO_INST:
                    gen_io = recibed_val.var_float;
                    break;
                case LACAN_VAR_VO_INST:
                    gen_vo = recibed_val.var_float;
                    break;
                case LACAN_VAR_TORQ_INST:
                    gen_tor = recibed_val.var_float;
                    break;
                case LACAN_VAR_W_INST:
                    gen_vel = recibed_val.var_float;
                    break;
                case LACAN_VAR_MOD:
                    gen_mod = recibed_val.var_char[0];
                    break;
                default:
                    break;
            }
            break;

       case LACAN_ID_VOLANTE:
            //se la paso a la pantalla del gen, si esta abierta
            emit postforVOL_arrived(msg);

            switch (variable) {
                case LACAN_VAR_IO_INST:
                    vol_io = recibed_val.var_float;
                    break;
                case LACAN_VAR_VO_INST:
                    vol_vo = recibed_val.var_float;
                    break;
                case LACAN_VAR_TORQ_INST:
                    vol_tor = recibed_val.var_float;
                    break;
                case LACAN_VAR_W_INST:
                    vol_vel = recibed_val.var_float;
                    break;
                case LACAN_VAR_MOD:
                    vol_mod = recibed_val.var_char[0];
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

}

void EstadoRed::on_button_vol_clicked()
{
    volante *vol_win = new volante(mw);
    vol_win->setModal(true);
    vol_win->show();

    send_queries = false;

    connect(vol_win, SIGNAL(volWindowsClosed()), this, SLOT(handle_dispWindowsClosed()));
    connect(this, SIGNAL(postforVOL_arrived(LACAN_MSG)), vol_win, SLOT(VOLpost_Handler(LACAN_MSG)));
}

void EstadoRed::on_button_gen_clicked()
{
    Gen_Eolico *gen_win = new Gen_Eolico(mw);
    gen_win->setModal(true);
    gen_win->show();

    send_queries = false;

    connect(gen_win, SIGNAL(genWindowsClosed()), this, SLOT(handle_dispWindowsClosed()));
    connect(this, SIGNAL(postforGEN_arrived(LACAN_MSG)), gen_win, SLOT(GENpost_Handler(LACAN_MSG)));
}

void EstadoRed::on_button_boost_clicked()
{
    boost *boost_win = new boost(mw);
    boost_win->setModal(true);
    boost_win->show();

    send_queries = false;

   // connect(this, SIGNAL(postforBOOST_arrived(LACAN_MSG)), boost_win, SLOT(GENpost_Handler(LACAN_MSG)));
}


void EstadoRed::on_pushButton_gen_enable_clicked(){
    dest = LACAN_ID_GEN;
    cmd = LACAN_CMD_ENABLE;
    mw->LACAN_Do(cmd,false,dest);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

void EstadoRed::on_pushButton_gen_disable_clicked(){
    dest = LACAN_ID_GEN;
    cmd = LACAN_CMD_DISABLE;
    mw->LACAN_Do(cmd,false,dest);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}

void EstadoRed::on_pushButton_vol_enable_clicked(){
    dest = LACAN_ID_VOLANTE;
    cmd = LACAN_CMD_ENABLE;
    mw->LACAN_Do(cmd,false,dest);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();

}

void EstadoRed::on_pushButton_vol_disable_clicked(){
    dest = LACAN_ID_VOLANTE;
    cmd = LACAN_CMD_DISABLE;
    mw->LACAN_Do(cmd,false,dest);
    connect(&(mw->msg_ack.back()->ack_timer),SIGNAL(timeout()), mw, SLOT(verificarACK()));
    mw->agregar_log_sent();
}


void EstadoRed::closeEvent(QCloseEvent *e){
    mw->change_ERflag(false);
    time_2sec->stop();
    delete time_2sec;
    QDialog::closeEvent(e);
}

EstadoRed::~EstadoRed()
{
    delete ui;
}


void EstadoRed::handle_dispWindowsClosed(){
   send_queries = true;
}

