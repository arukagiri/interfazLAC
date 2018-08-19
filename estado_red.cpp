#include "estado_red.h"
#include "ui_estado_red.h"

Estado_Red::Estado_Red(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Estado_Red)
{
    ui->setupUi(this);
}

Estado_Red::~Estado_Red()
{
    delete ui;
}

void Estado_Red::on_pushButton_clicked()
{

}
