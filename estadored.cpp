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

    gen_v=-9999999;
    gen_i=-9999999;
    boost_v=-9999999;
    boost_i=-9999999;
    vol_v=-9999999;
    vol_i=-9999999;


    ui->label_gen_v->setText("----");
    ui->label_gen_i->setText("----");

    ui->label_vol_v->setText("----");
    ui->label_vol_i->setText("----");

    ui->label_boost_v->setText("----");
    ui->label_boost_i->setText("----");

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
    Gen_Eolico *gen_win = new Gen_Eolico();
    gen_win->setModal(true);
    gen_win->show();
    connect(this, SIGNAL(postforGEN_arrived(LACAN_MSG)), gen_win, SLOT(GENpost_Handler(LACAN_MSG)));
}


void EstadoRed::refresh_values(){

    ui->label_gen_v->setText(QString::number(gen_v));
    ui->label_gen_i->setText(QString::number(gen_i));

    ui->label_boost_v->setText(QString::number(boost_v));
    ui->label_boost_i->setText(QString::number(boost_i));

    ui->label_vol_v->setText(QString::number(vol_v));
    ui->label_vol_i->setText(QString::number(vol_i));

}

void EstadoRed::send_qry(){

    mw->dest=LACAN_ID_GEN;
    LACAN_Query(mw,LACAN_VAR_VO);
    LACAN_Query(mw,LACAN_VAR_IO);
    mw->dest=LACAN_ID_VOLANTE;
    LACAN_Query(mw,LACAN_VAR_VO);
    LACAN_Query(mw,LACAN_VAR_IO);
    mw->dest=LACAN_ID_BOOST;
    LACAN_Query(mw,LACAN_VAR_VO);
    LACAN_Query(mw,LACAN_VAR_IO);

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
            boost_i=msg.BYTE2;
            break;
        case LACAN_VAR_VO:
            boost_v=msg.BYTE2;
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
            gen_i=msg.BYTE2;
            break;
        case LACAN_VAR_VO:
            gen_v=msg.BYTE2;
            break;
        default:
            break;
        }
        break;


    case LACAN_ID_VOLANTE:
        switch (msg.BYTE1) {
        case LACAN_VAR_IO:
            vol_i=msg.BYTE2;
            break;
        case LACAN_VAR_VO:
            vol_v=msg.BYTE2;
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
