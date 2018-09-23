#include "addnewdevdialog.h"
#include "ui_addnewdevdialog.h"

AddNewDevDialog::AddNewDevDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddNewDevDialog)
{
    devname="";
    ui->setupUi(this);
    ui->devnameEdit->setMaxLength(30);
}

AddNewDevDialog::~AddNewDevDialog()
{
    delete ui;
}



void AddNewDevDialog::on_ok_button_clicked()
{
    devname=ui->devnameEdit->text();
    emit dev_name_set(devname);
    this->close();
}

void AddNewDevDialog::on_cancel_button_clicked()
{
    emit add_name_canceled();
    this->close();
}

void AddNewDevDialog::closeEvent(QCloseEvent *e){
    emit add_name_canceled();
    QDialog::closeEvent(e);
}
