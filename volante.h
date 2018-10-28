#ifndef VOLANTE_H
#define VOLANTE_H

#include <QDialog>
#include "PC.h"

namespace Ui {
class volante;
}

class volante : public QDialog
{
    Q_OBJECT

public:
    explicit volante(QWidget *parent = 0);
    ~volante();

private:
    Ui::volante *ui;
    uint16_t actual_mode;
};

#endif // VOLANTE_H
