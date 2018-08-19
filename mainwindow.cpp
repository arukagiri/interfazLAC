#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "comandar.h"
#include "consultar.h"
#include "enviar_mensaje.h"
#include "estadored.h"
#include "PC.h"
#include <QDebug>
#include "LACAN_SEND.h"
#include <QtGui>
#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QDateTime>
#include <QMessageBox>
#include "lacan_detect.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include "LACAN_REC.h"


void agregar_textlog(ABSTRACTED_MSG abs_msg, QString way){
    static uint8_t cont=0;
    QString file_folder=QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString file_path="";

    if(QDir(file_folder+"/Log de Mensajes LACAN").exists()){
        file_path = file_folder+"/Log de Mensajes LACAN/"+abs_msg.curr_time.mid(3,7)+".txt";
    }else if(QDir(file_folder).exists()){
        QDir(file_folder).mkdir("Log de Mensajes LACAN");
        file_path = file_folder+"/Log de Mensajes LACAN/"+abs_msg.curr_time.mid(3,7)+".txt";
    }else {
        file_path = QDir::currentPath()+"/"+abs_msg.curr_time.mid(3,7)+".txt";
    }

    QFile file(file_path);
    file.open(QFile::WriteOnly | QFile::Append | QFile::Text);
    QTextStream out(&file);
    if(!(cont%25)){
        out<<"Sentido"<<"\t"<<"Fecha y hora"<<"\t\t"<<"Destino"<<"\t\t\t"<<"Funcion"<<"\t"<<"Tipo de variable"<<"\t"<<"Valor de variable"<<"\t"<<"Comando"<<"\t"<<"Codigo de ACK"<<"\t"<<"Codigo de error"<<"\n";
        cont=0;
    }
    out<<way<<"\t"<<abs_msg.curr_time<<"\t"<<abs_msg.dest<<"\t"<<abs_msg.fun<<"\t"<<abs_msg.var_type<<"\t"<<abs_msg.var_val<<"\t"<<abs_msg.com<<"\t"<<abs_msg.ack_code<<"\t"<<abs_msg.err_code<<"\n";
    file.flush();
    file.close();
    cont++;
}

ABSTRACTED_MSG abstract_msg(vector <LACAN_MSG> msg_log){
    QString format_time="hh:mm:ss";
    QString format_date="dd.MM.yyyy";
    QDateTime curr_date_time=QDateTime::currentDateTime();
    ABSTRACTED_MSG abs_msg={"","","","","","","","", curr_date_time.toString(format_date)+" "+curr_date_time.toString(format_time)+"hs"};

    //DESTINO
    switch((msg_log.back().BYTE0 >> LACAN_BYTE0_RESERVED)&LACAN_IDENT_MASK){
    case LACAN_ID_BOOST:
        abs_msg.dest="Boost\t";
        break;
    case LACAN_ID_GEN:
        abs_msg.dest="Generador Eolico";
        break;
    case LACAN_ID_BROADCAST:
        abs_msg.dest="Broadcast\t";
        break;
    case LACAN_ID_VOLANTE:
        abs_msg.dest="Volante de Inercia";
        break;
    default:
        abs_msg.dest="No especificada/No soportada";
        break;
    }



    //FUNCION
    switch((msg_log.back().ID&LACAN_FUN_MASK)>>LACAN_IDENT_BITS){
    case LACAN_FUN_ERR:
        abs_msg.fun="Error";
        qDebug()<<"ERROR DENTRO DE ABS, MSG = "<<msg_log.back().BYTE1;
        abs_msg.err_code=detect_err(msg_log.back().BYTE1);
        break;

    case LACAN_FUN_DO:
        abs_msg.fun="Do";
        abs_msg.ack_code=QString::number(msg_log.back().BYTE1);
        abs_msg.com=detect_cmd(msg_log.back().BYTE2);
        break;

    case LACAN_FUN_SET:
        abs_msg.fun="Set";
        abs_msg.var_val=QString::number(msg_log.back().BYTE3);
        abs_msg.ack_code=QString::number(msg_log.back().BYTE1);
        abs_msg.var_type=detect_var(msg_log.back().BYTE2);
        break;

    case LACAN_FUN_QRY:
        abs_msg.fun="Query";
        abs_msg.ack_code=QString::number(msg_log.back().BYTE1);
        abs_msg.var_type=detect_var(msg_log.back().BYTE2);
        break;

    case LACAN_FUN_ACK:
        abs_msg.fun="Acknowledge";
        abs_msg.ack_res=detect_res(msg_log.back().BYTE2);
        abs_msg.ack_code=QString::number(msg_log.back().BYTE1);
        break;

    case LACAN_FUN_POST:
        abs_msg.fun="Post";
        abs_msg.var_type=detect_var(msg_log.back().BYTE1);
        abs_msg.var_val=QString::number(msg_log.back().BYTE2);
        break;

    case LACAN_FUN_HB:
        abs_msg.fun="Heartbeat";
        break;

    default:
        abs_msg.fun="No especificada/soportada";
    }
    return abs_msg;
}

