#include "estadored.h"
#include "ui_estadored.h"
#include "gen_eolico.h"
#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include "LACAN_SEND.h"
#include "PC.h"
#include <QTimer>

EstadoRed::EstadoRed(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EstadoRed)
{
    ui->setupUi(this);

    mw = qobject_cast<MainWindow*>(this->parent());

    time_2sec = new QTimer();

    gen_vo=-9999999;
    gen_io=-9999999;
    boost_vo=-9999999;
    boost_io=-9999999;
    vol_vo=-9999999;
    vol_io=-9999999;


    ui->label_gen_vo->setText("----");
    ui->label_gen_io->setText("----");
    ui->label_gen_velocidad->setText("----");
    ui->label_gen_torque->setText("----");

    ui->label_vol_vo->setText("----");
    ui->label_vol_io->setText("----");
    ui->label_boost_vi->setText("----");
    ui->label_boost_ii->setText("----");

    ui->label_boost_vo->setText("----");
    ui->label_boost_io->setText("----");
    ui->label_vol_velocidad->setText("----");
    ui->label_vol_torque->setText("----");


    send_qry();
    connect(time_2sec, SIGNAL(timeout()), this, SLOT(timer_handler()));

    time_2sec->start(2000);

}

EstadoRed::~EstadoRed()
{
    delete ui;
}


void EstadoRed::on_button_gen_clicked()
{
    Gen_Eolico *gen_win = new Gen_Eolico(mw);
    gen_win->setModal(true);
    gen_win->show();
    connect(this, SIGNAL(postforGEN_arrived(LACAN_MSG)), gen_win, SLOT(GENpost_Handler(LACAN_MSG)));
}


void EstadoRed::refresh_values(){

    ui->label_gen_vo->setText(QString::number(gen_vo));
    ui->label_gen_io->setText(QString::number(gen_io));
    ui->label_gen_velocidad->setText(QString::number(gen_vel));
    ui->label_gen_torque->setText(QString::number(gen_tor));

    ui->label_boost_vo->setText(QString::number(boost_vo));
    ui->label_boost_io->setText(QString::number(boost_io));
    ui->label_boost_vi->setText(QString::number(boost_vi));
    ui->label_boost_ii->setText(QString::number(boost_io));

    ui->label_vol_vo->setText(QString::number(vol_vo));
    ui->label_vol_io->setText(QString::number(vol_io));
    ui->label_vol_velocidad->setText(QString::number(vol_vel));
    ui->label_vol_torque->setText(QString::number(vol_tor));

}

void EstadoRed::send_qry(){
/*
    mw->dest=LACAN_ID_GEN;
    LACAN_Query(mw,LACAN_VAR_VO);
    LACAN_Query(mw,LACAN_VAR_IO);
    mw->dest=LACAN_ID_VOLANTE;
    LACAN_Query(mw,LACAN_VAR_VO);
    LACAN_Query(mw,LACAN_VAR_IO);
    mw->dest=LACAN_ID_BOOST;
    LACAN_Query(mw,LACAN_VAR_VO);
    LACAN_Query(mw,LACAN_VAR_IO);
*/
}


void EstadoRed::timer_handler(){
    refresh_values();
    send_qry();
}


void EstadoRed::var_changed(uint16_t var, uint16_t data){
    qDebug()<<var;
    qDebug()<<data;
}


void EstadoRed::ERpost_Handler(LACAN_MSG msg){
    uint16_t source=msg.ID&LACAN_IDENT_MASK;
    switch (source) {
    case LACAN_ID_BOOST:
        switch (msg.BYTE1) {
        case LACAN_VAR_IO:
            boost_io=msg.BYTE2;
            break;
        case LACAN_VAR_VO:
            boost_vo=msg.BYTE2;
            break;
        default:
            break;
        }
        break;


    case LACAN_ID_GEN:

        //se la paso a la pantalla de gen, si esta abierta
        emit postforGEN_arrived(msg);

        switch (msg.BYTE1) {
        case LACAN_VAR_IO:
            gen_io=msg.BYTE2;
            break;
        case LACAN_VAR_VO:
            gen_vo=msg.BYTE2;
            break;
        default:
            break;
        }
        break;


    case LACAN_ID_VOLANTE:
        switch (msg.BYTE1) {
        case LACAN_VAR_IO:
            vol_io=msg.BYTE2;
            break;
        case LACAN_VAR_VO:
            vol_vo=msg.BYTE2;
            break;
        default:
            break;
        }
        break;

    default:
        break;
    }

}

void EstadoRed::closeEvent(QCloseEvent *e){
    mw->change_ERflag();
    time_2sec->stop();
    delete time_2sec;
    QDialog::closeEvent(e);
}
