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

    void on_button_gen_clicked();

private:
    Ui::EstadoRed *ui;

    float gen_v;
    float gen_i;
    float bust_v;
    float bust_i;
    float vol_v;
    float vol_i;
    void refresh_values(void);
    void update_values(void);
};

#endif // ESTADORED_H
