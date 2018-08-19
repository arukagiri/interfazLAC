#ifndef ESTADO_RED_H
#define ESTADO_RED_H

#include <QDialog>

namespace Ui {
class Estado_Red;
}

class Estado_Red : public QDialog
{
    Q_OBJECT

public:
    explicit Estado_Red(QWidget *parent = 0);
    ~Estado_Red();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Estado_Red *ui;
};

#endif // ESTADO_RED_H
