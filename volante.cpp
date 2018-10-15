#include "volante.h"
#include "ui_volante.h"

volante::volante(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::volante)
{
    ui->setupUi(this);
}

volante::~volante()
{
    delete ui;
}
