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

private slots:
    void ERpost_Handler(LACAN_MSG msg);

    void on_button_gen_clicked();

    void timer_handler();

    void var_changed(uint16_t var,uint16_t data);

private:
    Ui::EstadoRed *ui;
    MainWindow* mw ;
    QTimer *time_2sec;

    float gen_vo;
    float gen_io;
    float gen_vel;
    float gen_tor;

    float boost_vo;
    float boost_io;
    float boost_vi;
    float boost_ii;

    float vol_vo;
    float vol_io;
    float vol_vel;
    float vol_tor;

    void refresh_values(void);
    void send_qry(void);

};

#endif // ESTADORED_H
