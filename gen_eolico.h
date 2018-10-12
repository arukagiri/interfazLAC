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
    explicit Gen_Eolico(QWidget *parent = 0);
    ~Gen_Eolico();

protected:
    virtual void closeEvent(QCloseEvent *e) override;

private:
    Ui::Gen_Eolico *ui;
    uint16_t actual_mode;
    MainWindow* mw ;

    uint16_t cmd;
    data_can val;
    QString text_val;

    QTimer *time_2sec;

    void new_mode();
    void refresh_values();
    void send_qry();

    //variables para guardar el valor a mostrar

    data_can recibed_val;
    float iconv;         //para insertar solo numeros
    float isd_ref;
    float lim_ibat;
    float lim_ief;
    float lim_vdc;
    float pot_ref;
    float speed_ref;
    float torque_ref;
    float vdc;
    float ibat;

    float gen_vo;
    float gen_io;

    //cuando se edita un valor
    void set_spin_click(bool state);
    bool speed_ref_click;
    bool pot_ref_click;
    bool lim_vdc_click;
    bool vdc_click;
    bool iconv_click;
    bool lim_ief_click;
    bool torque_ref_click;
    bool isd_ref_click;
    bool lim_ibat_click;
    bool ibat_click;

    void set_limits_gen();



private slots:

    void timer_handler();

    void mode_changed();

    void GENpost_Handler(LACAN_MSG msg);

    void enviar_variables_generales();

    void on_pushButton_apply_clicked();
    void on_pushButton_cancel_clicked();
    void on_pushButton_comandar_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_spin_ibat_valueChanged(double arg1);
    void on_spin_lim_ibat_valueChanged(double arg1);
    void on_spin_lim_vdc_valueChanged(double arg1);
    void on_spin_speed_ref_valueChanged(double arg1);
    void on_spin_vdc_valueChanged(double arg1);
    void on_spin_pot_ref_valueChanged(double arg1);
    void on_spin_iconv_valueChanged(double arg1);
    void on_spin_lim_ief_valueChanged(double arg1);
    void on_spin_torque_ref_valueChanged(double arg1);
    void on_spin_isd_ref_valueChanged(double arg1);
};

#endif // GEN_EOLICO_H
