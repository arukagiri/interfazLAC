#ifndef ESTADORED_H
#define ESTADORED_H

#include <QDialog>
#include "PC.h"
#include "mainwindow.h"
#include <QTime>

namespace Ui {
class EstadoRed;
}

class EstadoRed : public QDialog
{
    Q_OBJECT

public:
    explicit EstadoRed(QWidget *parent);
    ~EstadoRed();
protected:
    virtual void closeEvent(QCloseEvent *e) override;
signals:
    void postforGEN_arrived(LACAN_MSG msg);
    void postforVOL_arrived(LACAN_MSG msg);

public slots:
    void handle_dispWindowsClosed();

private slots:
    void ERpost_Handler(LACAN_MSG msg);

    void on_button_gen_clicked();

    void timer_handler();

    void on_button_vol_clicked();

    void on_button_boost_clicked();

    void on_pushButton_gen_enable_clicked();

    void on_pushButton_gen_disable_clicked();

    void on_pushButton_vol_enable_clicked();

    void on_pushButton_vol_disable_clicked();

private:
    Ui::EstadoRed *ui;
    MainWindow* mw ;
    QTimer *time_2sec;

    data_can recibed_val;
    float refValue = -9999.0;
    uint16_t modRefValue = 0xFFFF;

    bool send_queries=false;
    float gen_vo = refValue;
    float gen_io = refValue;
    float gen_vel = refValue;
    float gen_tor = refValue;
    uint16_t gen_mod = modRefValue;

    float boost_vo = refValue;
    float boost_io = refValue;
    float boost_vi = refValue;
    float boost_ii = refValue;
    uint16_t boos_mod = modRefValue;

    float vol_vo = refValue;
    float vol_io = refValue;
    float vol_vel = refValue;
    float vol_tor = refValue;
    uint16_t vol_mod = modRefValue;

    uint16_t cmd;
    uint16_t dest;

    void refresh_values(void);
    void send_qry(void);
    void set_states(void);

};

#endif // ESTADORED_H
