#ifndef BOOST_H
#define BOOST_H

#include <QDialog>

namespace Ui {
class boost;
}

class boost : public QDialog
{
    Q_OBJECT

public:
    explicit boost(QWidget *parent = nullptr);
    ~boost();

private:
    Ui::boost *ui;
};

#endif // BOOST_H
