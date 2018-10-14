#ifndef VOLANTE_H
#define VOLANTE_H

#include <QDialog>

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
};

#endif // VOLANTE_H
