#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QPen>
#include <QMetaType>
#include <QListWidgetItem>
#include <QInputDialog>

#include <QDir>
#include <QFile>
#include <QTime>
#include <QDate>
#include <QFileInfo>
#include <QFileInfoList>
#include <QTimer>

#include <random>

#include "radaritem.h"
#include "monitoredareascenesubclass.h"
#include "routemath.h"
#include "structures.h"
#include "dialogs.h"
#include "transformation.h"
#include "routeanimation.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool drawingModeIsEnabled(void) { return drawingMode; }
    void setDrawingMode(bool s) { drawingMode = s; }
    void hideLastPanel(void) { if(displayedPanel!=NULL) displayedPanel->setHidden(true); }
    void setNewPanel(QGroupBox *newPanel) { displayedPanel = newPanel; }
    void addNewPoint(QGraphicsEllipseItem *p, QColor c);
    void deleteRoute(struct route *rt);
    void calculateRatios(void);
    double getRealWidth(void) { return realAreaWidth; }
    double getRealHeight(void) { return realAreaHeight; }

    bool getDistortionOption(void) { return use_distortion; }
    double getDistortion(void) { return distortion; }
    int getDistortionType(void) { return distortionType; }
    void setDistortionType(int t) { distortionType = t; }
    double getLeftAntDist(void) { return left_antenna_dist; }
    double getRightAntDist(void) { return right_antenna_dist; }
    void setAntennaDistance(double a, double b) { left_antenna_dist = a; right_antenna_dist = b; }
    void setDistortion(double dist, bool distEnabled) { distortion = dist; use_distortion = distEnabled; }
    bool getDrawEllipsesOption(void) { return drawEllipses; }
    void setDrawEllipsesOption(bool a) { drawEllipses = a; }
    bool getAnimationIsRunningStatus(void) { return animationRunning; }
    double getRefractiveIndex(void) { return refractiveIndex; }
    void setRefractiveIndex(double ri) { refractiveIndex = ri; }
    bool getToaMode(void) { return toaMode; }
    void setToaMode(bool tm) { toaMode = tm; }
    double getTimeStep(void) { return timeStep; }
    void setTimeStep(double ts) { timeStep = ts; }
    void clearPlotsObjects(void);
    void configureXYAxisTicks(void);
    double quantizeNumber(double num);
    double getQuantizationNumber(void) { return quantization; }
    void setQuantizationValue(double quantization_val) { quantization = quantization_val; }
    short getQuantizationType(void) { return quantizationType; }
    void setQuantizationType(short type) { quantizationType = type; }

    void centerEllipseItems(double x, double y, int index);
    double calculateDistance(double x1, double y1, double x2 = 0.0, double y2 = 0.0);
    double getTOA1(double x, double y);
    double getTOA2(double x, double y);

    struct route *lastPoint(struct route *startPoint); // returns last point of route specified by start point
    void setRouteColor(struct route *startPoint, QColor newColor, int width); // will change route color for every line and dot
    ~MainWindow();

private:

    const double PI = 3.14159265358979323846264338327950288419716939937510;
    const double speedOfLight = 299792458.0; // meters per second
    const double nanosec = 1000000000.0; // for conversion from seconds to nanoseconds
    double refractiveIndex; // index which represents relation between speed of light in vacuum and some another material
    bool toaMode; // if set to true, toa value will be represented as distance instead of time

    double makeDistortion(double coordinate);
    void removeDirRecursively(const QString &dir);

    double degToRads(double deg);

    Ui::MainWindow *ui;
    class monitoredAreaSceneSubclass *monitoredAreaScene; // main scene where all graphical objects are being painted
    QRectF *wall; // border around monitored area
    class radarItem *selectedRadar; // pointer to the last clicked radar
    struct route *selectedRoute; // last selected route
    QColor lastColor; // routes initial color

    QGroupBox *displayedPanel; // stores the pointer to the last displayed group box on the left side of working area 

    bool drawingMode; // while adding new route, this is set to true
    struct route *currentDrawing; // the pointer to the currently drawing route - last structure in chain
    struct route *currentDrawingBase; // the pointer to the currently drawing route - last structure in chain

    QList<class radarItem *> radarList; // list of all radar items in View
    QList<struct route *> routeList; // list of all route start points in View

    bool beforeHideDrawn; // if object goes behind radar (y<0.0) then we need to plot what we already have
    bool animationRunning; // if animation is running, this value is set to true
    QTimer * animationTimer; // pointer to timer managing the speed of animation
    class transformation2D * radarAnimationBackup; // here  is the transformation object stored when the animation is running
    QList <class routemath * > routeMathAnimationBackup; // here are pointers to all objects computing new coordinates during animation
    QList <class QCPItemCircle * > routeAnimationCircleBackup; // storage for all objects rendering in plot
    QList <QCPItemEllipse * > routeAnimationEllipseLeftBackup; //storege for ellipses drawn in plot for left radar antenna
    QList <QCPItemEllipse * > routeAnimationEllipseRightBackup; //storege for ellipses drawn in plot for right radar antenna
    QCPItemRect *leftAntenna; // points to position of left antenna
    QCPItemRect *rightAntenna; // points to position of right antenna
    QVector <double> *axisPlotV;

    /* ------ OPTIONS ------- */

    double distortion;
    double quantization;
    short quantizationType;
    int distortionType;
    bool use_distortion; // if this option is enabled, the value in distortion will set the range for rand function to make result coords more realistic
    double timeStep; // the time interval between getting values from radar
    double left_antenna_dist; // left reciever antenna distance from central transmitting antenna
    double right_antenna_dist; // right reciever antenna distance from central transmitting antenna

    double realAreaWidth; // this is real width (in real situation where radars are supposed to be tested), default = 10 metres
    double realAreaHeight; // this is real height (in real situation where radars are supposed to be tested), default = 10 metres
    double widthRatio; // is ratio of area scene in pixels to realAreaWidth (sceneRect)
    double heightRatio; // is ratio of area scene in pixels to realAreaWidth

    double defaultSpeed;

    bool drawEllipses; // if true, also ellipses representing TAO measure are to be drawn

    std::default_random_engine * generator; // generator of random numbers - used for noise generating

    /* ------ OPTIONS END --- */

public slots:
    void generateSimulationFilesSlot(void); // THIS SLOT WILL START GENERATING SEQUENCE INCLUDING FILE CREATION, COMPUTING COORDINATES ETC.

    void radarItemWasClickedSlot(void);
    void radarItemWasMovedSlot(void);
    void rotateRadarClockwiseSlot(void);
    void rotateRadarUnClockwiseSlot(void);
    void addRouteSlot(void);
    void addRadarSlot(void);
    void deleteRouteSlot(void);
    void deleteRadarSlot(void);
    void changeRadarNameSlot(void);
    void updateRadarListWidgetSlot(void);
    void updateRadarNamePanel(void);
    void radarItemWasSelectedSlot(QListWidgetItem*i);
    void routeItemWasSelectedSlot(QListWidgetItem*i);
    void updateSpeedSlot(void);
    void updateRouteNameSlot(void); 

    void changeDimensionsSlot(void);
    void updateDimensionsSlot(double width, double height);
    void changeOptionsSlot(void);
    void aboutActionSlot();


    void updatePlotTabSlot(int index);

    void plotSequenceSlot(void);
    void animationSequenceSlot(void);
    void animationSequenceNextStepSlot(void);
};

#endif // MAINWINDOW_H
