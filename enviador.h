#ifndef ENVIADOR_H
#define ENVIADOR_H

#include <QThread>


class enviador: public QThread
{
public:
    enviador();
    virtual void run() override;
};

#endif // ENVIADOR_H
