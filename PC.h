#include <iostream>
#include <QTimer>
#include <QString>
#include <stdint.h>
#include <QMap>

#ifndef PC_H_INCLUDED
#define PC_H_INCLUDED

#define LACAN_LOCAL_ID 0x01 //ID de dispositivo, en este caso de la PC
#define LACAN_FUNCTION_BITS 6 //cantidad de bits de funci n dentro del campo MSG_ID de CAN standard(11 bits en total)
#define LACAN_IDENT_BITS 5 //cantidad de bits de ID del nodo de la red dentro del campo MSG_ID de CAN standard(11 bits en total)
#define LACAN_IDENT_MASK 0x1f //mascara para extraer la ID del nodo dentro de la ID del mensaje CAN
#define LACAN_FUN_MASK (~LACAN_IDENT_MASK) //mascara para extraer la funcion del mensaje
#if (LACAN_IDENT_BITS + LACAN_FUNCTION_BITS != 11) //verificacion de la cantidad de bits en MSG_ID
    #error LACAN_IDENT_BITS + LACAN_FUNCTION_BITS != 11
#endif
#define LACAN_ID_STANDARD_MASK 0x7FF //mascara de 11 bits para poder extraer la MSG_ID
#define LACAN_BYTE0_RESERVED (8-(LACAN_IDENT_BITS)) //bits reservados dentro de un byte del mensaje donde tambien se manda la ID de dispositivo
#define LACAN_BYTE0_RESERVED_MASK (0x3) //mascara para extraer el campo reservado del byte0

// Diccionario de direcciones (ID). 5 bits (0x00 a 0x1f)
#define LACAN_ID_BROADCAST      0x00
#define LACAN_ID_MASTER         0x01
#define LACAN_ID_GEN            0x02
#define LACAN_ID_BOOST          0x03
#define LACAN_ID_VOLANTE        0x04

// Diccionario de funciones  (FUN). 6 bits (0x00 a 0x3f)
#define LACAN_FUN_ERR           0x01
#define LACAN_FUN_DO            0x04
#define LACAN_FUN_SET           0x08
#define LACAN_FUN_QRY           0x0C
#define LACAN_FUN_ACK           0x10
#define LACAN_FUN_POST          0x14
#define LACAN_FUN_HB            0x18

// Diccionario de comandos (CMD). 8 bits (0x00 a 0xff)
#define LACAN_CMD_TRIP          0x08
#define LACAN_CMD_STOP          0x0F //DESHABILITAR SALIDA
#define LACAN_CMD_RESET         0x10
#define LACAN_CMD_COUPLE        0x18
#define LACAN_CMD_START         0x1F // HABILITAR SALIDA
#define LACAN_CMD_DECOUPLE      0x28
#define LACAN_CMD_MAGNETIZE     0x2F
#define LACAN_CMD_MPPT_ENABLE   0x38
#define LACAN_CMD_MPPT_DISABLE  0x3F

