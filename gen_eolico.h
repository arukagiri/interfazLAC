#ifndef GEN_EOLICO_H
#define GEN_EOLICO_H

#include <QDialog>

namespace Ui {
class Gen_Eolico;
}

class Gen_Eolico : public QDialog
{
    Q_OBJECT

public:
    explicit Gen_Eolico(QWidget *parent = 0);
    ~Gen_Eolico();

private:
    Ui::Gen_Eolico *ui;
};

#endif // GEN_EOLICO_H
