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
    bool referenceChanged;
    bool send_queries;
    QTimer *time_2sec;
    int refValue = -5; // un valor que sea menor que cualquiera de los limites


    void refresh_mode();
    void refresh_values();
    void send_qry();
    void send_qry_variables();
    void send_qry_references();
    void processEditingFinished(QDoubleSpinBox* spin, uint16_t var);
    void blockAllSpinSignals(bool b);
    //variables para guardar el valor a mostrar

    data_can recibed_val;

    //Variables de SetPoint
    float pot_ref = refValue;
    float speed_ref = refValue;
    float torque_ref = refValue;
    float isd_ref = refValue;
    float lim_ibat = refValue;
    float lim_ief = refValue;
    float lim_vdc = refValue;

    //Variables de Salida
    float gen_vo = refValue;
    float gen_io = refValue;
    float gen_po = refValue;
    float gen_ibat = refValue;
    float gen_vel = refValue;
    float gen_tor = refValue;
    bool state = false;

signals:
    void genWindowsClosed();
public slots:
    void focusReturned();
private slots:

    void timer_handler();

    void verificar_mode_changed();

    void GENpost_Handler(LACAN_MSG msg);

    void on_pushButton_comandar_clicked();

    void on_pushButton_start_clicked();

    void on_pushButton_stop_clicked();

    void on_combo_modo_currentIndexChanged(int index);

    void on_spin_gen_speed_ref_editingFinished();

    void on_spin_gen_pot_ref_editingFinished();

    void on_spin_gen_torque_ref_editingFinished();

    void on_spin_gen_lim_ief_ref_editingFinished();

    void on_spin_gen_isd_ref_editingFinished();

    void on_spin_gen_lim_ibat_ref_editingFinished();

    void on_spin_gen_lim_vdc_ref_editingFinished();

    void on_edit_checkBox_stateChanged(int checked);

    void changeEditState();

};

#endif // GEN_EOLICO_H