MainWindow::MainWindow(QSerialPort &serial_port0,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial_port=&serial_port0;
    do_log=FALSE;
    outlog_cont=0;
    inlog_cont=0;
    ui->the_one_true_list_DESTINO->addItem("Broadcast");
    ui->the_one_true_list_DESTINO->addItem("Generador Eolico");
    ui->the_one_true_list_DESTINO->addItem("Volante de Inercia");
    ui->the_one_true_list_DESTINO->addItem("Boost");

    //this->setLayout(ui->verticalLayout_7);

    QStringList TableHeader;
    TableHeader<<"Destino"<<"Funcion"<<"Variable"<<"Valor"<<"Comando"<<"Codigo de ack"<<"Codigo de error"<<"Fecha y Hora";

    ui->tableWidget_received->setRowCount(10);
    ui->tableWidget_received->setColumnCount(8);
    ui->tableWidget_received->setHorizontalHeaderLabels(TableHeader);
    ui->tableWidget_received->verticalHeader()->setVisible(false);
    ui->tableWidget_received->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_received->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_received->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_received->setShowGrid(false);
    ui->tableWidget_received->setStyleSheet("QTableView {selection-background-color: blue;}");

    ui->tableWidget_sent->setRowCount(10);
    ui->tableWidget_sent->setColumnCount(8);
    ui->tableWidget_sent->setHorizontalHeaderLabels(TableHeader);
    ui->tableWidget_sent->verticalHeader()->setVisible(false);
    ui->tableWidget_sent->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_sent->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_sent->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_sent->setShowGrid(false);
    ui->tableWidget_sent->setStyleSheet("QTableView {selection-background-color: blue;}");

    connect(serial_port, SIGNAL(readyRead()), this, SLOT(handleRead()));
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_button_COMANDAR_clicked()
{
    uint16_t dest=0xFF;
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Broadcast"){
        dest=LACAN_ID_BROADCAST;
    }
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Generador Eolico"){
        dest=LACAN_ID_GEN;
    }
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Volante de Inercia"){
        dest=LACAN_ID_VOLANTE;
    }
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Boost"){
        dest=LACAN_ID_BOOST;
    }

    Comandar *comwin = new Comandar(*serial_port,msg_ack,code,msg_log,do_log,dest,this);
    comwin->setModal(true);
    comwin->show();
}

void MainWindow::on_button_CONSULTAR_clicked()
{
    uint16_t dest=0xFF;
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Broadcast"){
        dest=LACAN_ID_BROADCAST;
    }
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Generador Eolico"){
        dest=LACAN_ID_GEN;
    }
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Volante de Inercia"){
        dest=LACAN_ID_VOLANTE;
    }
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Boost"){
        dest=LACAN_ID_BOOST;
    }
    Consultar *conswin = new Consultar(*serial_port,msg_ack,code,msg_log,do_log,dest,this);
    conswin->setModal(true);
    conswin->show();

    //connect(ui->list_VARIABLE_QRY,SIGNAL(currentTextChanged(QString)),this,SLOT(CONSULTA_Changed()));

    //connect(conswin,SIGNAL(conswin->destroyed()),this,SLOT(agregar_log()));
}

void MainWindow::on_button_ENVIAR_MENSAJE_clicked()
{
    uint16_t dest=0xFF;
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Broadcast"){
        dest=LACAN_ID_BROADCAST;
    }
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Generador Eolico"){
        dest=LACAN_ID_GEN;
    }
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Volante de Inercia"){
        dest=LACAN_ID_VOLANTE;
    }
    if((ui->the_one_true_list_DESTINO->currentItem()->text())=="Boost"){
        dest=LACAN_ID_BOOST;
    }

    Enviar_Mensaje *envwin = new Enviar_Mensaje(*serial_port,msg_ack,code,msg_log,do_log,dest,this);

    envwin->setModal(true);
    envwin->show();

}

