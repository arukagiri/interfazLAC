#ifndef LACAN_DETECT_H
#define LACAN_DETECT_H

#include <QString>
#include <stdint.h>
#include "PC.h"
#include <QDebug>

QString detect_res(uint8_t res){
switch(res){
case LACAN_RES_OK:
    return "OK";
    break;
case LACAN_RES_MISSING_PREREQ:
    return "MISSING_PREREQ";
    break;
case LACAN_RES_RECEIVED:
    return "RECEIVED";
    break;
case LACAN_RES_NOT_IMPLEMENTED:
    return "NOT_IMPLEMENTED";
    break;
case LACAN_RES_OUT_OF_RANGE:
    return "OUT_OF_RANGE";
    break;
case LACAN_RES_BUSY:
    return "BUSY";
    break;
case LACAN_RES_DENIED:
    return "DENIED";
    break;
case LACAN_RES_GENERIC_FAILURE:
    return "GENERIC_FAILURE";
    break;
default:
    return "No especificada/soportada";
}}

QString detect_err(uint8_t err){
    qDebug()<<"ERROR DENTRO DE DETECT = "<<err;
    switch (err){
        case LACAN_ERR_GENERIC_ERR:
        return "Generic Error";
            break;
        case LACAN_ERR_OVERVOLTAGE:
        return "Over Voltage";
            break;
        case LACAN_ERR_UNDERVOLTAGE:
        return "Under Voltage";
            break;
        case LACAN_ERR_OVERCURRENT:
        return "Over Current";
            break;
        case LACAN_ERR_BAT_OVERCURRENT:
        return "Bat Over Current";
            break;
        case LACAN_ERR_OVERTEMPERATURE:
        return "Over Temperatura";
            break;
        case LACAN_ERR_OVERSPEED:
        return "Over Speed";
            break;
        case LACAN_ERR_UNDERSPEED:
        return "Under Speed";
            break;
        case LACAN_ERR_NO_HEARTBEAT:
        return "No HeartBeat";
            break;
        case LACAN_ERR_INTERNAL_TRIP:
        return "Internal Trip";
            break;
        case LACAN_ERR_EXTERNAL_TRIP:
        return "External Trip";
            break;
        default:
            return "No especificada/soportada";
        }
}

QString detect_var(uint8_t var){
       switch (var){
       case LACAN_VAR_STATUS:
           return "Vector de Estado";
           break;
       case LACAN_VAR_II_INST:
           return "Corriente de Entrada Instantanea";
           break;
       case LACAN_VAR_II_SETP:
           return "Corriente de Entrada SetPoint";
           break;
       case LACAN_VAR_IO_INST:
           return "Corriente de Salida Instantanea";
           break;
       case LACAN_VAR_IO_SETP:
           return "Corriente de Salida SetPoint";
           break;
       case LACAN_VAR_ISD_INST:
           return "Corriente de ISD Instantanea";
           break;
       case LACAN_VAR_ISD_SETP:
           return "Corriente de ISD SetPoint";
           break;
       case LACAN_VAR_IEF_INST:
           return "Corriente de Eficaz Instantanea";
           break;
       case LACAN_VAR_IEF_SETP:
           return "Corriente de Eficaz SetPoint";
           break;
       case LACAN_VAR_PI_INST:
           return "Potencia de Entrada Instantanea";
           break;
       case LACAN_VAR_PI_SETP:
           return "Potencia de Entrada SetPoint";
           break;
       case LACAN_VAR_PO_INST:
           return "Potencia de Salida Instantanea";
           break;
       case LACAN_VAR_PO_SETP:
           return "Potencia de Salida SetPoint";
           break;
       case LACAN_VAR_VI_INST:
           return "Tension de Entrada Instantenea";
           break;
       case LACAN_VAR_VI_SETP:
           return "Tension de Entrada SetPoint";
           break;
       case LACAN_VAR_VO_INST:
           return "Tension de Salida Instantenea";
           break;
       case LACAN_VAR_VO_SETP:
           return "Tension de Salida SetPoint";
           break;
       case LACAN_VAR_W_INST:
           return "Velocidad Angular Instantanea";
           break;
       case LACAN_VAR_W_SETP:
           return "Velocidad Angular SetPoint";
           break;
       case LACAN_VAR_TORQ_INST:
           return "Torque Instantaneo";
           break;
       case LACAN_VAR_TORQ_SETP:
           return "Torque SetPoint";
           break;
       case LACAN_VAR_I_BAT_INST:
           return "Corriente de Bateria Instantanea";
           break;
       case LACAN_VAR_I_BAT_SETP:
           return "Corriente de Bateria SetPoint";
           break;
       case LACAN_VAR_V_BAT_INST:
           return "Tension de Bateria Instantanea";
           break;
       case LACAN_VAR_V_BAT_SETP:
           return "Tension de Bateria SetPoint";
           break;
       case LACAN_VAR_MOD:
           return "Modo";
           break;
        default:
            return "No especificada/soportada";
        }

}


QString detect_cmd(uint8_t cmd){
    switch(cmd){
    case LACAN_CMD_TRIP:
        return "Trip";
        break;
    case LACAN_CMD_COUPLE:
        return "Couple";
        break;
    case LACAN_CMD_DECOUPLE:
        return "Decouple";
        break;
    case LACAN_CMD_MAGNETIZE:
        return "Magnetize";
        break;
    case LACAN_CMD_RESET:
        return "Reset";
        break;
    case LACAN_CMD_START:
        return "Start";
        break;
    case LACAN_CMD_STOP:
        return "Stop";
        break;
    case LACAN_CMD_MPPT_ENABLE:
        return "MPPT Enable";
        break;
    case LACAN_CMD_MPPT_DISABLE:
        return "MPPT Disable";
        break;
    default:
        return "No especificada o soportada";
    }
}


#endif // LACAN_DETECT_H
