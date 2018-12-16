#include "boost.h"
#include "ui_boost.h"

boost::boost(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::boost)
{
    ui->setupUi(this);
}

boost::~boost()
{
    delete ui;
}
