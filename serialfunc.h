#ifndef SERIALFUNC_H_INCLUDED
#define SERIALFUNC_H_INCLUDED
#include "serialib.h"

#include "PC.h"


#if defined (_WIN32) || defined( _WIN64)
#define         DEVICE_PORT             "COM1"                               // COM1 for windows

#endif

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyS0"                         // ttyS0 for linux
#endif


void serialsend(serialib &LS, LACAN_MSG msg);
void sendinit(serialib &LS,int bdr);
void openport(serialib &LS, int bdr,char *pila, uint16_t &index_pila);
LACAN_MSG mensaje_recibido(char *pila);
void readport(char *pila, uint16_t &index_pila,uint16_t &dlc,serialib &LS);


#endif // SERIALFUNC_H_INCLUDED
