#include "dialogs.h"
#include <QDebug>

changeRadarNameDialog::changeRadarNameDialog(QWidget *parent, radarItem *selectedRadar)
    : QDialog(parent)
{

    selectedRadarItem = selectedRadar;

    centralLayout = new QVBoxLayout;
    buttonLayout = new QHBoxLayout;


    confirm = new QPushButton(tr("Accept"), this);
    cancel = new QPushButton(tr("Cancel"), this);
    buttonLayout->addWidget(confirm);
    buttonLayout->addWidget(cancel);
    confirm->setDefault(true);
    buttonWidget = new QWidget(this);
    buttonWidget->setLayout(buttonLayout);

    text = new QLineEdit(*selectedRadar->radarName(), this);
    info = new QLabel(tr("Write new name for radar"), this);


    centralLayout->addWidget(info);
    centralLayout->addWidget(text);
    centralLayout->addWidget(buttonWidget);


    this->setLayout(centralLayout);

    /**
      * If the Accept button is clicked, the name will change and parent
      * widget will obtain a signal about this action.
      *
      **/

    connect(confirm, SIGNAL(clicked()), this, SLOT(changeRadarNameSlot()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(this, SIGNAL(closeThisWindowWithAccept()), this, SLOT(close()));
}

void changeRadarNameDialog::changeRadarNameSlot(void)
{
    selectedRadarItem->setRadarName(new QString(text->text()));
    emit closeThisWindowWithAccept();
}

changeRadarNameDialog::~changeRadarNameDialog()
{
    delete centralWidget;
    delete buttonWidget;
    delete centralLayout;
    delete buttonLayout;
    delete confirm;
    delete cancel;
    delete info;
    delete text;
}

/* ------------------------------------------------------------------------------------------------------------ */

changeDimensionsOfWorkArea::changeDimensionsOfWorkArea(class MainWindow *parent)
    : QDialog(parent)
{
    parentWidget = parent;

    heightLayout = new QHBoxLayout;
    widthLayout = new QHBoxLayout;

    buttonWidget = new QWidget(this);
    widthWidget = new QWidget(this);
    heightWidget = new QWidget(this);
    widthSpinBox = new QDoubleSpinBox(this);
    heightSpinBox = new QDoubleSpinBox(this);

    widthLabel = new QLabel(tr("Width [m]: "), this);
    heightLabel = new QLabel(tr("Height [m]: "), this);

    buttonsLayout = new QHBoxLayout;
    confirm = new QPushButton(tr("Confirm"), this);
    cancel = new QPushButton(tr("Cancel"), this);

    centralLayout = new QVBoxLayout;

    widthSpinBox->setMaximum(200.0);
    widthSpinBox->setMinimum(1.0);
    heightSpinBox->setMaximum(200.0);
    heightSpinBox->setMinimum(1.0);
    widthSpinBox->setDecimals(2);
    heightSpinBox->setDecimals(2);

    widthSpinBox->setValue(parent->getRealWidth());
    heightSpinBox->setValue(parent->getRealHeight());

    heightLayout->addWidget(widthLabel);
    heightLayout->addWidget(widthSpinBox);
    widthLayout->addWidget(heightLabel);
    widthLayout->addWidget(heightSpinBox);


    widthWidget->setLayout(widthLayout);
    heightWidget->setLayout(heightLayout);


    buttonsLayout->addWidget(confirm);
    buttonsLayout->addWidget(cancel);
    buttonWidget->setLayout(buttonsLayout);
    confirm->setDefault(true);

    centralLayout->addWidget(widthWidget);
    centralLayout->addWidget(heightWidget);
    centralLayout->addWidget(buttonWidget);

    this->setLayout(centralLayout);

    connect(this->confirm, SIGNAL(clicked()), this, SLOT(changeDimensionsOfWorkAreaSlot()));
    connect(this->cancel, SIGNAL(clicked()), this, SLOT(close()));
}

void changeDimensionsOfWorkArea::changeDimensionsOfWorkAreaSlot()
{
    double w = widthSpinBox->value();
    double h = heightSpinBox->value();
    emit closeThisWindowWithAccept(w, h);
    this->close();
}

changeDimensionsOfWorkArea::~changeDimensionsOfWorkArea()
{
    delete heightLayout;
    delete widthLayout;
    delete centralLayout;
    delete buttonsLayout;

    delete widthSpinBox;
    delete heightSpinBox;

    delete widthLabel;
    delete heightLabel;

    delete confirm;
    delete cancel;

    delete widthWidget;
    delete heightWidget;
    delete buttonWidget;
}

/* ------------------------------------------------------------------------------------------------------------ */

options::options(MainWindow *parent)
    : QDialog(parent)
{
    parentWidget = parent;
    this->setWindowFlags(this->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    confirm = new QPushButton(tr("Confirm"), this);
    cancel = new QPushButton(tr("Cancel"), this);
    buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(confirm);
    buttonsLayout->addWidget(cancel);
    buttonWidget = new QWidget(this);
    buttonWidget->setLayout(buttonsLayout);
    connect(confirm, SIGNAL(clicked()), this, SLOT(updateOptionsSlot()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

    centralLayout = new QGridLayout;

    // input your options here

    int row = 0; // increment after each row

    // ANTENNAS DEPLOYMENT PIC
    antennas_deployment = new QPixmap(":/mainPics/pics/antennas_deployment.jpg");
    antennas_deploymentLabel = new QLabel(this);
    antennas_deploymentLabel->setPixmap(*antennas_deployment);
    antennas_deploymentLabel->setMask(antennas_deployment->mask());
    antennas_deploymentLabel->show();
    centralLayout->addWidget(antennas_deploymentLabel, row, 0, 3, 3);

    row += 4;

    // ANTENNAS DEPLOYMENT SETTING

    a_distance = new QDoubleSpinBox(this);
    b_distance = new QDoubleSpinBox(this);
    a_distance->setAlignment(Qt::AlignRight);
    b_distance->setAlignment(Qt::AlignRight);
    a_distance->setMinimum(0.0001);
    a_distance->setMaximum(100.0);
    a_distance->setDecimals(4);
    a_distance->setSingleStep(0.0001);
    b_distance->setMinimum(0.0001);
    b_distance->setMaximum(100.0);
    b_distance->setDecimals(4);
    b_distance->setSingleStep(0.0001);
    a_distance->setValue(parentWidget->getLeftAntDist());
    b_distance->setValue(parentWidget->getRightAntDist());

    a_distanceLabel = new QLabel("<b>a:</b>", this);
    a_distanceLabel->setMaximumWidth(8);
    b_distanceLabel = new QLabel("<b>b:</b>", this);
    b_distanceLabel->setMaximumWidth(8);

    distance_setting_layout = new QHBoxLayout;
    distance_setting_layout->addWidget(a_distanceLabel);
    distance_setting_layout->addWidget(a_distance);
    distance_setting_layout->addWidget(b_distanceLabel);
    distance_setting_layout->addWidget(b_distance);

    distance_setting_widget = new QWidget(this);
    distance_setting_widget->setLayout(distance_setting_layout);
    // note that picture has covered 3
    centralLayout->addWidget(distance_setting_widget, row, 0, 1, 3);

    row++;

    // SPACER

    OPTIONS_SPACER = new QFrame(this);
    OPTIONS_SPACER->setFrameShape(QFrame::HLine);
    OPTIONS_SPACER->setFrameShadow(QFrame::Sunken);

    centralLayout->addWidget(OPTIONS_SPACER, row, 0, 1, 3);

    row++;

    // REFRACTIVE INDEX
    refractiveIndex = new QDoubleSpinBox;
    refractiveIndex->setMinimum(0.000001);
    refractiveIndex->setDecimals(7);
    refractiveIndex->setSingleStep(0.0001);
    refractiveIndex->setValue(parentWidget->getRefractiveIndex());
    refractiveIndexLabel = new QLabel(tr("Refractive index <b>(material index)</b>"), this);
    centralLayout->addWidget(refractiveIndex, row, 0);
    centralLayout->addWidget(refractiveIndexLabel, row, 1, 1, 2);

    row++;

    // TIME STEP
    timeStep = new QDoubleSpinBox;
    timeStep->setDecimals(4);
    timeStep->setMinimum(0.0001);
    timeStep->setMaximum(5.0);
    timeStep->setValue(parentWidget->getTimeStep());
    timeStepLabel = new QLabel(tr("Saw time sampling period"), this);
    centralLayout->addWidget(timeStep, row, 0);
    centralLayout->addWidget(timeStepLabel, row, 1, 1, 2);

    row++;

    // ELLIPSES ANIMATION

    drawEllipsesLabel = new QLabel(tr("Draw ellipses in animation"), this);
    drawEllipsesCheckBox = new QCheckBox;
    if(parentWidget->getDrawEllipsesOption()) drawEllipsesCheckBox->setChecked(true);
    else drawEllipsesCheckBox->setChecked(false);
    if(parentWidget->getAnimationIsRunningStatus()) {
        drawEllipsesCheckBox->setDisabled(true);
        drawEllipsesLabel->setDisabled(true);
    } else {
        drawEllipsesCheckBox->setDisabled(false);
        drawEllipsesLabel->setDisabled(false);
    }
    centralLayout->addWidget(drawEllipsesCheckBox, row, 0, 1, 1, Qt::AlignRight);
    centralLayout->addWidget(drawEllipsesLabel, row, 1, 1, 2);

    row++;

    // TOA CONVERSION

    toaMode = new QCheckBox;
    toaMode->setChecked(parentWidget->getToaMode());
    toaModeLabel = new QLabel(tr("Convert TOA to distance"), this);
    centralLayout->addWidget(toaMode, row, 0, 1, 1, Qt::AlignRight);
    centralLayout->addWidget(toaModeLabel, row, 1, 1, 2);

    row++;

    // NOISE
    distortionType = parentWidget->getDistortionType();
    quantization = parentWidget->getQuantizationNumber();
    quantizationType = parentWidget->getQuantizationType();
    setMaximumDistortion();


    quantizationButton = new QPushButton(tr("Set quantization"), this);
    distortionButton = new QPushButton(tr("Distribution"), this);
    connect(distortionButton, SIGNAL(clicked()), this, SLOT(noiseDistributionSelectSlot()));
    distortionSlider = new QSlider(Qt::Horizontal, this);
    distortionSlider->setMinimum(1); // distortion will be calculated from maxDistortion from percentage
    distortionSlider->setMaximum(100); // 100%
    distortionSlider->setValue((parentWidget->getDistortion()/max_distortion)*100.0);
    distortionLabel = new QLabel; this->distortionChangedSlot(0); // by manual calling distortionChangeSlot we can also update label value
    connect(distortionSlider, SIGNAL(sliderMoved(int)), this, SLOT(distortionChangedSlot(int)));
    distortionCheckBox = new QCheckBox(this);
    distortionCheckBox->setChecked(parentWidget->getDistortionOption());
    if(!distortionCheckBox->isChecked()) {
        this->distortionSlider->setDisabled(true);
        this->distortionLabel->setDisabled(true);
        this->distortionButton->setDisabled(true);
    }
    centralLayout->addWidget(distortionCheckBox, row, 0, 1, 1, Qt::AlignRight);
    centralLayout->addWidget(distortionLabel, row, 1);

    row++; // slider will be in another row

    centralLayout->addWidget(distortionSlider, row, 1);
    centralLayout->addWidget(distortionButton, ++row, 1);
    connect(distortionCheckBox, SIGNAL(clicked(bool)), this, SLOT(distortionOptSlot(bool)));

    row++; // set Quantization to another row

    centralLayout->addWidget(quantizationButton, row, 1);
    connect(quantizationButton, SIGNAL(clicked()), this, SLOT(setQuantizationSlot()));

    row++;

    // SPACER

    OPTIONS_SPACER_2 = new QFrame(this);
    OPTIONS_SPACER_2->setFrameShape(QFrame::HLine);
    OPTIONS_SPACER_2->setFrameShadow(QFrame::Sunken);

    centralLayout->addWidget(OPTIONS_SPACER_2, row, 0, 1, 3);

    // final row
    centralWidget = new QWidget;
    centralWidget->setLayout(centralLayout);

    mainLayout = new QVBoxLayout;
    mainLayout->addWidget(centralWidget);
    mainLayout->addWidget(buttonWidget);

    this->setLayout(mainLayout);
}

options::~options()
{
    delete OPTIONS_SPACER;
    delete OPTIONS_SPACER_2;

    delete distance_setting_layout;
    delete buttonsLayout;
    delete centralLayout;
    delete mainLayout;

    delete confirm;
    delete cancel;
    delete refractiveIndex;
    delete buttonWidget;
    delete a_distance;
    delete a_distanceLabel;
    delete b_distance;
    delete b_distanceLabel;
    delete refractiveIndexLabel;
    delete toaModeLabel;
    delete distortionLabel;
    delete drawEllipsesLabel;
    delete distance_setting_widget;
    delete antennas_deploymentLabel;
    delete antennas_deployment;
    delete timeStepLabel;
    delete distortionButton;
    delete quantizationButton;

    delete distortionSlider;
    delete distortionCheckBox;
    delete toaMode;
    delete timeStep;

    delete centralWidget;
}

void options::updateOptionsSlot()
{
    parentWidget->setDistortion(current_distortion, distortionCheckBox->isChecked());
    parentWidget->setAntennaDistance(a_distance->value(), b_distance->value());
    parentWidget->setDrawEllipsesOption(drawEllipsesCheckBox->isChecked());
    parentWidget->setRefractiveIndex(refractiveIndex->value());
    parentWidget->setToaMode(toaMode->isChecked());
    parentWidget->setTimeStep(timeStep->value());
    parentWidget->setDistortionType(distortionType);
    parentWidget->setQuantizationValue(quantization);
    parentWidget->setQuantizationType(quantizationType);

    // close dialog
    this->close();
}

void options::distortionOptSlot(bool isChecked)
{
    if(isChecked) {
        this->distortionSlider->setDisabled(false);
        this->distortionLabel->setDisabled(false);
        this->distortionButton->setDisabled(false);
    }
    else
    {
        this->distortionSlider->setDisabled(true);
        this->distortionLabel->setDisabled(true);
        this->distortionButton->setDisabled(true);
    }
}

double options::setMaximumDistortion()
{
    switch(distortionType)
    {
        case 2:
            max_distortion = 1.0; // FOR UNIFORM DISTRIBUTION
        break;
        case 3:
            max_distortion = 1.0; // FOR NORMAL DISTRIBUTION
        break;
        case 4:
            max_distortion = 1000.0; // FOR EXPONENTIAL DISTRIBUTION
        break;
        default:
            max_distortion = 1.0; // FOR UNIFORM DISTRIBUTION
        break;
    }
}

void options::distortionChangedSlot(int v)
{
    QChar ch;

    if(distortionType==2)
            ch = QChar(0x394); // FOR UNIFORM DISTRIBUTION
    else if(distortionType==3)
            ch = QChar(0x3C3); // FOR NORMAL DISTRIBUTION
    else if(distortionType==4)
            ch = QChar(0x3BB); // FOR EXPONENTIAL DISTRIBUTION
    else ch = QChar(0x394); // FOR UNIFORM DISTRIBUTION


    current_distortion = (max_distortion/100.0)*((double)(distortionSlider->value()));
    distortionLabel->setText(tr("Noise effect <b>(%2: %1)</b>").arg(current_distortion).arg(ch));
}

void options::setDistortionType(int t)
{
    distortionType = t;

    setMaximumDistortion();
}

void options::noiseDistributionSelectSlot()
{
    noiseDistributionSelectDialog newDialog(this, distortionType);
    newDialog.exec();

    // parameter value

    QChar ch;

    if(distortionType==2)
            ch = QChar(0x394); // FOR UNIFORM DISTRIBUTION
    else if(distortionType==3)
            ch = QChar(0x3C3); // FOR NORMAL DISTRIBUTION
    else if(distortionType==4)
            ch = QChar(0x3BB); // FOR EXPONENTIAL DISTRIBUTION
    else ch = QChar(0x394); // FOR UNIFORM DISTRIBUTION


    distortionLabel->setText(tr("Noise effect <b>(%2: %1)</b>").arg((max_distortion/100.0)*(double)(distortionSlider->value())).arg(ch));
}

void options::setQuantizationSlot()
{
    setQuantizationDialog quantizationDialog(this, quantization, quantizationType, parentWidget->getRefractiveIndex());
    connect(&quantizationDialog, SIGNAL(confirmed(double, short)), this, SLOT(setNewQuantizationSlot(double, short)));
    quantizationDialog.exec();
}

void options::setNewQuantizationSlot(double newQuantization, short quantizationT)
{
    quantization = newQuantization;
    quantizationType = quantizationT;
}
