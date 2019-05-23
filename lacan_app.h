#ifndef LACAN_APP_H
#define LACAN_APP_H
#include <QApplication>

class lacan_app:public QApplication
{
public:
    lacan_app(int& argc, char** argv);
    virtual bool notify(QObject* receiver, QEvent* event) override;

};

#endif // LACAN_APP_H
