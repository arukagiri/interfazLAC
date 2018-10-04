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
    float val_float;
    data_can val_union;

    qDebug()<<"ENTRO A ABSTRACT MSG";
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
        abs_msg.ack_code=QString::number(msg_log.back().BYTE1);
        abs_msg.var_type=detect_var(msg_log.back().BYTE2);

        val_union.var_char[0]=char(msg_log.back().BYTE3);
        val_union.var_char[1]=char(msg_log.back().BYTE4);
        val_union.var_char[2]=char(msg_log.back().BYTE5);
        val_union.var_char[3]=char(msg_log.back().BYTE6);

        val_float = val_union.var_float;
        abs_msg.var_val=QString::number(double(val_float));

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

        val_union.var_char[0]=char(msg_log.back().BYTE2);
        val_union.var_char[1]=char(msg_log.back().BYTE3);
        val_union.var_char[2]=char(msg_log.back().BYTE4);
        val_union.var_char[3]=char(msg_log.back().BYTE5);

        val_float = val_union.var_float;
        abs_msg.var_val=QString::number(double(val_float));

        break;

    case LACAN_FUN_HB:
        abs_msg.fun="Heartbeat";
        break;

    default:
        abs_msg.fun="No especificada/soportada";
    }
    return abs_msg;
}

/*MainWindow::init_varmap(){

varmap["Corriente de Salida Instantanea"].id=LACAN_VAR_IO;
varmap["Tension"].tipo="int";
varmap["Vector de Estados"].id=LACAN_VAR_STATUS;
varmap["Vector de Estados"].tipo="Nada";
varmap[.id=LACAN_VAR_II_MAX;
].id=LACAN_VAR_II_MIN ;
varmap["Corriente de Entrada Instantanea"].id=LACAN_VAR_II;
varmap["Corriente de Entrada Instantanea"].tipo="float";  ;
].id=LACAN_VAR_II_SETP;
].id=LACAN_VAR_IO_MAX ;
].id=LACAN_VAR_IO_MIN ;
varmap["Corriente de Salida Instantanea"].id=LACAN_VAR_IO;
varmap["Corriente de Salida Instantanea"].tipo="float";
].id=LACAN_VAR_IO_SETP;
].id=LACAN_VAR_ISD_MAX;
].id=LACAN_VAR_ISD_MIN;
].id=LACAN_VAR_ISD    ;
].id=LACAN_VAR_ISD_SETP;
].id=LACAN_VAR_IEF_MAX ;
].id=LACAN_VAR_IEF_MIN ;
].id=LACAN_VAR_IEF     ;
].id=LACAN_VAR_IEF_SETP;
].id=LACAN_VAR_PI_MAX  ;
].id=LACAN_VAR_PI_MIN  ;
].id=LACAN_VAR_PI      ;
].id=LACAN_VAR_PI_SETP ;
].id=LACAN_VAR_PO_MAX  ;
].id=LACAN_VAR_PO_MIN  ;
].id=LACAN_VAR_PO      ;
].id=LACAN_VAR_PO_SETP ;
].id=LACAN_VAR_VI_MAX  ;
].id=LACAN_VAR_VI_MIN  ;
].id=LACAN_VAR_VI      ;
].id=LACAN_VAR_VI_SETP ;
].id=LACAN_VAR_VO_MAX  ;
].id=LACAN_VAR_VO_MIN  ;
varmap["Tension de Salida Instantanea"].id=LACAN_VAR_VO;
varmap["Tension de Salida Instantanea"].tipo="float";
].id=LACAN_VAR_VO_SETP ;
].id=LACAN_VAR_W_MAX   ;
].id=LACAN_VAR_W_MIN   ;
].id=LACAN_VAR_W       ;
].id=LACAN_VAR_W_SETP  ;
].id=LACAN_VAR_BAT_IMAX;
].id=LACAN_VAR_BAT_IMIN;
\].id=LACAN_VAR_BAT_I  ;
].id=LACAN_VAR_BAT_I_SETP;
].id=LACAN_VAR_BAT_VMAX  ;
].id=LACAN_VAR_BAT_VMIN  ;
].id=LACAN_VAR_BAT_V     ;
].id=LACAN_VAR_BAT_V_SETP;
].id=LACAN_VAR_MOD_POT   ;
].id=LACAN_VAR_MOD_VEL  ;
].id=LACAN_VAR_MOD_TORQ ;
}*/

