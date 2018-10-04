#ifndef ADDNEWDEVDIALOG_H
#define ADDNEWDEVDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class AddNewDevDialog;
}

class AddNewDevDialog : public QDialog
{
    Q_OBJECT

public:
    QString devname;
    explicit AddNewDevDialog(QWidget *parent = 0);
    virtual ~AddNewDevDialog() override;

signals:
    void dev_name_set(QString);

    void add_name_canceled();

protected:
    virtual void closeEvent(QCloseEvent *e) override;

private slots:
    void on_ok_button_clicked();

    void on_cancel_button_clicked();

private:
    Ui::AddNewDevDialog *ui;
};

#endif // ADDNEWDEVDIALOG_H