void MainWindow::on_button_ESTADO_RED_clicked()
{
    EstadoRed *estwin = new EstadoRed(this);
    estwin->setModal(true);
    estwin->show();
}

void MainWindow::agregar_log_sent(vector <LACAN_MSG> msg_log){
    ABSTRACTED_MSG abs_msg;
    abs_msg=abstract_msg(msg_log);
    if(do_log){
        ui->tableWidget_sent->setItem(outlog_cont, 0, new QTableWidgetItem(abs_msg.dest));
        ui->tableWidget_sent->setItem(outlog_cont, 1, new QTableWidgetItem(abs_msg.fun));
        ui->tableWidget_sent->setItem(outlog_cont, 2, new QTableWidgetItem(abs_msg.var_type));
        ui->tableWidget_sent->setItem(outlog_cont, 3, new QTableWidgetItem(abs_msg.var_val));
        ui->tableWidget_sent->setItem(outlog_cont, 4, new QTableWidgetItem(abs_msg.com));
        ui->tableWidget_sent->setItem(outlog_cont, 5, new QTableWidgetItem(abs_msg.ack_code));
        ui->tableWidget_sent->setItem(outlog_cont, 6, new QTableWidgetItem(abs_msg.err_code));
        ui->tableWidget_sent->setItem(outlog_cont, 7, new QTableWidgetItem(abs_msg.curr_time));
        outlog_cont++;
    }
    agregar_textlog(abs_msg,"Enviado");
}


void MainWindow::agregar_log_rec(vector <LACAN_MSG> msg_log){
    ABSTRACTED_MSG abs_msg;
    abs_msg=abstract_msg(msg_log);
    if(do_log){
        ui->tableWidget_received->setItem(inlog_cont, 0, new QTableWidgetItem(abs_msg.dest));
        ui->tableWidget_received->setItem(inlog_cont, 1, new QTableWidgetItem(abs_msg.fun));
        ui->tableWidget_received->setItem(inlog_cont, 2, new QTableWidgetItem(abs_msg.var_type));
        ui->tableWidget_received->setItem(inlog_cont, 3, new QTableWidgetItem(abs_msg.var_val));
        ui->tableWidget_received->setItem(inlog_cont, 4, new QTableWidgetItem(abs_msg.com));
        ui->tableWidget_received->setItem(inlog_cont, 5, new QTableWidgetItem(abs_msg.ack_code));
        ui->tableWidget_received->setItem(inlog_cont, 6, new QTableWidgetItem(abs_msg.err_code));
        ui->tableWidget_received->setItem(inlog_cont, 7, new QTableWidgetItem(abs_msg.curr_time));
        inlog_cont++;
    }
    agregar_textlog(abs_msg,"Recibido");
}

void MainWindow::on_button_START_clicked()
{
    QMessageBox::StandardButton reply;

    if(do_log==TRUE || outlog_cont>0){
        reply=QMessageBox::warning(this,"Aviso de perdida de mensajes","¿Esta seguro de comenzar una nueva sesion de registro de mensajes?\n Se borraran los mensajes de la sesion anterior",QMessageBox::Yes|QMessageBox::No);
        if(reply==QMessageBox::Yes){
            ui->tableWidget_received->clearContents();
            ui->tableWidget_sent->clearContents();
            outlog_cont=0;
            msg_log.clear();
        }
    }
    do_log=TRUE;
}

void MainWindow::on_button_STOP_clicked()
{
    do_log=FALSE;
}

void MainWindow::handleRead(){
    bool newmsgflag=0;
    static char pila[100]={0};
    qDebug()<<"algo llego\n";
    newmsgflag=readport2(pila, *serial_port);
    /*static uint16_t notsup_count, notsup_gen;
    if(newmsgflag){
        LACAN_MSG msg;
        int result;
        msg=mensaje_recibido2(pila);
        msg_log.push_back(msg);
        result=LACAN_Msg_Handler(msg,hb_con,msg_ack,notsup_count,notsup_gen);
        this->agregar_log_rec(msg_log);
    }*/

}
