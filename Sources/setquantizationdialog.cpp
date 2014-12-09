#include "setquantizationdialog.h"
#include "ui_setquantizationdialog.h"

setQuantizationDialog::setQuantizationDialog(QWidget *parent, double quantization, short quantizationType, double refractiveIndex) :
    QDialog(parent),
    ui(new Ui::setQuantizationDialog)
{
    ui->setupUi(this);

    if(quantizationType > 0)
    {
        this->ui->customFrequencySpinBox->setDecimals(2);
        this->ui->radarCustomGHz->setText(tr("Custom frequency [GHz]"));

        this->ui->radarFrequency->setDisabled(false);

        switch(quantizationType)
        {
            case 1:
                this->ui->radarCustomGHz->setChecked(true);
                break;
            case 2:
                this->ui->radar13GHz->setChecked(true);
                break;
            case 3:
                this->ui->radar45GHz->setChecked(true);
                break;
            case 4:
                this->ui->radar99GHz->setChecked(true);
                break;
            case 5:
                this->ui->radar695GHz->setChecked(true);
                break;
            default:
                this->ui->radar13GHz->setChecked(true);
                break;
        }

        this->ui->customFrequencySpinBox->setValue(this->minimumTOAStepTime(quantization));
    }

    connect(ui->confirmButton, SIGNAL(clicked(QAbstractButton*)), this, SLOT(confirmedClicked()));
}

setQuantizationDialog::~setQuantizationDialog()
{
    delete ui;
}

double setQuantizationDialog::minimumTOAStepCalc(double freq)
{
    return 1/freq; // returning in nanoseconds... modify here if you want another measure
}

double setQuantizationDialog::minimumTOAStepTime(double period)
{
    return 1/period;
}

void setQuantizationDialog::confirmedClicked()
{
    // when returning quantization, automatically convert to correct minimum TOA step
    double minTOAstep;
    short quantType;


        // switching selection
        if(this->ui->radar13GHz->isChecked()) { minTOAstep = minimumTOAStepCalc(13.0); quantType = 2; }
        else if(this->ui->radar45GHz->isChecked()) { minTOAstep = minimumTOAStepCalc(4.5); quantType = 3; }
        else if(this->ui->radar99GHz->isChecked()) { minTOAstep = minimumTOAStepCalc(9.9); quantType = 4; }
        else if(this->ui->radar695GHz->isChecked()) { minTOAstep = minimumTOAStepCalc(6.95); quantType = 5; }
        else { minTOAstep = minimumTOAStepCalc(this->ui->customFrequencySpinBox->value()); quantType = 1; }


    emit this->confirmed(minTOAstep, quantType);
}
