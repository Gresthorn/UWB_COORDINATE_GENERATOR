#ifndef NOISEDISTRIBUTIONSELECTDIALOG_H
#define NOISEDISTRIBUTIONSELECTDIALOG_H

#include <QDialog>
#include "dialogs.h"

namespace Ui {
class noiseDistributionSelectDialog;
}

class noiseDistributionSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit noiseDistributionSelectDialog(class options *parent = 0, int distribution = 0);
    ~noiseDistributionSelectDialog();

private:
    Ui::noiseDistributionSelectDialog *ui;

    class options *parentWidget;

    int distributionType;

public slots:
    void updateDistortionType(void);
};

#endif // NOISEDISTRIBUTIONSELECTDIALOG_H
