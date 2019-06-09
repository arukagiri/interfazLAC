/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Agustin Baffo, Lucas Sandoz. Todos los derechos reservados.
 *  Licenciado bajo BSD de 3 clausulas. Para mas informacion ver el archivo LICENCE.md dentro
 *  del repositorio oficial del proyecto LACAN. https://github.com/AraragiRukasu/interfazLAC
 *--------------------------------------------------------------------------------------------*/

#include <QApplication>
#include "openport.h"
#include <QMessageBox>
#include "lacan_app.h"

/*--------------------------------------------------------------------------------------------
 * Frente a cualquier cambio de constantes fuera del programa (Ej BAUD del CAN) asegurese
 * de corroborar que no hay ninguna constante en el programa que corresponda a ese valor, debido
 * a que a veces el programa necesita saber valores externos para calcular otros utilizados internamente.
 *--------------------------------------------------------------------------------------------*/

int main(int argc, char *argv[])
{
    lacan_app a(argc, argv);
    //Comienza la aplicacion abriendo la ventana de seleccion de puerto

    a.setAttribute(Qt::AA_EnableHighDpiScaling);

    OpenPort* op=new OpenPort();
    op->setAttribute(Qt::WA_DeleteOnClose);
    op->show();

    return a.exec();
}
