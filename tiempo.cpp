#include "tiempo.h"
#include <stdint.h>
//Clase de timer, se le pasa una frecuencia al timer en el constructor por defecto, luego con start comienza a correr
//el timer, puediendose resetear con la misma funcion. La funcion getime calcula el tiempo segun la cantidad de pulsos de
//reloj pasaron desde el "start" sabiendo la frecuencia  del mismo, se ajusta para que la funcion devuelva el tiempo en
//microsegundos.
/*VER de implementar el destructor*/
tiempo::tiempo(){
    QueryPerformanceFrequency(&freq);
}
void tiempo::start(){
    QueryPerformanceCounter(&init);
}
int tiempo::getime(){
    uint64_t preres,frequency;
    int res;
    QueryPerformanceCounter(&ende);
    preres=(ende.QuadPart - init.QuadPart) * 1000000;
    res=preres/freq.QuadPart;
    return res;
}
int tiempo::getfreq(){
    return freq.QuadPart;
}
