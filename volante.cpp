#include "volante.h"
#include "ui_volante.h"

volante::volante(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::volante)
{
    ui->setupUi(this);

    ui->combo_modo->addItem("Volante de Inercia (0)",QVariant(0));
    ui->combo_modo->addItem("Variador de Velocidad (1)",QVariant(1));
    actual_mode=ui->combo_modo->currentIndex();

    connect(ui->combo_modo,SIGNAL(activated(int)),this,SLOT(mode_changed()));
}

volante::~volante()
{
    delete ui;
}
