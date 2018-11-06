#ifndef BYTESEND_H
#define BYTESEND_H

#include <QDialog>
#include <stdint.h>
#include "mainwindow.h"

namespace Ui {
class ByteSend;
}

class ByteSend : public QDialog
{
    Q_OBJECT

public:
    explicit ByteSend(QWidget *parent);
    MainWindow* mw;

    ~ByteSend();

private slots:
    void on_dlc_currentIndexChanged(int index);

    void on_pushButton_clicked();

private:
    Ui::ByteSend *ui;
    uint8_t text2int(const QString);
    uint8_t actual_dlc = 1;
};

#endif // BYTESEND_H
