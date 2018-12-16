#include "LACAN_REC.h"
#include <QMessageBox>
#include <QDebug>



void LACAN_POST_Handler(uint16_t source,uint16_t variable, uint16_t data){
//crear archivo para cada variable e ir guardando en bloc de notas
    static uint post_cont = 0;
    post_cont++;
    qDebug()<<"Entro a la handler de POST "<<post_cont<<" veces";
}


void LACAN_ACK_Handler(uint16_t BYTE1, vector<TIMED_MSG*>& msg_ack){
    //Frente la llegada de un ack, esta funcion marca el estado de ack del mensaje correspondiente como recibido
    static uint ack_cont = 0;
    ack_cont++;
    qDebug()<<"Entro a la handler de ACK "<<ack_cont<<" veces";
    for(vector<TIMED_MSG*>::iterator it_ack=msg_ack.begin();it_ack<msg_ack.end();it_ack++){
        if((*it_ack)->msg.BYTE1==BYTE1){
            (*it_ack)->ack_status=RECEIVED;
            (*it_ack)->ack_timer.start(DEAD_MSJ_ACK_TIME);//resetea el tiempo del mensaje para borrarlo luego segun un tiempo limite
            break;
        }
    }
}

void LACAN_HB_Handler(uint16_t source, vector<HB_CONTROL*>& hb_con, MainWindow *mw){
    //Cuando llega un HB, se identifica de que dispositivo proviene y luego se procede a renovar el estado como activo y reiniciar el timer
    /*VER no haría falta el switch, con el for y el if ya estaría, solamente habría que agregar un if en el caso de dispositivo
    nuevo*/
    static uint hb_cont = 0;
    static vector<int> ignored;
    hb_cont++;
    qDebug()<<"Entro a la handler de HB "<<hb_cont<<" veces";
    bool devfound=false;
    bool stalkerfound=false;
    for(vector<HB_CONTROL*>::iterator it_hb=hb_con.begin();it_hb<hb_con.end();it_hb++){
        if((*it_hb)->device==source){
            if((*it_hb)->hb_status==INACTIVE){
                (*it_hb)->hb_status=ACTIVE;
                mw->add_device_ui(source);
            }
            (*it_hb)->hb_timer.start(DEAD_HB_TIME);
            devfound=true;
            break;
        }
    }
    for(vector<int>::iterator it_ig=ignored.begin();it_ig<ignored.end();it_ig++){
        if((*it_ig)==source){
            stalkerfound=true;
            break;
        }
    }
    if(!(devfound||stalkerfound)){
        QMessageBox *addnewdev= new QMessageBox();
        addnewdev->setIcon(QMessageBox::Question);
        addnewdev->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        addnewdev->setDefaultButton(QMessageBox::Yes);
        addnewdev->setText("Ha llegado un Heartbeat de un dispositivo"
                           " desconocido,\nDesea agregarlo a la red?\n"
                           "En caso afirmativo se le pedira que ingrese"
                           " un nombre para el mismo.");
        addnewdev->setWindowTitle("Nuevo dispositivo encontrado");
        int reply = addnewdev->exec();
        switch (reply) {
        case (QMessageBox::Yes):
        {
            mw->add_new_device(source);
            break;
        }
        case (QMessageBox::No):
        {
            ignored.push_back(source);
            break;
        }
        }
    }
}
