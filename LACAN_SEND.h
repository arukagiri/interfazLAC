#ifndef LACAN_SEND_H_INCLUDED
#define LACAN_SEND_H_INCLUDED
#include <vector>
#include "PC.h"
#include <iostream>
#include <stdint.h>
#include "better_serialfunc.h"
#include "mainwindow.h"

using namespace std;

int16_t LACAN_Set(MainWindow *mw, uint16_t variable, uint16_t data);
int16_t LACAN_Do(MainWindow *mw, uint16_t cmd);
int16_t LACAN_Post(MainWindow *mw, uint16_t variable, uint16_t data);
int16_t LACAN_Query(MainWindow *mw, uint16_t variable);
int16_t LACAN_Heartbeat(MainWindow *mw);
int16_t LACAN_Error(MainWindow* mw, uint16_t errCode);
int16_t LACAN_Acknowledge(MainWindow* mw, uint16_t requestType, uint16_t object, uint16_t result);


#endif // LACAN_SEND_H_INCLUDED
