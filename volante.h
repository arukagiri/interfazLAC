#ifndef VOLANTE_H
#define VOLANTE_H

#include <QDialog>
#include "PC.h"
#include "stdint.h"
#include <QTime>
#include "comandar.h"
#include "mainwindow.h"
#include <QString>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QtCore>
#include <QtGui>
#include "lacan_limits_vol.h"

namespace Ui {
class volante;
}

class volante : public QDialog
{
    Q_OBJECT

public:
    explicit volante(QWidget *parent = 0);
    ~volante();

protected:
    virtual void closeEvent(QCloseEvent *e) override;

private:
    Ui::volante *ui;
    uint16_t actual_mode;

    MainWindow* mw ;
    uint16_t dest=LACAN_ID_VOLANTE;
    uint16_t cmd;
    data_can val;
    QString text_val; //ver si esto se usa

    QTimer *time_2sec;

    void new_mode();
    void refresh_values();
    void send_qry();

    data_can recibed_val;
    //variables para guardar el valor a mostrar en los spin box
    float id_ref;         //para insertar solo numeros
    float speed_ref;
    float standby_ref;

    //variables para mostrar en la salida (en los labels)
    float vol_vo;
    float vol_io;
    float vol_po;
    float vol_ibat;
    float vol_vel;
    float vol_tor;
    float vol_ener;

    //cuando se edita un valor
    void set_spin_click(bool state);
    bool speed_ref_click;
    bool id_ref_click;
    bool standby_click;

    void set_limits_vol();


private slots:
    void on_pushButton_start_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_comandar_clicked();

    void on_pushButton_apply_clicked();

    void on_pushButton_cancel_clicked();

    void timer_handler();

    void mode_changed();

    void VOLpost_Handler(LACAN_MSG msg);

    void enviar_variables_generales();

    void on_combo_modo_currentIndexChanged(int index);


    void on_spin_speed_ref_valueChanged(double arg1);
    void on_spin_standbay_speed_ref_valueChanged(double arg1);
    void on_spin_id_ref_valueChanged(double arg1);
};

#endif // VOLANTE_H
