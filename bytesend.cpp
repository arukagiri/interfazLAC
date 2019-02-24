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

void ByteSend::on_sendButton_clicked(){

    LACAN_MSG msg;
    uint8_t byteID1, byteID2;
    int base=16;//queremos enviar los datos como hexadecimales

    bool okID1=true;
    bool okID2=true;

    byteID1=ui->id1->text().toInt(&okID1,base);
    byteID2=ui->id2->text().toInt(&okID2,base);

    msg.ID=(byteID2<<8 | byteID1)&LACAN_ID_STANDARD_MASK;   //los pongo al reves porque el adaptador me los davuelta

    msg.DLC = actual_dlc;

    bool okDLC=true;

    switch(actual_dlc){
        case 8:
            msg.BYTE7=ui->data7->text().toInt(&okDLC,base);
            if(!okDLC){
                break;
            }
        [[clang::fallthrough]]; case 7:
            msg.BYTE6=ui->data6->text().toInt(&okDLC,base);
            if(!okDLC){
                break;
            }
        [[clang::fallthrough]]; case 6:
            msg.BYTE5=ui->data5->text().toInt(&okDLC,base);
            if(!okDLC){
                break;
            }
        [[clang::fallthrough]]; case 5:
            msg.BYTE4=ui->data4->text().toInt(&okDLC,base);
            if(!okDLC){
                break;
            }
        [[clang::fallthrough]]; case 4:
            msg.BYTE3=ui->data3->text().toInt(&okDLC,base);
            if(!okDLC){
                break;
            }
        [[clang::fallthrough]]; case 3:
            msg.BYTE2=ui->data2->text().toInt(&okDLC,base);
            if(!okDLC){
                break;
            }
        [[clang::fallthrough]]; case 2:
            msg.BYTE1=ui->data1->text().toInt(&okDLC,base);
            if(!okDLC){
                break;
            }
        [[clang::fallthrough]]; case 1:
            msg.BYTE0=ui->data0->text().toInt(&okDLC,base);
            if(!okDLC){
                break;
            }
    }
    if(okDLC&&okID1&&okID2){
        serialsend2(*(mw->serial_port),msg);
        //mw->msg_log.push_back(msg);
    }else{
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this,"Error","Error al enviar el mensaje");
    }
    this->close();
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


