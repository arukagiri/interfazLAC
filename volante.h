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
#include <QShortcut>

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
    int refValue = -9999; // un valor que sea menor que cualquiera de los limites
    QShortcut* editHotKey;

    data_can recibed_val;

    //paramétros del volante
    const double J = 0.5726;

    //Variables de Entrada
    float id_ref = refValue;
    float speed_ref = refValue;
    float standby_ref = refValue;

    //Variables de Salida
    float vol_vo = refValue;
    float vol_io = refValue;
    float vol_po = refValue;
    float vol_ibat = refValue;
    float vol_vel = refValue;
    float vol_tor = refValue;
    float vol_ener = refValue;
    bool send_queries;
    bool referenceChanged;

    uint16_t actual_mode = uint16_t(refValue);
    uint16_t previous_mode;
    uint16_t cmd;
    data_can val;
    QString text_val;

    QTimer *time_2sec;

    void refresh_values();
    void send_qry_variables();
    void send_qry_references();
    void processEditingFinished(QDoubleSpinBox* spin, uint16_t var, float prevValue);
    void blockAllSpinSignals(bool b);

signals:
    void volWindowsClosed();

private slots:

    void timer_handler();

    void VOLpost_Handler(LACAN_MSG msg);

    void on_pushButton_comandar_clicked();

    void on_pushButton_start_clicked();

    void on_pushButton_stop_clicked();

    void on_spin_vol_sbyspeed_ref_editingFinished();

    void on_spin_vol_isd_ref_editingFinished();

    void on_edit_checkBox_stateChanged(int check);

    void changeEditState();

    void on_pushButton_shutdown_clicked();
};

#endif // VOLANTE_H
