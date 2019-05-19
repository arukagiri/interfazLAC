#include <QApplication>
#include "openport.h"
#include <QMessageBox>
#include "lacan_app.h"

int main(int argc, char *argv[])
{
    lacan_app a(argc, argv);
    //Comienza la aplicacion abriendo la ventana de seleccion de puerto

    a.setAttribute(Qt::AA_EnableHighDpiScaling);

    OpenPort* op=new OpenPort();

    op->show();

    return a.exec();

}
