#ifndef SETQUANTIZATIONDIALOG_H
#define SETQUANTIZATIONDIALOG_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class setQuantizationDialog;
}

class setQuantizationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit setQuantizationDialog(QWidget *parent = 0, double quantization = 0.002, short quantizationType = 1, double refractiveIndex = 1.000293);
    ~setQuantizationDialog();

private:
    Ui::setQuantizationDialog *ui;
    double minimumTOAStepCalc(double freq);
    double minimumTOAStepTime(double period);


public slots:
    void confirmedClicked(void);


signals:
    void confirmed(double newQuantization, short quantizationT);
};

#endif // SETQUANTIZATIONDIALOG_H
