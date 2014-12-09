#include "aboutDialog.h"


aboutDialogCl::aboutDialogCl(QWidget * parent) : QDialog(parent)
{
    this->setupUi(this);

    connect(this->Ui_aboutDialog::close, SIGNAL(clicked()), this, SLOT(close()));
    // constructor
}
