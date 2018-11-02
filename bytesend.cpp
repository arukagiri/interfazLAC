#include "bytesend.h"
#include "ui_bytesend.h"
#include <stdint.h>
ByteSend::ByteSend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ByteSend)
{
    ui->setupUi(this);

    ui->dlc->addItem("1",QVariant(1));
    ui->dlc->addItem("2",QVariant(2));
    ui->dlc->addItem("3",QVariant(3));
    ui->dlc->addItem("4",QVariant(4));
    ui->dlc->addItem("5",QVariant(5));
    ui->dlc->addItem("6",QVariant(6));
    ui->dlc->addItem("7",QVariant(7));
    ui->dlc->addItem("8",QVariant(8));
    ui->dlc->setCurrentIndex(0);

    ui->data->setInputMask("HH;");
    ui->data->setText("");

    ui->data0->setInputMask("HH;");
    ui->data1->setInputMask("HH;");
    ui->data2->setInputMask("HH;");
    ui->data3->setInputMask("HH;");
    ui->data4->setInputMask("HH;");
    ui->data5->setInputMask("HH;");
    ui->data6->setInputMask("HH;");
    ui->data7->setInputMask("HH;");

    ui->id1->setInputMask("HH;");
    ui->id1->setText("");
    ui->id2->setInputMask("H9;");   //el segundo byte de esta deberia ser menor a 7
    ui->id2->setText("");

}


void ByteSend::on_dlc_currentIndexChanged(int index)
{
    uint8_t acutal_dlc = ui->dlc->itemData(index).toInt();

    switch (acutal_dlc) {
    case 1:
        ui->data1->setDisabled(true);
        ui->data2->setDisabled(true);
        ui->data3->setDisabled(true);
        ui->data4->setDisabled(true);
        ui->data5->setDisabled(true);
        ui->data6->setDisabled(true);
        ui->data7->setDisabled(true);
        break;
    case 2:
        ui->data1->setEnabled(true);
        ui->data2->setDisabled(true);
        ui->data3->setDisabled(true);
        ui->data4->setDisabled(true);
        ui->data5->setDisabled(true);
        ui->data6->setDisabled(true);
        ui->data7->setDisabled(true);
        break;
    case 3:
        ui->data1->setEnabled(true);
        ui->data2->setEnabled(true);
        ui->data3->setDisabled(true);
        ui->data4->setDisabled(true);
        ui->data5->setDisabled(true);
        ui->data6->setDisabled(true);
        ui->data7->setDisabled(true);
        break;
    case 4:
        ui->data1->setEnabled(true);
        ui->data2->setEnabled(true);
        ui->data3->setEnabled(true);
        ui->data4->setDisabled(true);
        ui->data5->setDisabled(true);
        ui->data6->setDisabled(true);
        ui->data7->setDisabled(true);
        break;
    case 5:
        ui->data1->setEnabled(true);
        ui->data2->setEnabled(true);
        ui->data3->setEnabled(true);
        ui->data4->setEnabled(true);
        ui->data5->setDisabled(true);
        ui->data6->setDisabled(true);
        ui->data7->setDisabled(true);
        break;
    case 6:
        ui->data1->setEnabled(true);
        ui->data2->setEnabled(true);
        ui->data3->setEnabled(true);
        ui->data4->setEnabled(true);
        ui->data5->setEnabled(true);
        ui->data6->setDisabled(true);
        ui->data7->setDisabled(true);
        break;
    case 7:
        ui->data1->setEnabled(true);
        ui->data2->setEnabled(true);
        ui->data3->setEnabled(true);
        ui->data4->setEnabled(true);
        ui->data5->setEnabled(true);
        ui->data6->setEnabled(true);
        ui->data7->setDisabled(true);
        break;
    case 8:
        ui->data1->setEnabled(true);
        ui->data2->setEnabled(true);
        ui->data3->setEnabled(true);
        ui->data4->setEnabled(true);
        ui->data5->setEnabled(true);
        ui->data6->setEnabled(true);
        ui->data7->setEnabled(true);
        break;

    }

    switch (acutal_dlc) {
    case 1:
        ui->data->setInputMask("HH;");
        break;
    case 2:
        ui->data->setInputMask("HH HH;");
        break;
    case 3:
        ui->data->setInputMask("HH HH HH;");
        break;
    case 4:
        ui->data->setInputMask("HH HH HH HH;");
        break;
    case 5:
        ui->data->setInputMask("HH HH HH HH HH;");
        break;
    case 6:
        ui->data->setInputMask("HH HH HH HH HH HH;");
        break;
    case 7:
        ui->data->setInputMask("HH HH HH HH HH HH HH;");
        break;
    case 8:
        ui->data->setInputMask("HH HH HH HH HH HH HH HH;");
        break;
    }
}


ByteSend::~ByteSend()
{
    delete ui;
}
