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
    explicit volante(QWidget *parent);
    ~volante();

protected:
    virtual void closeEvent(QCloseEvent *e) override;

private:
    Ui::volante *ui;
    MainWindow* mw ;

    uint16_t dest=LACAN_ID_VOLANTE;

    data_can recibed_val;
    //Variables de Entrada
    float id_ref;
    float speed_ref;
    float standby_ref;

    //Variables de Salida
    float vol_vo;
    float vol_io;
    float vol_po;
    float vol_ibat;
    float vol_vel;
    float vol_tor;
    float vol_ener;
    bool send_queries;
    bool referenceChanged;

    uint16_t actual_mode;
    uint16_t previous_mode;
    uint16_t cmd;
    data_can val;
    QString text_val;

    QTimer *time_2sec;

    void new_mode();
    void refresh_values();
    void send_qry();
    void send_qry_variables();
    void send_qry_references();
    void processEditingFinished(QDoubleSpinBox* spin, uint16_t var);
    void blockAllSpinSignals(bool b);

signals:
    void volWindowsClosed();

private slots:

    void timer_handler();

    void mode_changed();

    void verificar_mode_changed();

    void VOLpost_Handler(LACAN_MSG msg);

    void on_pushButton_comandar_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void on_combo_modo_currentIndexChanged(int index);

    void on_spin_vol_speed_ref_editingFinished();
    void on_spin_vol_sbyspeed_ref_editingFinished();
    void on_spin_vol_isd_ref_editingFinished();
};

#endif // VOLANTE_H
