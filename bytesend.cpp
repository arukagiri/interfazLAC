#include "bytesend.h"
#include "ui_bytesend.h"
#include <stdint.h>
#include "PC.h"
#include <QMessageBox>
#include <QValidator>
#include <QLineEdit>
#include "lvalidartor.h"
#include <QIntValidator>


ByteSend::ByteSend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ByteSend)
{
    ui->setupUi(this);

    mw = qobject_cast<MainWindow*>(this->parent());

    ui->dlc->addItem("1",QVariant(1));
    ui->dlc->addItem("2",QVariant(2));
    ui->dlc->addItem("3",QVariant(3));
    ui->dlc->addItem("4",QVariant(4));
    ui->dlc->addItem("5",QVariant(5));
    ui->dlc->addItem("6",QVariant(6));
    ui->dlc->addItem("7",QVariant(7));
    ui->dlc->addItem("8",QVariant(8));
    ui->dlc->setCurrentIndex(0);

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
    //ui->id1->setCursorPosition(0);

    ui->id2->setInputMask("9");   //este byte debe ser menor a 7 porque la id es de 11 bits
    QValidator *val = new QIntValidator(0, 7, this);
    ui->id2->setValidator(val);
    ui->id2->setText("");
    //ui->id2->setCursorPosition(0);

}

void ByteSend::on_pushButton_clicked(){

    LACAN_MSG msg;
    uint8_t byteID1, byteID2;

    byteID1=text2int( ui->id1->text());
    byteID2=text2int(ui->id2->text());
    msg.ID=(byteID2<<8 | byteID1)&LACAN_ID_STANDARD_MASK;   //los pongo al reves porque el adaptador me los davuelta

    msg.DLC = actual_dlc;

    msg.BYTE0=text2int(ui->data0->text());
    msg.BYTE1=text2int(ui->data1->text());
    msg.BYTE2=text2int(ui->data2->text());
    msg.BYTE3=text2int(ui->data3->text());
    msg.BYTE4=text2int(ui->data4->text());
    msg.BYTE5=text2int(ui->data5->text());
    msg.BYTE6=text2int(ui->data6->text());
    msg.BYTE7=text2int(ui->data7->text());

    serialsend2(*(mw->serial_port),msg);
    //mw->msg_log.push_back(msg);
}

void ByteSend::on_dlc_currentIndexChanged(int index){
    actual_dlc = ui->dlc->itemData(index).toInt();

    switch (actual_dlc) {
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
}

ByteSend::~ByteSend(){
    delete ui;
}

uint8_t ByteSend::text2int(const QString letras){
    const QString text = letras;
    bool ok = true;
    int value = text.toInt(&ok, 16);
    if ( ok )
    {
        return value;
    }
    else{
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this,"Error","Error al enviar el mensaje");
        if(reply){
            this->close();
        }
    }
}



