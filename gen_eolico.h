#ifndef GEN_EOLICO_H
#define GEN_EOLICO_H

#include <QDialog>
#include "stdint.h"
#include <QTime>
#include "PC.h"
#include "comandar.h"
#include "mainwindow.h"
#include <QString>

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
    uint16_t val;
    QString text_val;

    QTimer *time_2sec;

    void new_mode();
    void refresh_values();
    void send_qry();

    //variables para guardar el valor a mostrar
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

    float gen_vo;
    float gen_io;

    //cuando se edita un valor
    void set_lineEdit_click(bool state);
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



private slots:

    void timer_handler();

    void mode_changed();

    void GENpost_Handler(LACAN_MSG msg);

    void on_pushButton_apply_clicked();
    void on_pushButton_cancel_clicked();
    void on_lineEdit_speed_ref_textEdited(const QString &arg1);
    void on_lineEdit_speed_ref_cursorPositionChanged(int arg1, int arg2);
    void on_lineEdit_pot_ref_textChanged(const QString &arg1);
    void on_lineEdit_lim_vdc_textChanged(const QString &arg1);
    void on_lineEdit_vdc_textChanged(const QString &arg1);
    void on_lineEdit_iconv_textChanged(const QString &arg1);
    void on_lineEdit_lim_ief_textChanged(const QString &arg1);
    void on_lineEdit_torque_ref_textChanged(const QString &arg1);
    void on_lineEdit_isd_ref_textChanged(const QString &arg1);
    void on_lineEdit_lim_ibat_textChanged(const QString &arg1);
    void on_lineEdit_ibat_textChanged(const QString &arg1);
    void on_pushButton_comandar_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
};

#endif // GEN_EOLICO_H
