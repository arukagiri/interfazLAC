#include "estadored.h"
#include "ui_estadored.h"
#include "gen_eolico.h"
#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include "clickablelabel.h"
#include "LACAN_SEND.h"
#include "PC.h"

EstadoRed::EstadoRed(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EstadoRed)
{
    ui->setupUi(this);

    //ClickableLabel *pc = new ClickableLabel(ui->label_pc);

    //connect(pc,SIGNAL(clicked()),this,SLOT(on_PC_clicked()));


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

}

//UNA FORMA DE RESOLVERLO SERIA CADA 2 SEG LLMAR A UPDATE Y DSP A QUERY DENTRO DE UN TIMER
//ENTONCES LE DAS 2 SEG A QUERY PARA QUE CONTESTE

//ESTA FUNCION MANDA QUERY A LOS DISPOSITIVOS PARA CONSULTAR EL VALOR ACTUAL DE I0 Y V0
void EstadoRed::refresh_values(){
    //LACAN_Query(LACAN_ID_GEN LACAN_VAR_VO);
    //LACAN_Query(LACAN_ID_VOLANTE LACAN_VAR_IO);

}

/*
//ESTA FUNCION UPDATEA LOS VALORES UNA VEZ QUE SE RECIBIERON TODAS LAS RESPUESTAS
void EstadoRed::update_values(){

    int value = 7;
    ui->label_gen_v->setText( QString::number( value ) );

    const QString text = ui->label_gen_v->text();
    bool ok = true;
    int valuew = text.toInt( &ok );
    if ( ok )
    {
        valuew++;
    }
    ui->label_gen_i->setText( QString::number( valuew ) );

gen_v=;
gen_i=;
bust_v=;
bust_i=;
vol_v=;
vol_i=;
}*/
