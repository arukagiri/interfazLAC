#include "gen_eolico.h"
#include "ui_gen_eolico.h"

Gen_Eolico::Gen_Eolico(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Gen_Eolico)
{
    ui->setupUi(this);
}

Gen_Eolico::~Gen_Eolico()
{
    delete ui;
}
