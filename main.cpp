#include <QApplication>
#include "openport.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //Comienza la aplicacion abriendo la ventana de seleccion de puerto

    a.setAttribute(Qt::AA_EnableHighDpiScaling);

    OpenPort* op=new OpenPort();

    op->show();

    return a.exec();

}
