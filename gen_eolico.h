#ifndef GEN_EOLICO_H
#define GEN_EOLICO_H

#include <QDialog>
#include "stdint.h"
#include <QTime>

namespace Ui {
class Gen_Eolico;
}

class Gen_Eolico : public QDialog
{
    Q_OBJECT

public:
    explicit Gen_Eolico(QWidget *parent = 0);
    ~Gen_Eolico();

private:
    Ui::Gen_Eolico *ui;
    uint16_t actual_mode;

    QTimer *time_2sec;

    void refresh_values();

private slots:

    void timer_handler();

    void mode_changed();
    void new_mode();

};

#endif // GEN_EOLICO_H
