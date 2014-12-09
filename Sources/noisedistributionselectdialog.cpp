#include "noisedistributionselectdialog.h"
#include "ui_noisedistributionselectdialog.h"

noiseDistributionSelectDialog::noiseDistributionSelectDialog(class options *parent, int distribution) :
    QDialog(parent),
    ui(new Ui::noiseDistributionSelectDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowTitle(tr("Noise settings"));

    parentWidget = parent;

    distributionType = distribution;

    switch(distribution)
    {
        case 2:
            ui->whiteNoiseRadio->setChecked(true);
        break;
        case 3:
            ui->normalDistributionRadio->setChecked(true);
        break;
        case 4:
            ui->exponentialDistributionRadio->setChecked(true);
        break;
        default:
            ui->whiteNoiseRadio->setChecked(true);
        break;
    }

    connect(ui->confirmButton, SIGNAL(clicked()), this, SLOT(updateDistortionType()));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

noiseDistributionSelectDialog::~noiseDistributionSelectDialog()
{
    delete ui;
}

void noiseDistributionSelectDialog::updateDistortionType()
{
    parentWidget->setDistortionType((-1)*ui->buttonGroup->checkedId());
    this->close();
}
