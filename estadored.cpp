#include "estadored.h"
#include "ui_estadored.h"
#include "gen_eolico.h"
#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include "LACAN_SEND.h"
#include "PC.h"
#include <QTimer>

EstadoRed::EstadoRed(QSerialPort &serial_port0,vector <TIMED_MSG*> &msg_ack0,uint8_t &code0,vector <LACAN_MSG> &msg_log0, bool do_log0,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EstadoRed)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow*>(this->parent());

    serial_port=&serial_port0;
    code=&code0;
    msg_ack=&msg_ack0;
    msg_log=&msg_log0;
    do_log=do_log0;
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


//LA IDEA ES QUE CADA VEZ QUE LLEGA UN POST CON UNA DE LAS VARIABLES DECLARADAS EN EL .H SE VAYA ACTUALIZANDO EL VALOR (guardandolo en las variables genV y genI apenas llega)
//SE SUPONE QUE ESA VARIABLE VA A LLEGAR ANTES DEL SEGUNDO LO DE LOS 2 SEGUNDOS QUE TARDAN EN LLAMARSE LAS FUNCIONES
//ENTONCES CUANDO SE LLAMAN A LAS FUNCUINES, LAS VARIABLES GEN_V Y GEN_I CONTIENEN EL VALOR ACTUAL DEL PARAMETRO
//refresh_value ES ACTUALIZA LOS VALORES QUE SE MUESTRAN EN LOS LABEL CON LOS VALORES ACTUALES CONTENIDOS EN DICHAS VARIABLES
//LUEGO send_qry MANDA SOLICITUD PARA QUE SE VAYAN ACTUALIZANDO LOS VALORES EN LOS 2 SEGUNDOS QUE SE TARDA EN LLAMAR DE NUEVO A resfresh
//APENAS SE ABRE LA VENTANA SE MANDAN UNOS qrys Y VAS A TENER QUE ESPERAR EL TIEMPO DEL TIMER HASTA QUE SE MUESTREN


}

EstadoRed::~EstadoRed()
{
    mw->change_ERflag();
    delete ui;
}


void EstadoRed::on_button_gen_clicked()
{
    Gen_Eolico *gen_win = new Gen_Eolico();
    gen_win->setModal(true);
    gen_win->show();

}


void EstadoRed::refresh_values(){

  /*  int value = 7;
    ui->label_gen_v->setText( QString::number( value ) );

    const QString text = ui->label_gen_v->text();
    bool ok = true;
    int valuew = text.toInt( &ok );
    if ( ok )
    {
        valuew++;
    }
    ui->label_gen_i->setText( QString::number( valuew ) );
*/


ui->label_gen_v->setText(QString::number(gen_v));
ui->label_gen_i->setText(QString::number(gen_i));

ui->label_boost_v->setText(QString::number(boost_v));
ui->label_boost_i->setText(QString::number(boost_i));

ui->label_vol_v->setText(QString::number(vol_v));
ui->label_vol_i->setText(QString::number(vol_i));


}

void EstadoRed::send_qry(){
    /*
    LACAN_Query(*serial_port,LACAN_ID_GEN,LACAN_VAR_VO, *code, *msg_ack, *msg_log);
    mw->agregar_log_sent(*msg_log);
    LACAN_Query(*serial_port,LACAN_ID_GEN,LACAN_VAR_IO, *code, *msg_ack, *msg_log);
    mw->agregar_log_sent(*msg_log);

    LACAN_Query(*serial_port,LACAN_ID_VOLANTE,LACAN_VAR_VO, *code, *msg_ack, *msg_log);
    mw->agregar_log_sent(*msg_log);
    LACAN_Query(*serial_port,LACAN_ID_VOLANTE,LACAN_VAR_IO, *code, *msg_ack, *msg_log);
    mw->agregar_log_sent(*msg_log);

    LACAN_Query(*serial_port,LACAN_ID_BOOST,LACAN_VAR_VO, *code, *msg_ack, *msg_log);
    mw->agregar_log_sent(*msg_log);
    LACAN_Query(*serial_port,LACAN_ID_BOOST,LACAN_VAR_IO, *code, *msg_ack, *msg_log);
    mw->agregar_log_sent(*msg_log);*/
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
        case LACAN_VAR_II://PUSE LAS DE ENTRADA PORQ NO TENGO NI PUTA IDEA CUALES VAN
            boost_i=msg.BYTE2;
            break;
        case LACAN_VAR_VI:
            boost_v=msg.BYTE2;
            break;
        default:
            //VER QUE PASA SI LLEGA UN POST CON OTRAS VARIABLES, QUIZA PODRIAMOS PASARLO A LA
            //VENTANA DEL DISPOSITIVO (SI ESTA ABIERTA) PARA MOSTRARLOS EN EL DB
            break;
        }
        break;
    case LACAN_ID_GEN:
        switch (msg.BYTE1) {
        case LACAN_VAR_II:
            gen_i=msg.BYTE2;
            break;
        case LACAN_VAR_VI:
            gen_v=msg.BYTE2;
            break;
        default:
            break;
        }
        break;
    case LACAN_ID_VOLANTE:
        switch (msg.BYTE1) {
        case LACAN_VAR_II:
            vol_i=msg.BYTE2;
            break;
        case LACAN_VAR_VI:
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
