#include "estadored.h"
#include "ui_estadored.h"
#include "gen_eolico.h"
#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include "clickablelabel.h"

EstadoRed::EstadoRed(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EstadoRed)
{
    ui->setupUi(this);

    ClickableLabel *pc = new ClickableLabel(ui->label_pc);

    connect(pc,SIGNAL(clicked()),this,SLOT(on_PC_clicked()));

}

EstadoRed::~EstadoRed()
{
    delete ui;
}


void EstadoRed::on_PC_clicked(){
    qDebug()<<"SAPE";

}

/*
void EstadoRed::on_GEN_clicked(){
    qDebug()<<"SAPE";

}

void EstadoRed::on_BOOST_clicked(){
    qDebug()<<"SAPE";

}

void EstadoRed::on_VOL_clicked(){
    qDebug()<<"SAPE";

}*/