MainWindow::MainWindow(QSerialPort &serial_port0,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial_port=&serial_port0;
    do_log=false;
    ERflag= false;
    dest=LACAN_ID_BROADCAST;
    outlog_cont=0;
    inlog_cont=0;


    ui->the_one_true_list_DESTINO->addItem("Broadcast");
    ui->the_one_true_list_DESTINO->addItem("Generador Eolico");
    ui->the_one_true_list_DESTINO->addItem("Volante de Inercia");
    ui->the_one_true_list_DESTINO->addItem("Boost");

    disp_map["Broadcast"]=LACAN_ID_BROADCAST;
    disp_map["Generador Eolico"]=LACAN_ID_GEN;
    disp_map["Volante de Inercia"]=LACAN_ID_VOLANTE;
    disp_map["Boost"]=LACAN_ID_BOOST;

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

void MainWindow::verificar_destino(){
    dest=disp_map[(ui->the_one_true_list_DESTINO->currentItem()->text())];
}


void MainWindow::on_button_COMANDAR_clicked()
{

    verificar_destino();
    Comandar *comwin = new Comandar(this);

    comwin->setModal(true);
    comwin->show();
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

    //do_log=FALSE;
    this->dest=LACAN_ID_GEN;
    data_can datos;
    //datos.var_float = 1.5;
     datos.var_int = 12e5;

    LACAN_Post(this,LACAN_VAR_IO,datos);
    this->agregar_log_sent();
   /* LACAN_Heartbeat(this);
    this->agregar_log_sent();*/

    LACAN_HB_Handler(7,hb_con,this);
}

void MainWindow::verificarHB(){
    //Encargada de verificar que todos los dispositivos de la red esten activos mediante el HB,
    //cada nodo debe enviar HB cada un cierto tiempo(HB_TIME), si este no se recibe dentro de un periodo de
    //tolerancia (DEAD_HB_TIME), esta funcion se encarga de que el programa lo considere inactivo
    for(vector<HB_CONTROL*>::iterator it_hb=hb_con.begin(); it_hb < hb_con.end(); it_hb++){
        if(((*it_hb)->hb_timer.remainingTime()<= 0) && ((*it_hb)->hb_status==ACTIVE)){
            (*it_hb)->hb_status=INACTIVE;
            cout<<"\nMurio un dispositivo :'( \n";
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
                qDebug()<<"entro al if RECEIVED";
                msg_ack.erase(it_ack);                  //si hace mucho que se mando el mensaje y no se hizo nada lo borramos
            }
        }
        else{
             if(!((*it_ack)->ack_timer.isActive())){  //si no llego el ack..
                (*it_ack)->ack_status=ACK_TIMEOUT;
                qDebug()<<"entro al if timeout";
                if((*it_ack)->retries<=0){  // si no quedan reintentos
                    qDebug()<<"entro al tosio";
                    disconnect(&(msg_ack.back()->ack_timer),SIGNAL(timeout()), this, SLOT(verificarACK()));
                    no_ACK_Handler(); //ver de eliminar el msg despues de procesar esta funcion, o dentro de la misma
                    msg_ack.erase(it_ack);
                }
                else{   //si quedan reintentos, vuelve a enviar el mensaje y descuenta reintentos

                    qDebug()<<"entro al reenviar";
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
    bool newmsgflag=0;
    static char pila[100]={0};
    newmsgflag=readport2(pila, *serial_port);
    static uint16_t notsup_count, notsup_gen;
    if(newmsgflag){
        LACAN_MSG msg;
        int result=0;
        uint prevsize=0;

        msg=mensaje_recibido2(pila);
        msg_log.push_back(msg);
        prevsize = hb_con.size();

        if((msg.ID>>LACAN_IDENT_BITS==LACAN_FUN_POST)&&ERflag){
            emit postforER_arrived(msg);
        }else{
            result=LACAN_Msg_Handler(msg,hb_con,msg_ack,notsup_count,notsup_gen,disp_map,this);
            //VER A partir de mensajes recibidos solo podria aumentar el numero de dispositivos conectados, no de msj con ACK
            if(hb_con.size()>prevsize){
                connect(&(hb_con.back()->hb_timer),SIGNAL(timeout()),this,SLOT(verificarHB()));//VER sin & no compila, ver si anda asi
            }
        }
        this->agregar_log_rec(msg_log);
    }
    qDebug()<<pila;
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
    hb_con.push_back(&newdev);

    AddNewDevDialog *diag=new AddNewDevDialog(this);
    diag->setModal(true);
    connect(diag, SIGNAL(dev_name_set(QString)), this, SLOT(add_dev_name(QString)));
    diag->show();

}

void MainWindow::add_dev_name(QString newdevname){
    if(!disp_map.contains(newdevname)){
        disp_map[newdevname]=newdev.device;
    }
    ui->the_one_true_list_DESTINO->addItem(newdevname);
}


