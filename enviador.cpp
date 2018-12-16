#include "enviador.h"
#include <QtCore>
#include <QDebug>

enviador::enviador()
{

}

void enviador::run(){
    while(1){
        this->sleep();
    }
}
