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
#include "addnewdevdialog.h"
#include <QColor>
#include "lacan_limits_gen.h"
#include "bytesend.h"
#include <stdlib.h>
#include <string.h>
#include "lacan_limits_vol.h"
#include <QThread>
#include "tiempo.h"
#include "senderthread.h"

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
    ABSTRACTED_MSG abs_msg={"","","","","","","","","", curr_date_time.toString(format_date)+" "+curr_date_time.toString(format_time)+"hs"};
    float val_float;
    data_can val_union;

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

    //REMITENTE
    switch(msg_log.back().ID&LACAN_IDENT_MASK){
    case LACAN_ID_BOOST:
        abs_msg.orig="Boost\t";
        break;
    case LACAN_ID_GEN:
        abs_msg.orig="Generador Eolico";
        break;
    case LACAN_ID_BROADCAST:
        abs_msg.orig="Broadcast\t";
        break;
    case LACAN_ID_VOLANTE:
        abs_msg.orig="Volante de Inercia";
        break;
    default:
        abs_msg.orig="No especificada/No soportada";
        break;
    }


    //FUNCION
    switch((msg_log.back().ID&LACAN_FUN_MASK)>>LACAN_IDENT_BITS){
    case LACAN_FUN_ERR:
        abs_msg.fun="Error";
        abs_msg.err_code=detect_err(msg_log.back().BYTE1);
        break;

    case LACAN_FUN_DO:
        abs_msg.fun="Do";
        abs_msg.ack_code=QString::number(msg_log.back().BYTE1);
        abs_msg.com=detect_cmd(msg_log.back().BYTE2);
        break;

    case LACAN_FUN_SET:
        abs_msg.fun="Set";
        abs_msg.ack_code=QString::number(msg_log.back().BYTE1);
        abs_msg.var_type=detect_var(msg_log.back().BYTE2);

        if(abs_msg.var_type=="Modo"){
            abs_msg.var_val = detect_mode(msg_log.back().BYTE3);
        }
        else{
        val_union.var_char[0]=char(msg_log.back().BYTE3);
        val_union.var_char[1]=char(msg_log.back().BYTE4);
        val_union.var_char[2]=char(msg_log.back().BYTE5);
        val_union.var_char[3]=char(msg_log.back().BYTE6);

        val_float = val_union.var_float;
        abs_msg.var_val=QString::number(double(val_float));
        }
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
        if(abs_msg.var_type=="Modo"){
            abs_msg.var_val = detect_mode(msg_log.back().BYTE2);
        }
        else{
            val_union.var_char[0]=char(msg_log.back().BYTE2);
            val_union.var_char[1]=char(msg_log.back().BYTE3);
            val_union.var_char[2]=char(msg_log.back().BYTE4);
            val_union.var_char[3]=char(msg_log.back().BYTE5);

            val_float = val_union.var_float;
            abs_msg.var_val=QString::number(double(val_float));
        }
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
    do_log=false;
    ERflag= false;
    NoUSB=false;
    dest=LACAN_ID_BROADCAST;
    outlog_cont=0;
    inlog_cont=0;
    periodicTimer = new QTimer();
    periodicTimer->start(HB_TIME);
    connect(periodicTimer,SIGNAL(timeout()),this,SLOT(do_stuff()));

    SenderThread* msgSender=new SenderThread(this);
    connect(msgSender,SIGNAL(sendTimeout()),this,SLOT(handleSendTimeout()));
    msgSender->start();


    ui->the_one_true_list_DESTINO->addItem("Broadcast");
    ui->the_one_true_list_DESTINO->addItem("Generador Eolico");
    ui->the_one_true_list_DESTINO->addItem("Volante de Inercia");
    ui->the_one_true_list_DESTINO->addItem("Boost");

    disp_map["Broadcast"]=LACAN_ID_BROADCAST;
    disp_map["Generador Eolico"]=LACAN_ID_GEN;
    disp_map["Volante de Inercia"]=LACAN_ID_VOLANTE;
    disp_map["Boost"]=LACAN_ID_BOOST;
    HB_CONTROL* newdev;
    newdev=new HB_CONTROL();
    newdev->device=LACAN_ID_GEN;
    newdev->hb_status=ACTIVE;
    newdev->hb_timer.start(DEAD_HB_TIME);
    hb_con.push_back(newdev);
    newdev=new HB_CONTROL();
    newdev->device=LACAN_ID_BOOST;
    newdev->hb_status=ACTIVE;
    newdev->hb_timer.start(DEAD_HB_TIME+100);
    hb_con.push_back(newdev);
    newdev=new HB_CONTROL();
    newdev->device=LACAN_ID_VOLANTE;
    newdev->hb_status=ACTIVE;
    newdev->hb_timer.start(DEAD_HB_TIME+200);
    hb_con.push_back(newdev);
    for(vector<HB_CONTROL*>::iterator it_hb=hb_con.begin(); it_hb < hb_con.end(); it_hb++){
         connect(&((*it_hb)->hb_timer), SIGNAL(timeout()), this, SLOT(verificarHB()));
    }
    QStringList TableHeader_send;
    QStringList TableHeader_rece;
    TableHeader_send<<"Destino"<<"Funcion"<<"Variable"<<"Valor"<<"Comando"<<"Codigo de ack"<<"Resultado ack"<<"Codigo de error"<<"Fecha y Hora";
    TableHeader_rece<<"Origen"<<"Funcion"<<"Variable"<<"Valor"<<"Comando"<<"Codigo de ack"<<"Resultado ack"<<"Codigo de error"<<"Fecha y Hora";

    ui->tableWidget_received->setRowCount(list_rec_cont);
    ui->tableWidget_received->setColumnCount(9);
    ui->tableWidget_received->setHorizontalHeaderLabels(TableHeader_rece);
    ui->tableWidget_received->verticalHeader()->setVisible(false);
    ui->tableWidget_received->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_received->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_received->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_received->setShowGrid(false);
    ui->tableWidget_received->setStyleSheet("QTableView {selection-background-color: blue;}");

    ui->tableWidget_sent->setRowCount(list_send_cont);
    ui->tableWidget_sent->setColumnCount(9);
    ui->tableWidget_sent->setHorizontalHeaderLabels(TableHeader_send);
    ui->tableWidget_sent->verticalHeader()->setVisible(false);
    ui->tableWidget_sent->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_sent->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget_sent->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget_sent->setShowGrid(false);
    ui->tableWidget_sent->setStyleSheet("QTableView {selection-background-color: blue;}");

    create_varmap_gen();
    create_varmap_vol();




    connect(serial_port, SIGNAL(readyRead()), this, SLOT(handleRead()));
    //VOLVER A PONER EL PRIMERO, VERSION LULI
    connect(serial_port, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(handlePortError(QSerialPort::SerialPortError)));
    //LA MALA
    //connect(serial_port, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handlePortError(QSerialPort::SerialPortError)));

}

