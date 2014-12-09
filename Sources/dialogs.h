#ifndef DIALOGS_H
#define DIALOGS_H

#include <QObject>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>

#include "noisedistributionselectdialog.h"
#include "setquantizationdialog.h"

#include "radaritem.h"
#include "mainwindow.h"

class changeRadarNameDialog : public QDialog
{

    /**
      * @brief Dialog for changing radar name
      *
      **/

    Q_OBJECT

private:
    class radarItem *selectedRadarItem;

    QWidget *centralWidget;
    QWidget *buttonWidget;

    QVBoxLayout *centralLayout;
    QHBoxLayout *buttonLayout;

    QPushButton *confirm;
    QPushButton *cancel;

    QLabel *info;
    QLineEdit *text;

public:
    changeRadarNameDialog(QWidget *parent, class radarItem *selectedRadar);
    ~changeRadarNameDialog();

public slots:
    void changeRadarNameSlot(void);

signals:
    void closeThisWindowWithAccept(void);

};

class changeDimensionsOfWorkArea : public QDialog
{
    /**
      * @brief Dialog for changing dimensions (width and height) of work area
      * to real lengths in metres.
      *
      **/

    Q_OBJECT

private:
    class MainWindow *parentWidget;

    QVBoxLayout *centralLayout;

    QDoubleSpinBox *widthSpinBox;
    QLabel *widthLabel;
    QHBoxLayout *widthLayout;
    QWidget *widthWidget;
    QDoubleSpinBox *heightSpinBox;
    QLabel *heightLabel;
    QHBoxLayout *heightLayout;
    QWidget *heightWidget;

    QHBoxLayout *buttonsLayout;
    QWidget *buttonWidget;
    QPushButton *confirm;
    QPushButton *cancel;

public:
    changeDimensionsOfWorkArea(class MainWindow * parent);
    ~changeDimensionsOfWorkArea();

public slots:
    void changeDimensionsOfWorkAreaSlot(void);
signals:
    void closeThisWindowWithAccept(double, double);
};

class options : public QDialog
{
    /**
      * @brief Dialog for setting basic options that have effect on generating
      * coordinates or making different
      *
      **/

    Q_OBJECT

private:
    class MainWindow *parentWidget;

    double setMaximumDistortion(void);

    QPixmap *antennas_deployment;
    QLabel *antennas_deploymentLabel;

    QDoubleSpinBox *a_distance;
    QDoubleSpinBox *b_distance;
    QLabel *a_distanceLabel;
    QLabel *b_distanceLabel;
    QHBoxLayout *distance_setting_layout;
    QWidget *distance_setting_widget;

    QGridLayout *centralLayout;
    QWidget *centralWidget;

    QDoubleSpinBox *timeStep;
    QLabel *timeStepLabel;

    QDoubleSpinBox *refractiveIndex;
    QLabel *refractiveIndexLabel;

    QCheckBox *toaMode;
    QLabel *toaModeLabel;

    QVBoxLayout *mainLayout; // merge content and button into one layout

    QHBoxLayout *buttonsLayout;
    QWidget *buttonWidget;

    QPushButton *confirm;
    QPushButton *cancel;

    QLabel *distortionLabel;
    QPushButton *distortionButton;
    QLabel *drawEllipsesLabel;
    QSlider *distortionSlider;
    QCheckBox *distortionCheckBox;
    QCheckBox *drawEllipsesCheckBox;

    QPushButton *quantizationButton;

    QFrame *OPTIONS_SPACER;
    QFrame *OPTIONS_SPACER_2;

    double quantization;
    short quantizationType;
    double max_distortion;
    double current_distortion;
    int distortionType;

public:
   options(class MainWindow * parent);

   void setDistortionType(int t);

   ~options();

public slots:
   void updateOptionsSlot();

   void distortionOptSlot(bool isChecked);
   void distortionChangedSlot(int v);

   void noiseDistributionSelectSlot(void);
   void setQuantizationSlot(void);

   void setNewQuantizationSlot(double newQuantization, short quantizationT);
signals:
};

#endif // DIALOGS_H
