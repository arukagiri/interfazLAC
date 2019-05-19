#include "lacan_app.h"
#include <QMessageBox>

lacan_app::lacan_app(int &argc, char **argv):QApplication(argc, argv){}

bool lacan_app::notify(QObject* receiver, QEvent* event){
    bool done = true;
    try {
        done = QApplication::notify(receiver, event);
    } catch (std::exception &e) {
        QMessageBox::warning(nullptr, "Ups", "Al parecer hubo una excepcion:\n"+QString(*(e.what())), QMessageBox::Ok);
    } catch (...) {
        QMessageBox::warning(nullptr, "Ups", "Al parecer hubo una excepcion desconocida", QMessageBox::Ok);
    }
    return done;
}
