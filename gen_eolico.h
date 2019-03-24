#ifndef GEN_EOLICO_H
#define GEN_EOLICO_H

#include <QDialog>
#include "stdint.h"
#include <QTime>
#include "PC.h"
#include "comandar.h"
#include "mainwindow.h"
#include <QString>
#include <QDoubleSpinBox>

namespace Ui {
class Gen_Eolico;
}

class Gen_Eolico : public QDialog
{
    Q_OBJECT

public:
    explicit Gen_Eolico(QWidget *parent);
    ~Gen_Eolico();

protected:
    virtual void closeEvent(QCloseEvent *e) override;

private:
    Ui::Gen_Eolico *ui;
    MainWindow* mw ;
    uint16_t actual_mode;
    uint16_t previous_mode;
    uint16_t dest = LACAN_ID_GEN;
    uint16_t cmd;
    data_can val;
    QString text_val;

    QTimer *time_2sec;

    void new_mode();
    void refresh_values();
    void send_qry();

    //variables para guardar el valor a mostrar

    data_can recibed_val;

    //Variables de SetPoint
    float pot_ref;
    float speed_ref;
    float torque_ref;
    float isd_ref;
    float lim_ibat;
    float lim_ief;
    float lim_vdc;

    //Variables de Salida
    float gen_vo;
    float gen_io;
    float gen_po;
    float gen_ibat;
    float gen_vel;
    float gen_tor;




private slots:

    void timer_handler();

    void mode_changed();

    void verificar_mode_changed();

    void GENpost_Handler(LACAN_MSG msg);

    void on_pushButton_comandar_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_combo_modo_currentIndexChanged(int index);
};

#endif // GEN_EOLICO_H
