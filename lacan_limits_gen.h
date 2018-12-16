#ifndef LACAN_LIMITS_GEN_H
#define LACAN_LIMITS_GEN_H

//LIMITES MAXIMOS DE CADA VARIABLE DEL GENERADOR

#define LACAN_VAR_GEN_IBAT_MAX     2  //A
#define LACAN_VAR_GEN_IBAT_MIN    -3

#define LACAN_VAR_GEN_TORQ_MAX     3  //Nm
#define LACAN_VAR_GEN_TORQ_MIN     0

#define LACAN_VAR_GEN_PO_MAX  2     //kW
#define LACAN_VAR_GEN_PO_MIN  0     //kW

#define LACAN_VAR_GEN_IEF_MAX      10   //A
#define LACAN_VAR_GEN_IEF_MIN       0   //A

#define LACAN_VAR_GEN_W_MAX   1500    //rpm o 50Hz
#define LACAN_VAR_GEN_W_MIN   120     //rpm o 4Hz

#define LACAN_VAR_GEN_ISD_MAX   10  //A
#define LACAN_VAR_GEN_ISD_MIN   0   //A

#define LACAN_VAR_GEN_IO_MAX    10  //A
#define LACAN_VAR_GEN_IO_MIN    0   //A

#define LACAN_VAR_GEN_VO_MAX    300 //V
#define LACAN_VAR_GEN_VO_MIN    100 //V

#endif // LACAN_LIMITS_GEN_H
