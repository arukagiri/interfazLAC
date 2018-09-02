#include "LACAN_REC.h"
#include <QMessageBox>
#include <QDebug>

int LACAN_Msg_Handler(LACAN_MSG &mje, vector<HB_CONTROL*>& hb_con, vector<TIMED_MSG*>& msg_ack, uint16_t& notsup_count, uint16_t& notsup_gen, QMap<QString,uint16_t> disp_map){

    //Esta funcion identifica el tipo de mensaje recibido para luego darle el correcto tratamiento
    uint16_t source=mje.ID&LACAN_IDENT_MASK;
    uint16_t fun=mje.ID>>LACAN_IDENT_BITS;

    switch(fun){
	case LACAN_FUN_DO:
        LACAN_NOTSUP_Handler(source, notsup_count, notsup_gen);
	break;
	case LACAN_FUN_SET:
        LACAN_NOTSUP_Handler(source, notsup_count, notsup_gen);
	break;
    case LACAN_FUN_QRY:
        LACAN_NOTSUP_Handler(source, notsup_count, notsup_gen);
	break;
	case LACAN_FUN_ACK:
        LACAN_ACK_Handler(mje.BYTE1, msg_ack);
	break;
    case LACAN_FUN_POST:
        LACAN_POST_Handler(source,mje.BYTE1,mje.BYTE2);
	break;
	case LACAN_FUN_ERR:
	//	return LACAN_ERR_Handler(source,LACAN_queue[queueIndex].BYTE1);
	break;
	case LACAN_FUN_HB:
        LACAN_HB_Handler(source, hb_con, disp_map);
        break;
	default:
		return LACAN_NO_SUCH_MSG;
	}
	return LACAN_SUCCESS;
}





void LACAN_POST_Handler(uint16_t source,uint16_t variable, uint16_t data){
//crear archivo para cada variable e ir guardando en bloc de notas
}


void LACAN_ACK_Handler(uint16_t BYTE1, vector<TIMED_MSG*>& msg_ack){
    //Frente la llegada de un ack, esta funcion marca el estado de ack del mensaje correspondiente como recibido
    for(vector<TIMED_MSG*>::iterator it_ack=msg_ack.begin();it_ack<msg_ack.end();it_ack++){
        if((*it_ack)->msg.BYTE1==BYTE1){
            (*it_ack)->ack_status=RECEIVED;
            (*it_ack)->ack_timer.start(DEAD_MSJ_ACK_TIME);//resetea el tiempo del mensaje para borrarlo luego segun un tiempo limite
            break;
        }
    }
}


void LACAN_HB_Handler(uint16_t source, vector<HB_CONTROL*>& hb_con, QMap<QString,uint16_t> disp_map){
    //Cuando llega un HB, se identifica de que dispositivo proviene y luego se procede a renovar el estado como activo y reiniciar el timer
    /*VER no haría falta el switch, con el for y el if ya estaría, solamente habría que agregar un if en el caso de dispositivo
    nuevo*/
    static vector<int> ignored;
    bool devfound=false;
    bool stalkerfound=false;
    for(vector<HB_CONTROL*>::iterator it_hb=hb_con.begin();it_hb<hb_con.end();it_hb++){
        if((*it_hb)->device==source){
            (*it_hb)->hb_status=ACTIVE;
            (*it_hb)->hb_timer.start(DEAD_HB_TIME);
            devfound=true;
            break;
        }
    }
    for(vector<int>::iterator it_ig=ignored.begin();it_ig<ignored.end();it_ig++){
        if((*it_ig)==source){
            stalkerfound=true;
        }
    }
    if(!(devfound&&stalkerfound)){
        QMessageBox::StandardButton reply = QMessageBox::question(0,
                                       "Nuevo dispositivo encontrado", "Ha llegado un Heartbeat de un dispositivo"
                                                                       "desconocido,¿Desea agregarlo a la red?\n"
                                                                       "En caso afirmativo se le pedira que ingrese"
                                                                       "un nombre para el mismo",
                              QMessageBox::Yes | QMessageBox::No );
        switch (reply) {
        case (QMessageBox::Yes):
        {
            qDebug()<<"\nAgregando nuevo dispositivo\n";//Evaluar la posibildad de q llegue un heartbeat de un dispositivo que no existe (corrupcion del source durante el envio)
            HB_CONTROL newdev;
            QString newdev_name;
            newdev.device=source;
            newdev.hb_timer.start(DEAD_HB_TIME);
            newdev.hb_status=ACTIVE;
            hb_con.push_back(&newdev);
            newdev_name = "Name";// AGREGAR VENTANA QUE PREGUNTE CUAL VA A SER EL NOMBRE
            if(!disp_map.contains(newdev_name)){
                disp_map[newdev_name]=source;
            }
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
