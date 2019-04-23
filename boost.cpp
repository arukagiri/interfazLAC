#include "boost.h"
#include "ui_boost.h"

boost::boost(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::boost)
{
    ui->setupUi(this);

    this->setWindowTitle("Boost");
}

boost::~boost()
{
    delete ui;
}
