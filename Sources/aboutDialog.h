#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "ui_aboutDialog.h"

class aboutDialogCl : public QDialog, public Ui::aboutDialog
{
    Q_OBJECT

public:
    aboutDialogCl(QWidget * parent = 0);
private:
signals:
public slots:
};

#endif // ABOUTDIALOG_H
