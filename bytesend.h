#ifndef BYTESEND_H
#define BYTESEND_H

#include <QDialog>

namespace Ui {
class ByteSend;
}

class ByteSend : public QDialog
{
    Q_OBJECT

public:
    explicit ByteSend(QWidget *parent = 0);
    ~ByteSend();

private slots:
    void on_dlc_currentIndexChanged(int index);

private:
    Ui::ByteSend *ui;
};

#endif // BYTESEND_H
