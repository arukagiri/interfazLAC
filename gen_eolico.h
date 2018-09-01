#ifndef GEN_EOLICO_H
#define GEN_EOLICO_H

#include <QDialog>
#include "stdint.h"
#include <QTime>
#include "PC.h"

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

    void new_mode();
    void refresh_values();

    int16_t iconv;         //para insertar solo numeros
    int16_t isd_ref;
    int16_t lim_ibat;
    int16_t lim_ief;
    int16_t lim_vdc;
    int16_t pot_ref;
    int16_t speed_ref;
    int16_t torque_ref;
    int16_t vdc;
    int16_t ibat;

private slots:

    void timer_handler();

    void mode_changed();

    void GENpost_Handler(LACAN_MSG msg);

};

#endif // GEN_EOLICO_H
