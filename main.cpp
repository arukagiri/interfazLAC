#include <QApplication>
#include "openport.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Comienza la aplicacion abriendo la ventana de seleccion de puerto
    OpenPort* op=new OpenPort();

    op->show();

    return a.exec();
}
