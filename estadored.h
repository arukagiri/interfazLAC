#ifndef ESTADORED_H
#define ESTADORED_H

#include <QDialog>

namespace Ui {
class EstadoRed;
}

class EstadoRed : public QDialog
{
    Q_OBJECT

public:
    explicit EstadoRed(QWidget *parent = 0);
    ~EstadoRed();

private slots:
    void on_PC_clicked();
    //void on_GEN_clicked();
    //void on_BOOST_clicked();
    //void on_VOL_clicked();


private:
    Ui::EstadoRed *ui;
};

#endif // ESTADORED_H