// Diccionario de variables (VAR). 8 bits (0x00 a 0xff)
#define LACAN_VAR_STATUS        0x02
#define LACAN_VAR_II_MAX        0x04
#define LACAN_VAR_II_MIN        0x05
#define LACAN_VAR_II            0x06
#define LACAN_VAR_II_SETP       0x07
#define LACAN_VAR_IO_MAX        0x0A
#define LACAN_VAR_IO_MIN        0x0B
#define LACAN_VAR_IO            0x0C
#define LACAN_VAR_IO_SETP       0x0D
#define LACAN_VAR_ISD_MAX       0x14
#define LACAN_VAR_ISD_MIN       0x15
#define LACAN_VAR_ISD           0x16
#define LACAN_VAR_ISD_SETP      0x17
#define LACAN_VAR_IEF_MAX       0x1A
#define LACAN_VAR_IEF_MIN       0x1B
#define LACAN_VAR_IEF           0x1C
#define LACAN_VAR_IEF_SETP      0x1D
#define LACAN_VAR_PI_MAX        0x24
#define LACAN_VAR_PI_MIN        0x25
#define LACAN_VAR_PI            0x26
#define LACAN_VAR_PI_SETP       0x27
#define LACAN_VAR_PO_MAX        0x2A
#define LACAN_VAR_PO_MIN        0x2B
#define LACAN_VAR_PO            0x2C
#define LACAN_VAR_PO_SETP       0x2D
#define LACAN_VAR_VI_MAX        0x34
#define LACAN_VAR_VI_MIN        0x35
#define LACAN_VAR_VI            0x36
#define LACAN_VAR_VI_SETP       0x37
#define LACAN_VAR_VO_MAX        0x3A
#define LACAN_VAR_VO_MIN        0x3B
#define LACAN_VAR_VO            0x3C
#define LACAN_VAR_VO_SETP       0x3D
#define LACAN_VAR_W_MAX         0x44
#define LACAN_VAR_W_MIN         0x45
#define LACAN_VAR_W             0x46
#define LACAN_VAR_W_SETP        0x47
#define LACAN_VAR_BAT_IMAX      0x54
#define LACAN_VAR_BAT_IMIN      0x55
#define LACAN_VAR_BAT_I         0x56
#define LACAN_VAR_BAT_I_SETP    0x57
#define LACAN_VAR_BAT_VMAX      0x5A
#define LACAN_VAR_BAT_VMIN      0x5B
#define LACAN_VAR_BAT_V         0x5C
#define LACAN_VAR_BAT_V_SETP    0x5D
#define LACAN_VAR_MOD_POT       0x64 // ver si lo usamos al final
#define LACAN_VAR_MOD_VEL       0x65
#define LACAN_VAR_MOD_TORQ      0x66
#define LACAN_VAR_MOD_MPPT      0x67

#define LACAN_VAR_TORQI_MAX     0x0
#define LACAN_VAR_TORQI_MIN       0x0
#define LACAN_VAR_TORQI            0x0
#define LACAN_VAR_TORQI_SETP       0x0
#define LACAN_VAR_TORQO_MAX        0x0
#define LACAN_VAR_TORQO_MIN         0x0
#define LACAN_VAR_TORQO             0x0
#define LACAN_VAR_TORQO_SETP        0x0


// Diccionario de resultados (RES). 8 bits (0x00 a 0xff)
#define LACAN_RES_OK                0x00
#define LACAN_RES_MISSING_PREREQ    0x01
#define LACAN_RES_RECEIVED          0x02
#define LACAN_RES_NOT_IMPLEMENTED   0x08
#define LACAN_RES_OUT_OF_RANGE      0x10
#define LACAN_RES_BUSY              0x18
#define LACAN_RES_DENIED            0x20
#define LACAN_RES_GENERIC_FAILURE   0x28

// Diccionario de errores (ERR). 8 bits (0x00 a 0xff)
#define LACAN_ERR_GENERIC_ERR       0x00
#define LACAN_ERR_OVERVOLTAGE       0x08
#define LACAN_ERR_UNDERVOLTAGE      0x10
#define LACAN_ERR_OVERCURRENT       0x18
#define LACAN_ERR_BAT_OVERCURRENT   0x19
#define LACAN_ERR_OVERTEMPERATURE   0x20
#define LACAN_ERR_OVERSPEED         0x28
#define LACAN_ERR_UNDERSPEED        0x30
#define LACAN_ERR_NO_HEARTBEAT      0x38
#define LACAN_ERR_INTERNAL_TRIP     0x39
#define LACAN_ERR_EXTERNAL_TRIP     0x40

// Diccionario de tipos de solicitudes (REQ). 3 bits (0x0 a 0x7)
#define LACAN_REQ_SET           0x01
#define LACAN_REQ_DO            0x02
#define LACAN_REQ_QRY           0x03


// C digos de respuesta del protocolo
#define LACAN_SUCCESS           0
#define LACAN_FAILURE           -1
#define LACAN_NO_SUCH_MSG       -2  //no usados en la PC, pero se debe tener en cuenta que estan reservados en los DSP
#define LACAN_NO_BOXES          -3