void MainWindow::handlePortError(QSerialPort::SerialPortError error){
    if(error!=QSerialPort::NoError){
        NoUSB=true;
        if(serial_port->isOpen()){
            serial_port->close();
        }
        QMessageBox::warning(this, "Ups","Error con el puerto USB serie",QMessageBox::Ok);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::verificar_destino(){
    dest=disp_map[(ui->the_one_true_list_DESTINO->currentItem()->text())];
}

void MainWindow::on_button_COMANDAR_clicked()
{

    verificar_destino();
    if(dest  != LACAN_ID_BROADCAST){
        Comandar *comwin = new Comandar(this);

        comwin->setModal(true);
        comwin->show();
    }
    else{
        QMessageBox::warning(this, "Error","No puede comandar a todos los dispositivos juntos",QMessageBox::Ok);
    }
}

void MainWindow::on_button_CONSULTAR_clicked()
{

    verificar_destino();
    Consultar *conswin = new Consultar(this);
    conswin->setModal(true);
    conswin->show();

    //connect(ui->list_VARIABLE_QRY,SIGNAL(currentTextChanged(QString)),this,SLOT(CONSULTA_Changed()));

    //connect(conswin,SIGNAL(conswin->destroyed()),this,SLOT(agregar_log()));
}

void MainWindow::on_button_ENVIAR_MENSAJE_clicked()
{

    verificar_destino();
    Enviar_Mensaje *envwin = new Enviar_Mensaje(this);

    envwin->setModal(true);
    envwin->show();
}

void MainWindow::on_button_ESTADO_RED_clicked()
{
    EstadoRed *estwin = new EstadoRed(this);
    estwin->setModal(true);
    estwin->show();
    ERflag=true;
    connect(this, SIGNAL(postforER_arrived(LACAN_MSG)), estwin, SLOT(ERpost_Handler(LACAN_MSG)));
}

void MainWindow::agregar_log_sent(){
    ABSTRACTED_MSG abs_msg;
    abs_msg=abstract_msg(msg_log);
    if(do_log){
        if(list_send_cont>=LOG_LIMIT){  //limite de mensajes
        do_log=FALSE;
        QMessageBox::StandardButton reply;
        reply=QMessageBox::warning(this,"Limite de log alcanzado.","Se ha superado la cantidad maxima de mensajes del log. Desea iniciar una nueva sesion?\n Se borraran los mensajes de la sesion anterior",QMessageBox::Yes|QMessageBox::No);
            if(reply==QMessageBox::Yes){
                ui->tableWidget_received->clearContents();
                ui->tableWidget_sent->clearContents();
                outlog_cont=0;
                msg_log.clear();
                list_rec_cont = 0;
                list_send_cont = 0;
                ui->tableWidget_received->setRowCount(list_rec_cont);
                ui->tableWidget_sent->setRowCount(list_send_cont);
                do_log=TRUE;
            }
        }
        else{
        list_send_cont++;
        ui->tableWidget_sent->setRowCount(list_send_cont);
        ui->tableWidget_sent->setItem(outlog_cont, 0, new QTableWidgetItem(abs_msg.dest));
        ui->tableWidget_sent->setItem(outlog_cont, 1, new QTableWidgetItem(abs_msg.fun));
        ui->tableWidget_sent->setItem(outlog_cont, 2, new QTableWidgetItem(abs_msg.var_type));
        ui->tableWidget_sent->setItem(outlog_cont, 3, new QTableWidgetItem(abs_msg.var_val));
        ui->tableWidget_sent->setItem(outlog_cont, 4, new QTableWidgetItem(abs_msg.com));
        ui->tableWidget_sent->setItem(outlog_cont, 5, new QTableWidgetItem(abs_msg.ack_code));
        ui->tableWidget_sent->setItem(outlog_cont, 6, new QTableWidgetItem(abs_msg.ack_res));
        ui->tableWidget_sent->setItem(outlog_cont, 7, new QTableWidgetItem(abs_msg.err_code));
        ui->tableWidget_sent->setItem(outlog_cont, 8, new QTableWidgetItem(abs_msg.curr_time));
        outlog_cont++;
    }
    agregar_textlog(abs_msg,"Enviado");
    }
}

void MainWindow::agregar_log_rec(vector <LACAN_MSG> msg_log){
    ABSTRACTED_MSG abs_msg;
    abs_msg=abstract_msg(msg_log);
    if(do_log){
        if(list_rec_cont>=LOG_LIMIT){  //limite de mensajes
        QMessageBox::StandardButton reply;
        reply=QMessageBox::warning(this,"Limite de log alcanzado.","Se ha superado la cantidad maxima de mensajes del log. Desea iniciar una nueva sesion?\n Se borraran los mensajes de la sesion anterior",QMessageBox::Yes|QMessageBox::No);
            if(reply==QMessageBox::Yes){
                ui->tableWidget_received->clearContents();
                ui->tableWidget_sent->clearContents();
                outlog_cont=0;
                msg_log.clear();
                list_rec_cont = 0;
                list_send_cont = 0;
                ui->tableWidget_received->setRowCount(list_rec_cont);
                ui->tableWidget_sent->setRowCount(list_send_cont);
                do_log=TRUE;
            }
            else{
                on_button_STOP_clicked();
            }
        }
        else{
        list_rec_cont++;
        ui->tableWidget_received->setRowCount(list_rec_cont);
        ui->tableWidget_received->setItem(inlog_cont, 0, new QTableWidgetItem(abs_msg.orig));
        ui->tableWidget_received->setItem(inlog_cont, 1, new QTableWidgetItem(abs_msg.fun));
        ui->tableWidget_received->setItem(inlog_cont, 2, new QTableWidgetItem(abs_msg.var_type));
        ui->tableWidget_received->setItem(inlog_cont, 3, new QTableWidgetItem(abs_msg.var_val));
        ui->tableWidget_received->setItem(inlog_cont, 4, new QTableWidgetItem(abs_msg.com));
        ui->tableWidget_received->setItem(inlog_cont, 5, new QTableWidgetItem(abs_msg.ack_code));
        ui->tableWidget_received->setItem(inlog_cont, 6, new QTableWidgetItem(abs_msg.ack_res));
        ui->tableWidget_received->setItem(inlog_cont, 7, new QTableWidgetItem(abs_msg.err_code));
        ui->tableWidget_received->setItem(inlog_cont, 8, new QTableWidgetItem(abs_msg.curr_time));
        inlog_cont++;
        }
    }
    agregar_textlog(abs_msg,"Recibido");
}

void MainWindow::on_button_START_clicked()
{
    QMessageBox::StandardButton reply;

    if(outlog_cont>0 || inlog_cont>0){
        reply=QMessageBox::warning(this,"Aviso de perdida de mensajes","¿Esta seguro de comenzar una nueva sesion de registro de mensajes?\n Se borraran los mensajes de la sesion anterior",QMessageBox::Yes|QMessageBox::No);
        if(reply==QMessageBox::Yes){
            ui->tableWidget_received->clearContents();
            ui->tableWidget_sent->clearContents();
            outlog_cont=0;
            msg_log.clear();
            list_rec_cont = 0;
            list_send_cont = 0;
            ui->tableWidget_received->setRowCount(list_rec_cont);
            ui->tableWidget_sent->setRowCount(list_send_cont);
        }
    }
    do_log=TRUE;
}

void MainWindow::on_button_STOP_clicked()
{
    do_log=FALSE;
    for(int i=0;i<50;i++){
        LACAN_Acknowledge(this,1,LACAN_RES_OK);
    }
}

void MainWindow::verificarHB(){
    //Encargada de verificar que todos los dispositivos de la red esten activos mediante el HB,
    //cada nodo debe enviar HB cada un cierto tiempo(HB_TIME), si este no se recibe dentro de un periodo de
    //tolerancia (DEAD_HB_TIME), esta funcion se encarga de que el programa lo considere inactivo
    for(vector<HB_CONTROL*>::iterator it_hb=hb_con.begin(); it_hb < hb_con.end(); it_hb++){
        //ver de cambiar condicion porq el remaining empieza a correr de nuevo cuando entra al slot
        if(((*it_hb)->hb_timer.remainingTime()> 15000) && ((*it_hb)->hb_status==ACTIVE)){
            (*it_hb)->hb_status=INACTIVE;
            (*it_hb)->hb_timer.stop();
            erase_device_ui(uint16_t((*it_hb)->device));
        }
    }
}

void MainWindow::verificarACK(){
    //Esta función verifica principalmente que llegue el acknowledge de un mensaje enviado(SET, DO, QUERY), si el mismo
    //no llega luego de un periodo de tiempo(WAIT_ACK_TIME) se setea al mensaje con la condicion ACK_TIMEOUT para que otra
    //parte del programa realice las acciones correspondientes, además un mensaje que recibio su correspondiente acknowledge
    //se sigue almacenando dentro del vector por un tiempo determinado (DEAD_MSJ_ACK_TIME) antes de que sea borrado

    for(vector<TIMED_MSG*>::iterator it_ack=msg_ack.begin();it_ack<msg_ack.end();it_ack++){

        if((*it_ack)->ack_status==RECEIVED){        //si llego el ack..
            //if((*it_ack)->ack_timer.remainingTime()<=0){
            if(!((*it_ack)->ack_timer.isActive())){
                msg_ack.erase(it_ack);                  //si hace mucho que se mando el mensaje y no se hizo nada lo borramos
            }
        }
        else{
             if(!((*it_ack)->ack_timer.isActive())){  //si no llego el ack y se vencion el timer
                (*it_ack)->ack_status=ACK_TIMEOUT;
                if((*it_ack)->retries<=0 && show_miss_ack_flag == 0){  //si no quedan reintentos
                    if((*it_ack)->show_miss_ack==1){ //si el mensaje se mando desde la mainwindows
                        show_miss_ack_flag = 1;
                        QMessageBox::StandardButton reply;
                        QString name = disp_map.key(((*it_ack)->msg.BYTE0) >> LACAN_BYTE0_RESERVED);
                        QString mje = "Se ha agotado el tiempo de espera de la respuesta del " + name + ".";
                        reply = QMessageBox::warning(this,"Error al enviar",mje,QMessageBox::Ok);
                        if(reply){
                            show_miss_ack_flag = 0;
                        }
                    }
                    disconnect(&(msg_ack.back()->ack_timer),SIGNAL(timeout()), this, SLOT(verificarACK()));
                    no_ACK_Handler();
                    msg_ack.erase(it_ack);
                }
                else{   //si quedan reintentos, vuelve a enviar el mensaje y descuenta reintentos
                    serialsend2(*(this->serial_port),(*it_ack)->msg);
                    (*it_ack)->ack_status=PENDACK;
                    (*it_ack)->ack_timer.setSingleShot(true);
                    (*it_ack)->ack_timer.start(WAIT_ACK_TIME);
                    (*it_ack)->retries--;
                    this->msg_log.push_back((*it_ack)->msg);
                    this->agregar_log_sent();
                }
             }
        }
    }
}

void MainWindow::no_ACK_Handler(void){}

void MainWindow::handleRead(){
    uint16_t cant_msg=0, msgLeft=0;
    uint16_t first_byte[33]={0};        //notar que el primer elemento de este vector, siempre es 0
    static vector<char> pila;
    static uint16_t notsup_count, notsup_gen;

    cant_msg=readport2(pila, first_byte, *serial_port); //devuelve la cantidad de mensajes que se levantaron del puerto
    msgLeft = cant_msg;
    for(int i=0;i<cant_msg;i++){    //aca hay que ir recorriendo la pila, que puede tener mas de un mensaje
        LACAN_MSG msg;
        int result=0;
        uint prevsize=0;
        char sub_pila[13]={0}; //para ir guardando los mensajes parciales que pueden estar adentro de la pila. tiene la longitud del tamano maximo de un mensaje del adaptador

        //sub_pila=pila(first_byte[i],first_byte[i+1]-1);  //extraigo de pila, un solo mensaje
        int h=0;
        for(uint j=0;j<first_byte[1];j++){
            sub_pila[h]=pila.at(j);
            h++;
        } //extraigo de pila, un solo mensaje
        pila.erase(pila.begin()+first_byte[i],pila.begin()+first_byte[i+1]); //borro el mensaje que acabo de copiar a la sub pila
        //reconfiguro los indices q indican el comienzo de cada mensaje
        for(uint k=1; k<msgLeft; k++){
            first_byte[k]=first_byte[k+1];
        }
        msgLeft--;

        msg=mensaje_recibido2(sub_pila);
        msg_log.push_back(msg);
        prevsize = hb_con.size();

        if((msg.ID>>LACAN_IDENT_BITS==LACAN_FUN_POST)&&ERflag){
            emit postforER_arrived(msg);
        }//else{
            result=LACAN_Msg_Handler(msg,hb_con,msg_ack,notsup_count,notsup_gen,disp_map,this);
            //VER A partir de mensajes recibidos solo podria aumentar el numero de dispositivos conectados, no de msj con ACK
            if(hb_con.size()>prevsize){
                connect(&(hb_con.back()->hb_timer),SIGNAL(timeout()),this,SLOT(verificarHB()));//VER sin & no compila, ver si anda asi
            }
        //}
        agregar_log_rec(msg_log);
    }
}

void MainWindow::change_ERflag(){
    if(ERflag){
        ERflag=false;
    }else{
        ERflag=true;
    }
}

void MainWindow::add_new_device(uint16_t source){

    qDebug()<<"\nAgregando nuevo dispositivo\n";//Evaluar la posibildad de q llegue un heartbeat de un dispositivo que no existe (corrupcion del source durante el envio)

    newdev.device=source;
    newdev.hb_timer.start(DEAD_HB_TIME);
    newdev.hb_status=ACTIVE;

    AddNewDevDialog *diag=new AddNewDevDialog(this);
    diag->setModal(true);
    connect(diag, SIGNAL(dev_name_set(QString)), this, SLOT(add_dev_name(QString)));
    diag->show();

}

void MainWindow::add_dev_name(QString newdevname){
    if(!disp_map.contains(newdevname)){
        hb_con.push_back(&newdev);
        disp_map[newdevname]=newdev.device;
        ui->the_one_true_list_DESTINO->addItem(newdevname);
    }else if(disp_map[newdevname]==newdev.device){
        ui->the_one_true_list_DESTINO->addItem(newdevname);
    }else{
        QMessageBox::warning(this, "Error agregando dispositivo", "El nombre ingresado ya existe\n ", QMessageBox::Ok );
        add_new_device(newdev.device);
    }
}

void MainWindow::erase_device_ui(uint16_t inactiveDev){

    QString name = disp_map.key(inactiveDev);
    qDeleteAll(ui->the_one_true_list_DESTINO->findItems(name, Qt::MatchFixedString));
    //int row=ui->the_one_true_list_DESTINO->row(new QListWidgetItem(disp_map.key(inactiveDev)) );
    //ui->the_one_true_list_DESTINO->takeItem(row);
}

void MainWindow::add_device_ui(uint16_t reactivatedDev){
    QString name = disp_map.key(reactivatedDev);
    ui->the_one_true_list_DESTINO->addItem(name);
}

void MainWindow::LACAN_NOTSUP_Handler(uint16_t source, uint16_t& notsup_count, uint16_t& notsup_gen, uint8_t code){
    //En el caso de que llegue a la computadora un mensaje que no tiene sentido, como por ejemplo un SET, DO o QUERY,
    //se ejecuta esta funcion para monitorear el flujo de mensajes recibidos no soportados
    dest=source;
    LACAN_Acknowledge(this,code,LACAN_FAILURE);
    notsup_count++;
    if(source&LACAN_ID_GEN){
        notsup_gen++;
    }

}

int MainWindow::LACAN_Msg_Handler(LACAN_MSG &mje, vector<HB_CONTROL*>& hb_con, vector<TIMED_MSG*>& msg_ack, uint16_t& notsup_count, uint16_t& notsup_gen, QMap<QString,uint16_t> disp_map, MainWindow *mw){


    //Esta funcion identifica el tipo de mensaje recibido para luego darle el correcto tratamiento
    uint16_t source=mje.ID&LACAN_IDENT_MASK;
    uint16_t fun=mje.ID>>LACAN_IDENT_BITS;
    uint8_t code;

    switch(fun){
    case LACAN_FUN_DO:
        code=mje.BYTE1;
        LACAN_NOTSUP_Handler(source, notsup_count, notsup_gen,code);
       // LACAN_Acknowledge(MainWindow* mw, uint16_t requestType, uint16_t code, uint16_t result)
    break;
    case LACAN_FUN_SET:
        code=mje.BYTE1;
        LACAN_NOTSUP_Handler(source, notsup_count, notsup_gen,code);
    break;
    case LACAN_FUN_QRY:
        code=mje.BYTE1;
        LACAN_NOTSUP_Handler(source, notsup_count, notsup_gen,code);
    break;
    case LACAN_FUN_ACK:
        LACAN_ACK_Handler(mje.BYTE1, msg_ack);
    break;
    case LACAN_FUN_POST:
        LACAN_POST_Handler(source,mje.BYTE1,mje.BYTE2);
    break;
    case LACAN_FUN_ERR:
        LACAN_ERR_Handler(source,mje.BYTE1);
    break;
    case LACAN_FUN_HB:
        LACAN_HB_Handler(source, hb_con, mw);
        break;
    default:
        return LACAN_NO_SUCH_MSG;
    }
    return LACAN_SUCCESS;
}

void MainWindow::LACAN_ERR_Handler(uint16_t source,uint16_t err_cod){
    QString msg_err ="Dispositivo: ";
    msg_err = msg_err +  QString::number(source) + "\nError: " + QString::number(err_cod) ;
        QMessageBox::warning(this,"Mensaje de Error recibido",msg_err,QMessageBox::Ok);
}

void MainWindow::do_stuff(){
    static int cReconnect=0; // usamos contadores para realizar acciones periodicas con distintos intervalos mediante un solo timer
    static int cExit=0;

    if(NoUSB){
        cReconnect++;
        if(cReconnect>=6){
            cExit++;
            cReconnect=0;
            int retval = serial_port->open(QSerialPort::ReadWrite);
            if(retval){
                QMessageBox *connectionRegained= new QMessageBox();
                connectionRegained->setIcon(QMessageBox::Information);
                connectionRegained->setStandardButtons(QMessageBox::Ok);
                connectionRegained->setText("Se ha recuperado la conexion con el adaptador,"
                                   "\nYorokobe ningendomo.");
                connectionRegained->setWindowTitle("Conexion recuperada");
                connectionRegained->exec();
                cExit=0;
                NoUSB=false;
                uint8_t bdr=0x05;
                sendinit2(*serial_port,bdr);
            }else{
                if(cExit<5){
                    QMessageBox *connectionLost= new QMessageBox();
                    connectionLost->setIcon(QMessageBox::Warning);
                    connectionLost->setStandardButtons(QMessageBox::Ok);
                    connectionLost->setText("Se ha perdido la conexion con el adaptador"
                                       "\nPor favor revise el puerto USB,"
                                       "el programa intentara reconectar automaticamente "+ QString::number(5-cExit) + " veces mas.");
                    connectionLost->setWindowTitle("Error en la reconexion");
                    connectionLost->exec();
                }else{
                    QMessageBox::warning(this, "Ups",
                                                   "No se pudo reiniciar el puerto luego de 5 intentos,\n la aplicacion se cerrara, chau",
                                                   QMessageBox::Ok);
                    QCoreApplication::exit(1);
                }
            }
        }

    }else{
        LACAN_Heartbeat(this);
    }

}

void MainWindow::on_button_ESTADO_RED_2_clicked()
{
    ByteSend *bytewin = new ByteSend(this);
    bytewin->setModal(true);
    bytewin->show();
}


bool MainWindow::device_is_connected(uint8_t id){
    for(vector<HB_CONTROL*>::iterator it_hb=hb_con.begin(); it_hb < hb_con.end(); it_hb++){
        if((*it_hb)->device == id){
            return (*it_hb)->hb_status;
        }
    }
    return false;
}

void MainWindow::create_varmap_gen(){
    /* LACAN_VAR IO;
    IO.instantanea=LACAN_VAR_IO_INST;
    IO.setp=LACAN_VAR_IO_SETP;
    IO.max=LACAN_VAR_GEN_IO_MAX;
    IO.min=LACAN_VAR_GEN_IO_MIN;*/
    LACAN_VAR ISD_GEN;
    ISD_GEN.instantanea=LACAN_VAR_ISD_INST;
    ISD_GEN.setp=LACAN_VAR_ISD_SETP;
    ISD_GEN.max=LACAN_VAR_GEN_ISD_MAX;
    ISD_GEN.min=LACAN_VAR_GEN_ISD_MIN;
    LACAN_VAR IEF_GEN;
    IEF_GEN.instantanea=LACAN_VAR_IEF_INST;
    IEF_GEN.setp=LACAN_VAR_IEF_SETP;
    IEF_GEN.max=LACAN_VAR_GEN_IEF_MAX;
    IEF_GEN.min=LACAN_VAR_GEN_IEF_MIN;
    LACAN_VAR PO_GEN;
    PO_GEN.instantanea=LACAN_VAR_PO_INST;
    PO_GEN.setp=LACAN_VAR_PO_SETP;
    PO_GEN.max=LACAN_VAR_GEN_PO_MAX;
    PO_GEN.min=LACAN_VAR_GEN_PO_MIN;
    LACAN_VAR VO_GEN;
    VO_GEN.instantanea=LACAN_VAR_VO_INST;
    VO_GEN.setp=LACAN_VAR_VO_SETP;
    VO_GEN.max=LACAN_VAR_GEN_VO_MAX;
    VO_GEN.min=LACAN_VAR_GEN_VO_MIN;
    LACAN_VAR TORQ_GEN;
    TORQ_GEN.instantanea=LACAN_VAR_TORQ_INST;
    TORQ_GEN.setp=LACAN_VAR_TORQ_SETP;
    TORQ_GEN.max=LACAN_VAR_GEN_TORQ_MAX;
    TORQ_GEN.min=LACAN_VAR_GEN_TORQ_MIN;
    LACAN_VAR W_GEN;
    W_GEN.instantanea=LACAN_VAR_W_INST;
    W_GEN.setp=LACAN_VAR_W_SETP;
    W_GEN.max=LACAN_VAR_GEN_W_MAX;
    W_GEN.min=LACAN_VAR_GEN_W_MIN;
    LACAN_VAR IBAT_GEN;
    IBAT_GEN.instantanea=LACAN_VAR_I_BAT_INST;
    IBAT_GEN.setp=LACAN_VAR_I_BAT_SETP;
    IBAT_GEN.max=LACAN_VAR_GEN_IBAT_MAX;
    IBAT_GEN.min=LACAN_VAR_GEN_IBAT_MIN;
    //varmap_gen["Corriente de Salida"]=IO;
    varmap_gen["Corriente de ISD"]=ISD_GEN;
    varmap_gen["Corriente Eficaz"]=IEF_GEN;
    varmap_gen["Potencia de Salida"]=PO_GEN;
    varmap_gen["Tension de Salida"]=VO_GEN;
    varmap_gen["Torque"]=TORQ_GEN;
    varmap_gen["Velocidad Angular"]=W_GEN;
    varmap_gen["Corriente de Bateria"]=IBAT_GEN;
}

void MainWindow::create_varmap_vol(){
    LACAN_VAR W_VOL;
    W_VOL.instantanea=LACAN_VAR_W_INST;
    W_VOL.setp=LACAN_VAR_W_SETP;
    W_VOL.max=LACAN_VAR_GEN_W_MAX;
    W_VOL.min=LACAN_VAR_GEN_W_MIN;
    LACAN_VAR ID_VOL;
    ID_VOL.instantanea=LACAN_VAR_ID_INST;
    ID_VOL.setp=LACAN_VAR_ID_SETP;
    ID_VOL.max=LACAN_VAR_VOL_ID_MAX;
    ID_VOL.min=LACAN_VAR_VOL_ID_MIN;
    LACAN_VAR W_STBY_VOL;
    W_STBY_VOL.instantanea=LACAN_VAR_STANDBY_W_INST;
    W_STBY_VOL.setp=LACAN_VAR_STANDBY_W_SETP;
    W_STBY_VOL.max=LACAN_VAR_VOL_STANDBY_W_MAX;
    W_STBY_VOL.min=LACAN_VAR_VOL_STANDBY_W_MIN;
    LACAN_VAR PO_VOL;
    PO_VOL.instantanea=LACAN_VAR_PO_INST;
    PO_VOL.setp=LACAN_VAR_PO_SETP;
    //PO_VOL.max=LACAN_VAR_VOL_PO_MAX;
    //PO_VOL.min=LACAN_VAR_VOL_PO_MIN;
    LACAN_VAR VO_VOL;
    VO_VOL.instantanea=LACAN_VAR_VO_INST;
    VO_VOL.setp=LACAN_VAR_VO_SETP;
    //VO_VOL.max=LACAN_VAR_VOL_VO_MAX;
    //VO_VOL.min=LACAN_VAR_VOL_VO_MIN;
    LACAN_VAR TORQ_VOL;
    TORQ_VOL.instantanea=LACAN_VAR_TORQ_INST;
    TORQ_VOL.setp=LACAN_VAR_TORQ_SETP;
    //TORQ_VOL.max=LACAN_VAR_VOL_TORQ_MAX;
    //TORQ_VOL.min=LACAN_VAR_VOL_TORQ_MIN;
    LACAN_VAR IBAT_VOL;
    IBAT_VOL.instantanea=LACAN_VAR_I_BAT_INST;
    IBAT_VOL.setp=LACAN_VAR_I_BAT_SETP;
    //IBAT_VOL.max=LACAN_VAR_VOL_I_BAT_MAX;
    //IBAT_VOL.min=LACAN_VAR_VOL_I_BAT_MIN;

    varmap_vol["Corriente de ID"]=ID_VOL;
    varmap_vol["Velocidad Angular"]=W_VOL;
    varmap_vol["Velocidad angular Standby"]=W_STBY_VOL;
    varmap_vol["Potencia de Salida"]=PO_VOL;
    varmap_vol["Torque"]=TORQ_VOL;
    varmap_vol["Tension de Salida"]=VO_VOL;
    varmap_vol["Corriente de Bateria"]=IBAT_VOL;
}

void MainWindow::on_pushButton_clicked(bool checked)
{
    dest = LACAN_ID_GEN;
    data_can val;
    val.var_float=1.5;
    LACAN_Post(this,LACAN_VAR_STANDBY_W_SETP,val);
}


void MainWindow::handleSendTimeout(){
    static int cont=0;
    if(!stack.empty()){
        serialsend2(*serial_port,*stack.back());
        qDebug()<<"ENVIADO";
        stack.pop_back();
    }

}
