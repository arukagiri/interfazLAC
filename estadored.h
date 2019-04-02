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

    void var_changed(uint16_t var,uint16_t data);

    void on_pushButton_clicked();

    void on_button_vol_clicked();

    void on_button_boost_clicked();

private:
    Ui::EstadoRed *ui;
    MainWindow* mw ;
    QTimer *time_2sec;

    data_can recibed_val;

    bool send_query;
    float gen_vo;
    float gen_io;
    float gen_vel;
    float gen_tor;
    uint16_t gen_mod;

    float boost_vo;
    float boost_io;
    float boost_vi;
    float boost_ii;
    uint16_t boos_mod;

    float vol_vo;
    float vol_io;
    float vol_vel;
    float vol_tor;
    uint16_t vol_mod;

    void refresh_values(void);
    void send_qry(void);
    void set_states(void);

};

#endif // ESTADORED_H