//Estados de acknowledge del mensaje TIMED_MSG (campo status de la struct)
#define PENDACK         0
#define RECEIVED        1
#define ACK_TIMEOUT     2

//Estados de los dispositivos segun heartbeat

#define ACTIVE      true
#define INACTIVE    false


//Mascaras y corrimientos
#define DLC_MASK            15//mascara para extraer el dlc del segundo byte del mensaje
#define BOTTOM_FUN_MASK     0xE0//mascaras y corrimientos para poder armar el campo de funcion a partir de lo que llega (que esta el reves)
#define UPPER_FUN_MASK      0x07
#define FUN_MOV_BOTTOM      5
#define FUN_MOV_UPPER       3
#define FUN_MOV_FORSOURCE   5//movimiento para poder armar la ID con la source y la fun


#define HB_TIME 3000                 //en milisegundos(5 seg), es el periodo en el cual los integrantes de la red deben enviar sus HB
#define DEAD_HB_TIME HB_TIME*2+500   //tiempo que debe transcurrir desde el ultimo HB para considerar un nodo inactivo (10.5 seg)
#define DEAD_MSJ_ACK_TIME 30000      //tiempo para borrar del vector un mensaje desde que recibio su correspondiente ack
#define WAIT_ACK_TIME 500            //tiempo de espera un ack

//union apta para manejar un vector que contiene el estado del dispositivo, es decir,
//que variables esta tomando como referencia para realizar el control (de esta manera se puede
//deducir indirectamente el modo del mismo)
union LACAN_32b_DATA{
    uint32_t uint_32b;
    int32_t int_32b;
    float float_32b;
};

union data_can{
    float    var_float;
    char     var_char[4];
    uint32_t var_int;
};

//Estructura a base de bits para armar el mensaje con el formato CAN standard
struct LACAN_MSG{
   uint16_t SENTIDO:1;
   uint16_t DLC:4;
   uint16_t ID:11;
   uint16_t BYTE0:8;
   uint16_t BYTE1:8;
   uint16_t BYTE2:8;
   uint16_t BYTE3:8;
   uint16_t BYTE4:8;
   uint16_t BYTE5:8;
   uint16_t BYTE6:8;
   uint16_t BYTE7:8;
};

typedef struct LACAN_MSG LACAN_MSG;

//Estructura armada para poder manejar de manera conjunta el manejo de los mensajes que esperan un ack, est
//provista del mensaje propiamente dicho, un timer para controlar los tiempos caracteristicos del ack(ver LACAN_PRO.h)
//y un campo de estado, las posibilidades del mismo se encuentran mas arriba como "Estados de acknowledge del mensaje TIMED_MSG"
struct  TIMED_MSG{
        LACAN_MSG msg;
        QTimer ack_timer;
        uint8_t ack_status;
};


typedef struct TIMED_MSG TIMED_MSG;


//test
struct  TIMED_MSG2{
        LACAN_MSG msg;
        QTimer* ack_timer;
        uint8_t ack_status;
};


typedef struct TIMED_MSG TIMED_MSG;

//Se procede con un una idea parecida al TIMED_MSG para controlar el estado de los nodos en la red, en este caso tenemos un
//identificador de dispositivo en vez de un mensaje, el temporizador(ver LACAN_PRO.h) y el campo de estado(m s en
//"Estados de los dispositivos segun heartbeat" arriba) cumplen las mismas funciones
struct  HB_CONTROL{
        QTimer hb_timer;
        bool hb_status;
        uint16_t device;
};

typedef struct HB_CONTROL HB_CONTROL;

//QString fun, com, dest, var_type, var_val, code;
struct ABSTRACTED_MSG{

    QString fun, com, dest, var_type, var_val, err_code, ack_res, ack_code,curr_time;

};

struct variable {
  QString tipo;
  uint16_t id;
};

#endif // PC_H_INCLUDED


