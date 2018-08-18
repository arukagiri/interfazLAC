#ifndef TIEMPO_H
#define TIEMPO_H

#if defined (_WIN32) || defined( _WIN64)
#include <windows.h>        //para el timer
#include <iomanip>          //para el timer
#endif

class tiempo{
    LARGE_INTEGER init,ende,freq;
    public:
        tiempo();
        void start();
        int getime();
        int getfreq();
        ~tiempo(){}
};

#endif // TIME_H
