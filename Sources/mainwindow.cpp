#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutDialog.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    qDebug() << QDir::currentPath();

    this->setWindowTitle(tr("UWB - coordinate generator"));

    srand(time(NULL)); // see makeDistortion() function
    generator = new default_random_engine;

    /**
      * @brief This is the constructor of MainWindow with all widgets showed
      * when the application is firstly run.
      *
      **/

    ui->setupUi(this);

    /**
      * @brief The following code adds a new scene to the view in MainWindow
      * Remember, that the code relies on static width and height of
      * scene to be 560 x 560 (Program suppose the radar image to be
      * 60 pixels high). This information is later used for conversion
      * to real coordinates in metres.
      * MonitoredAreaView has width and height set to 562 x 562 because
      * of very close width/height parameters of widgets inside what
      * may evoke displaying scrollbars. Two pixels more will ensure
      * this is not going to happen.
      *
      **/
    /* ========================== BASIC VARIABLE INITIALIZATION ========================== */

    // setting selectedRadar pointer to NULL to protect program from trying use rotation slots if nothing is selected
    selectedRadar = NULL;
    // setting selectedRoute pointer to NULL to protect program from trying change color of route if nothing was selected yet
    selectedRoute = NULL;
    ui->radarPanel->setHidden(true);
    ui->routePanel->setHidden(true);
    drawingMode = false;

    displayedPanel = NULL;

    refractiveIndex = 1.000293; // index for air
    defaultSpeed = 1.5;
    realAreaWidth = 10.0;
    realAreaHeight = 10.0;
    timeStep = 1.0/13.0;
    left_antenna_dist = 1.0;
    right_antenna_dist = 1.0;

    ui->tabWidget->setCurrentIndex(0);
    ui->routePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // in plotTab it is possible to select more routes to plot them together
    ui->routesPlotList->setSelectionMode(QAbstractItemView::MultiSelection);

    // animation variables init
    animationRunning = false;
    beforeHideDrawn = false;
    drawEllipses = false;
    toaMode = false;
    animationTimer = NULL;
    leftAntenna = NULL;
    rightAntenna = NULL;
    radarAnimationBackup = NULL;
    axisPlotV = NULL;


    /* ========================== BASIC SCENE AND VIEW SETTINGS ========================== */

    monitoredAreaScene = new monitoredAreaSceneSubclass(this);
    // firstly declare radarItem because it contains the height of radar_pic.png
    radarItem *radar = new radarItem("OPERATOR");
    monitoredAreaScene->addItem(radar); // will be 'OPERATOR RADAR'   
    // signals/slots of QGraphicsItemGroup is allowed only because inheritance of QObject class
    radar->setFlags(QGraphicsItem::ItemIsSelectable);
    connect((radarItem *)(radar), SIGNAL(radarItemIsClickedSignal()), this, SLOT(radarItemWasClickedSlot()));

    // creating new radar list and adding the 'OPERATOR' with index 0
    radarList.append(radar);
    ui->radarsList->addItem(tr("OPERATOR RADAR"));
    ui->radarsList->item(radarList.count()-1)->setTextColor(Qt::red);
    ui->radarsList->item(radarList.count()-1)->setData(Qt::UserRole, (radarList.count()-1));

    // use height of radar_pic.png to get better sizing of scene
    // finally monitored area is going to have exactly 500 x 500 pixels
    monitoredAreaScene->setSceneRect(-250.0-(radar->radarPicHeight()), 0.0-(radar->radarPicHeight()),
                                     500.0+(radar->radarPicHeight())*2, 500.0+(radar->radarPicHeight())*2);
    ui->monitoredAreaView->centerOn(0.0, 250.0);
    // switch y axis to have positive values upper
    ui->monitoredAreaView->scale(1.0, -1.0);
    ui->monitoredAreaView->setScene(monitoredAreaScene);

    /**
      * @brief Adding the border which will graphically separate monitored
      * area from radar position area. In real world this might be
      * the wall, or obstacle positioned in front of radars while
      * observing objects.
      *
      **/

    wall = new QRectF(-250.0, 0.0, 500.0, 500.0);
    monitoredAreaScene->addRect(*wall, QPen(Qt::black, 2, Qt::SolidLine));

    this->calculateRatios();

    /* ========================== SETTING UP BUTTON ACTIONS ========================== */

    // radar panel is hidden until one of radars is clicked

    connect(ui->generateFilesButton, SIGNAL(clicked()), SLOT(generateSimulationFilesSlot()));
    connect(ui->rotatePlusButton, SIGNAL(pressed()), this, SLOT(rotateRadarClockwiseSlot()));
    connect(ui->rotateMinusButton, SIGNAL(pressed()), this, SLOT(rotateRadarUnClockwiseSlot()));
    connect(ui->addRouteButton, SIGNAL(pressed()), this, SLOT(addRouteSlot()));
    connect(ui->addRadarButton, SIGNAL(clicked()), this, SLOT(addRadarSlot()));
    connect(ui->deleteRouteButton, SIGNAL(clicked()), this, SLOT(deleteRouteSlot()));
    connect(ui->deleteRadarButton, SIGNAL(clicked()), this, SLOT(deleteRadarSlot()));
    connect(ui->changeRadarNameButton, SIGNAL(clicked()), this, SLOT(changeRadarNameSlot()));
    connect(ui->radarsList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(radarItemWasSelectedSlot(QListWidgetItem*)));
    connect(ui->routesList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(routeItemWasSelectedSlot(QListWidgetItem*)));
    connect(ui->changeSpeedButton, SIGNAL(clicked()), this, SLOT(updateSpeedSlot()));
    connect(ui->changeRouteNameButton, SIGNAL(clicked()), this, SLOT(updateRouteNameSlot()));
    connect(ui->tabWidget, SIGNAL(tabBarClicked(int)), this, SLOT(updatePlotTabSlot(int)));

    connect(ui->startPlotSequenceButton, SIGNAL(clicked()), this, SLOT(plotSequenceSlot()));
    connect(ui->runAnimationButton, SIGNAL(clicked()), this, SLOT(animationSequenceSlot()));

    /* ========================== SETTING UP MENU BUTTON ACTIONS ===================== */
    connect(ui->actionDimensions_of_area, SIGNAL(triggered()), this, SLOT(changeDimensionsSlot()));
    connect(ui->actionOptions, SIGNAL(triggered()), this, SLOT(changeOptionsSlot()));
    connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(aboutActionSlot()));

    /* ============================= OPTIONS SETTINGS ================================ */
    distortion = 0.01; // this is measured in meters = (1 cm)
    distortionType = 2; // id of radio button in dialog box !!!!!!!
    quantizationType = 1; // X > 0 stands for rounding quantization, 0 for chip quantization
    quantization = 1.0/13.0; // rounding noise effect to quantization specified, default for 13 GHz radar
    use_distortion = true;
}

void MainWindow::calculateRatios()
{
    /**
      * @brief This function will calculate ratio between the width/height of scene and width/height
      * set up by user in dialog. These ratios are important for calculating real
      * coordinates in metres in real sized area. Note that sceneRect is (initially in version 1.0)
      * 500.0 + radar picture height*2.
      *
      **/

    double radarPicHeight = radarList.at(0)->radarPicHeight()*2;

    this->widthRatio = (this->realAreaWidth/(this->monitoredAreaScene->sceneRect().width()-radarPicHeight));
    this->heightRatio = (this->realAreaHeight/(this->monitoredAreaScene->sceneRect().height()-radarPicHeight));
}

void MainWindow::addNewPoint(QGraphicsEllipseItem *p, QColor c)
{
    /**
      * @brief This function will automatically add new point into the currently
      * drawing chain. It will add new point while updating currentDrawing and
      * currentDrawingBase pointers. Recieved 'p' pointer is obtained from child object.
      *
      **/

      struct route *temp = new struct route;
      temp->next = NULL;
      temp->previous = currentDrawing;
      temp->point = NULL;
      temp->line = NULL;
      temp->speed = NULL;

      currentDrawing->next = temp;
      currentDrawing->point = p;

      // if we have second point, we need to draw a connection line
      if(currentDrawing->previous!=NULL)
           currentDrawing->line = monitoredAreaScene->addLine(currentDrawing->point->scenePos().rx(),
                                                              currentDrawing->point->scenePos().ry(),
                                                              currentDrawing->previous->point->scenePos().rx(),
                                                              currentDrawing->previous->point->scenePos().ry(), QPen(c));

      currentDrawing = temp;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete wall;
}

double MainWindow::makeDistortion(double coordinate)
{
    int diff1 = (double)(rand() % 100);
    double diff2 = 1.0;
    if(diff1>50) diff2 = -1.0;

    if(distortionType == 2)
    {
        // simple, uniform distribution

        return (((diff2)*this->distortion)+coordinate);
    }
    else if(distortionType == 3)
    {
        // gauss distribution

        std::normal_distribution<double> distribution(0.0, this->distortion);
        return (diff2*distribution(*this->generator)+coordinate);
    }
    else if(distortionType == 4)
    {
        // exponential distribution

        std::exponential_distribution<double> distribution(this->distortion);
        return (diff2*distribution(*this->generator)+coordinate);
    }
    else return 0.0; // no noise will be added

    return 0.0;
}

void MainWindow::removeDirRecursively(const QString &dir)
{
    QDir directory(dir);
    Q_FOREACH(QFileInfo info, directory.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
    {
        // recursively delete all files and dirs inside
        if (info.isDir()) this->removeDirRecursively(info.absoluteFilePath());
        else QFile::remove(info.absoluteFilePath());
    }
    directory.rmdir(dir);

    return;
}

double MainWindow::degToRads(double deg)
{
    // conversion to radians from degrees
    return (PI/180.0)*deg;
}

void MainWindow::generateSimulationFilesSlot()
{
    /**
      * @brief This slot will start generating new files which will contain
      * all coordinates of moving objects with time specified. Each radar has
      * own file and coordinates are calculated for the radar's perspective from
      * basic coordinates. Basic coordinates are directly coordinates for the
      * OPERATOR perspective.
      *
      * The structure of file is as the following:
      *
      * THE NUMBER OF SENSOR ->
      * TIME ->
      * NUMBER OF VISIBLE OBJECT ->
      * COORDINATES OF 1st, 2nd, 3rd, ... OBJECT IN ORDER X,Y INCLUDING LEFT AND RIGHT RECIEVER ANTENNA TOA (LEFT ANTENNA FIRST)
      *
      * The separator dividing these information is: #
      *
      * One special file is generated: sensors position. It contains the position
      * of radar in relation to OPERATOR. Sperarator used is: #
      *
      * The structure of this file is:
      *
      * BASIC_SCENE_SETTINGS
      * TARGETS_SPEED
      * ! (separator)
      * THE NUMBER OF SENSOR -> (0 index stands for OPERATOR)
      * X_POS ->
      * Y_POS ->
      * X_POS_LEFT_ANTENNA ->
      * Y_POS_LEFT_ANTENNA ->
      * X_POS_RIGHT_ANTENNA ->
      * Y_POS_RIGHT_ANTENNA ->
      * ANGLE
      *
      **/

    if(ui->routesList->count()==0)
    {
        // if no routes were drawn
        QMessageBox::information(this, tr("Empty routes list"), tr("There is no route to generate coordinate file from."), QMessageBox::Ok);
        return;
    }

    QTime currentTime = QTime::currentTime();
    QDate currentDate = QDate::currentDate();
    QString dateTimeString = currentTime.toString()+"_"+currentDate.toString();

    QString path = QDir::currentPath().append(QString("/Generated Files/%1").arg(dateTimeString).replace(":", "_").replace(" ", "_"));

    // Create dir where generated files can be stored

    QDir generatedFilesDir(path);
    if(!generatedFilesDir.exists())
        generatedFilesDir.mkpath("./");
    else {
        // if somehow such dir exists we will remove and start with new empty dir
        this->removeDirRecursively(generatedFilesDir.path());
    }

    // create a file where radar positions and angles in relation to OPERATOR will be stored
    QFile radarPositions(QString(path)+QString("/")+"setup.txt");
    radarPositions.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream radarPositionsStream(&radarPositions);

    int radarCount = 0;
    QList <QFile *> fileList; // list of all created files
    QList <class transformation2D *> radarTransformationObjects;

    // add basic info about settings into setup.txt file
    radarPositionsStream << "DIMENSIONS#" << this->realAreaWidth << "#" << this->realAreaHeight << endl;
    radarPositionsStream << "NOISE#" << this->use_distortion << endl;
    if(this->use_distortion)
    {
        switch(this->distortionType)
        {
            case 2:
                radarPositionsStream << "DISTRIBUTION#UNIFORM#" << this->distortion << endl;
                break;
            case 3:
                radarPositionsStream << "DISTRIBUTION#NORMAL#" << this->distortion << endl;
                break;
            case 4:
                radarPositionsStream << "DISTRIBUTION#EXPONENTIAL#" << this->distortion << endl;
                break;
            default:
                break;
        }
    }
    else radarPositionsStream << "DISTRIBUTION#NONE#0" << endl;
    radarPositionsStream << "REFRACTIVE_INDEX#" << this->refractiveIndex << endl;
    radarPositionsStream << "TOA_TO_DISTANCE_CONVERSION#" << this->toaMode << endl;
    radarPositionsStream << "SAMPLING_PERIOD#" << this->timeStep << endl;
    radarPositionsStream << "TARGETS_COUNT#" << routeList.count() << endl;
    radarPositionsStream << "TARGET_SPEEDS#" << this->timeStep;
    for(int i = 0; i<routeList.count(); i++) if(routeList.at(i) != NULL) radarPositionsStream << "#" << *routeList.at(i)->speed;
    radarPositionsStream << endl;
    radarPositionsStream << "!" << endl;

    for(int i = 0; i < this->radarList.count(); i++)
    {
        // create a text file for each of radar unit
        if(radarList.at(i) != NULL)
        {
            QString name(*radarList.at(i)->radarName());
            QFile *new_file = new QFile(QString(path)+QString("/")+name+".txt");
            new_file->open(QIODevice::WriteOnly | QIODevice::Text);
            fileList.append(new_file);

            // fill the radar positions/angle file with values
            radarPositionsStream << radarCount++ << "#" << this->radarList.at(i)->x()*widthRatio << "#"
                                 << this->radarList.at(i)->y()*heightRatio << "#"
                                 << (this->radarList.at(i)->x()*widthRatio - cos(degToRads(this->radarList.at(i)->realAngle()))*left_antenna_dist) << "#"
                                 << (this->radarList.at(i)->y()*heightRatio - sin(degToRads(this->radarList.at(i)->realAngle()))*left_antenna_dist) << "#"
                                 << (this->radarList.at(i)->x()*widthRatio + cos(degToRads(this->radarList.at(i)->realAngle()))*right_antenna_dist) << "#"
                                 << (this->radarList.at(i)->y()*heightRatio + sin(degToRads(this->radarList.at(i)->realAngle()))*right_antenna_dist) << "#"
                                 << this->radarList.at(i)->realAngle() << '\n';

            radarTransformationObjects.append(new transformation2D(0.0, 0.0, this->radarList.at(i)->x()*widthRatio, this->radarList.at(i)->y()*heightRatio, this->radarList.at(i)->realAngle()));
        }
    }

    QList <class routemath *> routesMath;
    int routesCount = 0;
    for(int j = 0; j < this->routeList.count(); j++)
    {
        // create a routemath object for each route
        if(this->routeList.at(j) != NULL)
        {
            routesCount++;
            routesMath.append(new routemath(this->routeList.at(j), *this->routeList.at(j)->speed, this->timeStep, this->widthRatio, this->heightRatio));
        }
    }

    bool finished = true;
    double * xy;
    double cur_time = 0.0;
    while(1==1)
    {
        Q_FOREACH(class routemath *route, routesMath)
        {
            // calculate new positions for targets
            xy = route->nextStep();
        }

        for(int z = 0; z < radarTransformationObjects.count(); z++)
        {
            QTextStream radarWrite(fileList.at(z));
            // insert the radar index (zero stands for OPERATOR), number of visible objects and current time
            radarWrite <<  z << "#" << cur_time; //<< "#" << routesCount;

            int target_counter = 0;
            Q_FOREACH(class routemath *route, routesMath)
            {
                if(!radarTransformationObjects.at(z)->isBehindRadar(route->getNewX(), route->getNewY())) target_counter++;
            }

            radarWrite << "#" << target_counter;
        }

        Q_FOREACH(class routemath *route, routesMath)
        {

            for(int z = 0; z < radarTransformationObjects.count(); z++)
            {
                // if target is behind the radar
                if(radarTransformationObjects.at(z)->isBehindRadar(route->getNewX(), route->getNewY())) continue;

                QTextStream radarWrite(fileList.at(z));
                radarTransformationObjects.at(z)->setUpOldCoordinates(route->getNewX(), route->getNewY());
                double x = radarTransformationObjects.at(z)->getX();
                double y = radarTransformationObjects.at(z)->getY();

                // add distortion
                if(this->use_distortion)
                {
                    x = this->makeDistortion(x);
                    y = this->makeDistortion(y);
                }    

                double TOA1rounded = this->quantizeNumber((this->getTOA1(x, y)/(speedOfLight*refractiveIndex))*nanosec);
                double TOA2rounded = this->quantizeNumber((this->getTOA2(x, y)/(speedOfLight*refractiveIndex))*nanosec);

                radarWrite << "#" << x << "#" << y;

                if(toaMode) radarWrite << "#" << (TOA1rounded/nanosec)*(speedOfLight*refractiveIndex) << "#" << (TOA2rounded/nanosec)*(speedOfLight*refractiveIndex); // write distance
                else radarWrite << "#" << TOA1rounded << "#" << TOA2rounded; // write in nanoseconds
            }

            if(!route->end()) finished = false;
        }

        for(int z = 0; z < radarTransformationObjects.count(); z++)
        {
            QTextStream radarWrite(fileList.at(z));
            // insert the end of line into each file
            radarWrite <<  endl;
        }

        cur_time += this->timeStep;

        if(finished) break;
        finished = true;
    }

    // close files and free memory related to this function
    for(int j = 0; j < fileList.count(); j++) fileList.at(j)->close();
    fileList.clear();
    routesMath.clear();
    delete xy;


    QMessageBox::information(this, tr("Success"), tr("All files are generated."), QMessageBox::Ok);
}

void MainWindow::radarItemWasClickedSlot()
{
    /**
      * @brief This slot ensures that 'selectedRadar' pointer will be
      * pointing to the last clicked radar item so all information
      * about specific radar (angle, coordinates, index, ...) will
      * be correctly shown.
      *
      **/

    this->hideLastPanel(); // if displayed panel was for ex. routePanel etc.
    if(selectedRoute) { routeItemWasSelectedSlot(NULL); } // if any route was selected, now unselect
    this->setNewPanel(ui->radarPanel);
    selectedRadar = dynamic_cast<radarItem *>(sender());
    ui->radarPanel->setTitle(*selectedRadar->radarName());
    ui->RadarAngleLabelVal->setText(tr("%1").arg((-1.0)*selectedRadar->currentAngle()));
    ui->RadarPositionXLabelVal->setText(tr("%1").arg(selectedRadar->pos().rx()));
    ui->RadarPositionYLabelVal->setText(tr("%1").arg(selectedRadar->pos().ry()));

    // we cannot rotate OPERATOR radar as the base coordinate system
    if(!QString::compare(QString("OPERATOR"), *selectedRadar->radarName(), Qt::CaseInsensitive))
    {
        // if the selected radar is OPERATOR
        ui->rotateMinusButton->setDisabled(true);
        ui->rotatePlusButton->setDisabled(true);
        ui->rotationLabel->setDisabled(true);
        ui->changeRadarNameButton->setDisabled(true);

    }
    else
    {
        // if another radar is selected
        if(!ui->rotateMinusButton->isEnabled()) ui->rotateMinusButton->setDisabled(false);
        if(!ui->rotatePlusButton->isEnabled()) ui->rotatePlusButton->setDisabled(false);
        if(!ui->rotationLabel->isEnabled()) ui->rotationLabel->setDisabled(false);
        if(!ui->changeRadarNameButton->isEnabled()) ui->changeRadarNameButton->setDisabled(false);
    }

    if(ui->radarPanel->isHidden()) ui->radarPanel->setHidden(false);
}

void MainWindow::radarItemWasMovedSlot()
{
    /**
      * This function will update the position of radar each time it emitted signal
      * indicating that user moved with the radar.
      *
      **/

    ui->RadarPositionXLabelVal->setText(tr("%1").arg(selectedRadar->pos().rx()));
    ui->RadarPositionYLabelVal->setText(tr("%1").arg(selectedRadar->pos().ry()));
}

void MainWindow::rotateRadarUnClockwiseSlot()
{
    if(selectedRadar!=NULL)
    {
        if(selectedRadar->currentAngle()<=45.0)
        {
            float currentItemAngle = selectedRadar->rotation();
            float currentRadarAngle = selectedRadar->currentAngle();
            selectedRadar->setAngle(currentRadarAngle + 5.0);
            selectedRadar->setAngleOfItem(currentItemAngle + 5.0);
            ui->RadarAngleLabelVal->setText(tr("%1").arg((-1.0)*(currentRadarAngle + 5.0)));
            if(((-1.0)*(currentRadarAngle + 5.0))>45.0) {}
        }
    }
}

void MainWindow::deleteRouteSlot()
{
    /**
      * @brief This slot provides interfase for deleting the selected
      * route from the list.
      *
      **/

    if(ui->routesList->currentItem())
    {
        if(QMessageBox::question(NULL, tr("Deleting route: %1").arg(ui->routesList->currentItem()->text()),
                                 tr("Are you sure, you want to delete the selected route?"))==QMessageBox::Yes)
        {
            // deleting all objects
            this->deleteRoute(static_cast<struct route *>(routeList.at(ui->routesList->currentItem()->data(Qt::UserRole).toInt())));
            delete routeList.at(ui->routesList->currentItem()->data(Qt::UserRole).toInt());
            routeList.replace(ui->routesList->currentItem()->data(Qt::UserRole).toInt(), NULL);
            delete ui->routesList->currentItem();
            selectedRoute = NULL; // reseting last selected route (now the selected route is no more existing)
            this->hideLastPanel(); // ensures that user cannot manipulate the data that do not exist now
        }
    }
    else
    {
        QMessageBox::warning(NULL, tr("No item selected"),
                             tr("No item was selected yet. Please select the route to delete."),
                             QMessageBox::Ok);
    }
}

void MainWindow::deleteRadarSlot()
{
    /**
      * @brief This slot will remove the selected radar in the radarsList.
      * Note: OPERATOR RADAR will always have index 0 in the radarsList as
      * well as in radarList and cannot be removed.
      *
      **/

    if(ui->radarsList->currentItem())
    {
        if(ui->radarsList->currentItem()->data(Qt::UserRole).toInt()==0)
        {
            // OPERATOR RADAR
            QMessageBox::warning(NULL, tr("OPERATOR RADAR was selected."),
                                 tr("Operator radar cannot be deleted."),
                                 QMessageBox::Ok);
        }
        else
        {
            delete radarList.at(ui->radarsList->currentItem()->data(Qt::UserRole).toInt());
            radarList.replace(ui->radarsList->currentItem()->data(Qt::UserRole).toInt(), NULL);
            delete ui->radarsList->currentItem();
            this->hideLastPanel(); // ensures that user cannot modify data that do not exist now
        }
    }
    else
    {
        QMessageBox::warning(NULL, tr("No item selected"),
                             tr("No item was selected yet. Please select the radar to delete."),
                             QMessageBox::Ok);
    }
}


void MainWindow::rotateRadarClockwiseSlot()
{
    if(selectedRadar!=NULL)
    {
        if(selectedRadar->currentAngle()>=(-45.0))
        {
            float currentItemAngle = selectedRadar->rotation();
            float currentRadarAngle = selectedRadar->currentAngle();
            selectedRadar->setAngle(currentRadarAngle - 5.0);
            selectedRadar->setAngleOfItem(currentItemAngle - 5.0);
            ui->RadarAngleLabelVal->setText(tr("%1").arg((-1.0)*(currentRadarAngle - 5.0)));
            if(((-1.0)*(currentRadarAngle - 5.0))>45.0) {}
        }
    }
}

void MainWindow::addRouteSlot()
{
    /**
     * @brief When this button is pressed, the user is allowed to draw points
     * and lines into monitoredAreaScene and creating new route of moving
     * object. The variable: drawingMode of MainWindow is used to determine
     * whether the mode is turned on/off. This variable is then used by child
     * objects to know about mode status.
     */

    QPushButton *pushedButton = dynamic_cast<QPushButton *>(sender());

    if(!drawingModeIsEnabled())
    {
        // initialization of new chain of route points
        currentDrawing = new struct route;
        currentDrawingBase = currentDrawing;
        // first structure of route chain must have information about the speed of moving object
        currentDrawingBase->speed = new double;
        *currentDrawingBase->speed = defaultSpeed;
        currentDrawing->next = currentDrawing->previous = NULL;
        currentDrawing->point = NULL;
        currentDrawing->line = NULL;

        // setting up buttons
        pushedButton->setText(tr("Finish drawing route"));
        setDrawingMode(true);
        this->cursor().setShape(Qt::PointingHandCursor);
    }
    else
    {
        if(currentDrawing->next == NULL && currentDrawing->previous == NULL)
        {
            // nothing was drawn
            delete currentDrawing;
            QMessageBox::information(NULL, tr("Nothing was drawn"), tr("Any new route was detected."), QMessageBox::Ok);
        }
        else
        {
            // the route is added to the list of routes
            routeList.append(currentDrawingBase);
            // adding new item to the list
            int count = routeList.count(); // number of routes so far
            QListWidgetItem *newItem = new QListWidgetItem(tr("Route %1").arg(count-1));
            newItem->setData(Qt::UserRole, (count-1)); // later we will have a reference as index of route in routelist
            ui->routesList->addItem(newItem);

            // finally the last, empty part is deleted
            currentDrawing->previous->next = NULL;
            delete currentDrawing;
        }

        pushedButton->setText(tr("Add new route"));
        monitoredAreaScene->refreshNewRoute();
        setDrawingMode(false);
        this->cursor().setShape(Qt::ArrowCursor);
    }
}

void MainWindow::addRadarSlot()
{
    /**
      * @brief This slot is responsible for adding new radar item in the
      * scene. The new radar is then displayed by a radar icon in the scene
      * and in the radar list.
      *
      **/

    int rdItems = radarList.count();

    radarItem *rd = new radarItem(tr("Radar %1").arg(rdItems));
    rd->setFlags(QGraphicsItemGroup::ItemIsMovable);
    monitoredAreaScene->addItem(rd);
    radarList.append(rd);

    ui->radarsList->addItem(tr("Radar %1").arg(rdItems));
    int rdItemsInList = ui->radarsList->count()-1; // the last added has last ID (zero is OPERATOR)
    ui->radarsList->item(rdItemsInList)->setTextColor(Qt::blue);
    ui->radarsList->item(rdItemsInList)->setData(Qt::UserRole, rdItems);

    connect((radarItem *)(rd), SIGNAL(radarItemIsClickedSignal()), this, SLOT(radarItemWasClickedSlot()));
    connect((radarItem *)(rd), SIGNAL(radarItemIsMovedSignal()), this, SLOT(radarItemWasMovedSlot()));
}

void MainWindow::changeRadarNameSlot()
{
    /**
      * @brief This slot will create new dialog where the user can
      * change the radar name. All procedure is done in changeRadarNameDialog.
      *
      **/

    changeRadarNameDialog *dialogWindow = new changeRadarNameDialog(this, this->selectedRadar);
    connect(dialogWindow, SIGNAL(closeThisWindowWithAccept()), this, SLOT(updateRadarListWidgetSlot()));
    connect(dialogWindow, SIGNAL(closeThisWindowWithAccept()), this, SLOT(updateRadarNamePanel()));
    dialogWindow->exec();
}

void MainWindow::updateRadarNamePanel(void)
{
    /**
      * @brief This function will only update radar name in left panel.
      *
      **/

    ui->radarPanel->setTitle(*selectedRadar->radarName());
}

void MainWindow::updateRadarListWidgetSlot(void)
{
    /**
      * @brief This function will go through all pointers of radarList
      * and find out at which index the selected radar lies. Then find
      * this index in radars list data and update the radar name there.
      *
      **/

    int num = radarList.count();
    int index = 0;
    for(int i=0; i<num; i++) {
        if(radarList.at(i)==selectedRadar)
        {
            index=i;
            break;
        }
        else continue;
    }
    ui->radarsList->item(index)->setText(*selectedRadar->radarName());
}
void MainWindow::radarItemWasSelectedSlot(QListWidgetItem *i)
{
    /**
      * @brief This slot is ensuring that when user clicks the item in
      * the list of radars, on the left panel automatically all information
      * will update similarly as the item was clicked in the scene directly.
      *
      **/

    int indexRadarSelected = i->data(Qt::UserRole).toInt();
    emit radarList.at(indexRadarSelected)->radarItemIsClickedSignal();
}

void MainWindow::routeItemWasSelectedSlot(QListWidgetItem*i)
{
    /**
      * @brief This slot is ensuring that when user clicks the item in
      * the list of routes, on the left panel automatically all information
      * will update for specific route.
      *
      **/

    if(i==NULL)
    {
        /**
         * This condition was added because, when radar item is clicked, it is
         * better to unselect previously selected route.
         *
         **/

        setRouteColor(selectedRoute, lastColor, 1);
        selectedRoute = NULL;

    }
    else
    {
        int indexRouteSelected = i->data(Qt::UserRole).toInt();
        if(selectedRoute!=NULL) setRouteColor(selectedRoute, lastColor, 1); // if it is not NULL (first select)
        selectedRoute = routeList.at(indexRouteSelected);
        lastColor = selectedRoute->point->brush().color();
        setRouteColor(selectedRoute, Qt::black, 3);
        this->hideLastPanel();
        this->setNewPanel(ui->routePanel);

        ui->routePanel->setTitle(i->text());

        double x1 = selectedRoute->point->x();
        double y1 = selectedRoute->point->y();
        struct route *last = lastPoint(selectedRoute);
        double x2 = last->point->x();
        double y2 = last->point->y();

        ui->RouteStartLabelVal->setText(tr("%1 x %2").arg(x1).arg(y1));
        ui->RouteEndLabelVal->setText(tr("%1 x %2").arg(x2).arg(y2));

        // update displayed speed
        ui->speedVal->setText(tr("%1").arg(*selectedRoute->speed));

        if(ui->routePanel->isHidden()) ui->routePanel->setHidden(false);
    }
}

struct route *MainWindow::lastPoint(struct route *startPoint)
{
    /**
      * @brief This function will return the last point of selected route.
      *
      **/
    struct route *temp = startPoint;
    while(temp->next) temp = temp->next;
    return temp;
}

void MainWindow::setRouteColor(struct route *startPoint, QColor newColor, int width)
{
    /**
      * @brief This function will change color of given route. Width parameter specifies
      * the width line objects in pixels
      *
      **/
    QPen pen(newColor, width);
    struct route *temp = startPoint;
    while(temp->next)
    {
        if(temp->line!=NULL) temp->line->setPen(pen); // first point does not have line object
        temp->point->setBrush(QBrush(newColor));
        temp->point->setPen(pen);
        temp = temp->next;
    }
    if(temp->line!=NULL) temp->line->setPen(QPen(pen)); // in situation when route is only one point
    temp->point->setBrush(QBrush(newColor));
    temp->point->setPen(pen);
}

void MainWindow::updateSpeedSlot(void)
{
    bool ok = false;
    QInputDialog getVal;
    getVal.setOption(QInputDialog::NoButtons);
    double val = getVal.getDouble(this, tr("Change speed"), tr("Speed: "), *selectedRoute->speed, 0.01, 10.0, 2, &ok);
    if(ok) { *selectedRoute->speed = val; ui->speedVal->setText(tr("%1").arg(val));}
}

void MainWindow::updateRouteNameSlot(void)
{
    bool ok = false;
    QInputDialog getVal;
    getVal.setOption(QInputDialog::NoButtons);
    QString val = getVal.getText(this, tr("Change name"), tr("Speed: "), QLineEdit::Normal, ui->routesList->currentItem()->text(), &ok);
    if(ok) { ui->routesList->currentItem()->setText(val); ui->routePanel->setTitle(tr("Route name: %1").arg(val));}
}

void MainWindow::changeDimensionsSlot()
{
    class changeDimensionsOfWorkArea dialogWin(this);

    connect(&dialogWin, SIGNAL(closeThisWindowWithAccept(double, double)), this, SLOT(updateDimensionsSlot(double, double)));

    dialogWin.exec();
}

void MainWindow::updateDimensionsSlot(double width, double height)
{
    this->realAreaWidth = width;
    this->realAreaHeight = height;

    this->calculateRatios();
}

void MainWindow::changeOptionsSlot(void)
{
    class options optionsDialog(this);

    optionsDialog.exec();
}

void MainWindow::aboutActionSlot()
{
    /**
      * @brief Displaying about dialog with information about the software.
      *
      **/

    class aboutDialogCl * aboutW = new aboutDialogCl(this);

    aboutW->exec();
}

void MainWindow::updatePlotTabSlot(int index)
{
    /**
      * @brief This function will update QListWidgets in plotter tab which will be used
      * to select radar / route to plot in graph. These widgets will contain the same
      * items as widgets in main tab.
      *
      **/

    if(index!=1) return;
    else {
        // deleting old records
        while(ui->routesPlotList->count() > 0) ui->routesPlotList->takeItem(0);
        while(ui->radarsPlotList->count() > 0) ui->radarsPlotList->takeItem(0);

        for(int a = 0; a < ui->routesList->count(); a++) ui->routesPlotList->addItem(ui->routesList->item(a)->clone());
        for(int a = 0; a < ui->radarsList->count(); a++) ui->radarsPlotList->addItem(ui->radarsList->item(a)->clone());
    }
}

void MainWindow::plotSequenceSlot()
{
    /**
      * @brief This function checks for selected radar and route. Then starts to compute the values
      * for items specified and saving them to QVector. The result is plotted into QCustomPlot.
      *
      * It is possible to select more routes and plot them
      *
      **/

    if(!ui->radarsPlotList->currentItem() || !ui->routesPlotList->currentItem())
    {
        QMessageBox::information(this, tr("Radar or route is not selected"), tr("To plot route you must select the radar and the route to start plotting sequence"), QMessageBox::Ok);
        return;
    }


    ui->routePlot->clearGraphs();
    ui->routePlot->clearPlottables();
    ui->routePlot->clearItems();

    this->clearPlotsObjects();

    // get selected radar
    radarItem * selectedRadar = this->radarList.at(ui->radarsPlotList->currentItem()->data(Qt::UserRole).toInt());
    // create object for transformation of coordinates for radar selected
    transformation2D selectedRadarTransformation(0.0, 0.0, selectedRadar->x()*widthRatio, selectedRadar->y()*heightRatio, selectedRadar->realAngle());


    QList<QListWidgetItem *> selectedRoutes = ui->routesPlotList->selectedItems();

    double * xy;

    Q_FOREACH(QListWidgetItem *route, selectedRoutes)
    {
       if(this->routeList.at(route->data(Qt::UserRole).toInt())!=NULL) {


            QVector <double> xVector, yVector;

            class routemath coordinateCalculation(this->routeList.at(route->data(Qt::UserRole).toInt()), *this->routeList.at(route->data(Qt::UserRole).toInt())->speed,
                                                                    this->timeStep, this->widthRatio, this->heightRatio);
            while(!coordinateCalculation.end())
            {
                // go over all route structers and calculate the object position
                xy = coordinateCalculation.nextStep();
                selectedRadarTransformation.setUpOldCoordinates(xy[0], xy[1]);

                double x = selectedRadarTransformation.getX();
                double y = selectedRadarTransformation.getY();


                if(this->use_distortion)
                {
                    x = this->makeDistortion(x);
                    y = this->makeDistortion(y);
                }

                // if target is behind radar
                if(y<0.0)
                {
                    // if last route was already drawn
                    if(!beforeHideDrawn)
                    {
                        // plot lastly computed route
                        QCPCurve * moving_obj = new QCPCurve(ui->routePlot->xAxis, ui->routePlot->yAxis);
                        moving_obj->setPen(QPen(this->routeList.at(route->data(Qt::UserRole).toInt())->point->brush().color()));
                        moving_obj->setData(xVector, yVector);

                        ui->routePlot->addPlottable(moving_obj);

                        xVector.clear();
                        yVector.clear();
                    }
                    beforeHideDrawn = true;
                    continue;
                }

                beforeHideDrawn = false;
                xVector.append(x);
                yVector.append(y);
            }

            // plot lastly computed route
            QCPCurve * moving_obj = new QCPCurve(ui->routePlot->xAxis, ui->routePlot->yAxis);
            moving_obj->setPen(QPen(this->routeList.at(route->data(Qt::UserRole).toInt())->point->brush().color()));
            moving_obj->setData(xVector, yVector);

            ui->routePlot->addPlottable(moving_obj);
       }
    }

    // plot area borders, vectors starts clockwisely from left down corner
    //QCPCurve * borders = new QCPCurve(ui->routePlot->xAxis, ui->routePlot->yAxis);
    //borders->setPen(QPen(QBrush(QColor(0,0,0)), 3.0));
    //borders->setBrush(QBrush(QColor(0,0,0, 35)));
    QVector <double> xBorderVector, yBorderVector;

    double radarPicHeight = radarList.at(0)->radarPicHeight()*2;
    double sceneWidth = this->monitoredAreaScene->sceneRect().width() - radarPicHeight;
    double sceneHeight = this->monitoredAreaScene->sceneRect().height() - radarPicHeight;

    selectedRadarTransformation.setUpOldCoordinates(((-1.0)*sceneWidth/2.0)*this->widthRatio, 0.0);
    xBorderVector.append(selectedRadarTransformation.getX());
    yBorderVector.append(selectedRadarTransformation.getY());

    selectedRadarTransformation.setUpOldCoordinates((sceneWidth/2.0)*this->widthRatio, 0.0);
    xBorderVector.append(selectedRadarTransformation.getX());
    yBorderVector.append(selectedRadarTransformation.getY());

    selectedRadarTransformation.setUpOldCoordinates((sceneWidth/2.0)*this->widthRatio, sceneHeight*this->heightRatio);
    xBorderVector.append(selectedRadarTransformation.getX());
    yBorderVector.append(selectedRadarTransformation.getY());

    selectedRadarTransformation.setUpOldCoordinates(((-1.0)*sceneWidth/2.0)*this->widthRatio, sceneHeight*this->heightRatio);
    xBorderVector.append(selectedRadarTransformation.getX());
    yBorderVector.append(selectedRadarTransformation.getY());

    selectedRadarTransformation.setUpOldCoordinates(((-1.0)*sceneWidth/2.0)*this->widthRatio, 0.0);
    xBorderVector.append(selectedRadarTransformation.getX());
    yBorderVector.append(selectedRadarTransformation.getY());

    //borders->setData(xBorderVector, yBorderVector);
    for(int i = 0; i<xBorderVector.count(); i++)
    {
        QCPItemLine * line = new QCPItemLine(ui->routePlot);
        line->start->setCoords(xBorderVector.at(i), yBorderVector.at(i));
        if((i+1)==xBorderVector.count()) line->end->setCoords(xBorderVector.at(0), yBorderVector.at(0));
        else line->end->setCoords(xBorderVector.at(i+1), yBorderVector.at(i+1));
        line->setPen(QPen(QBrush(Qt::black), 2.0));
        ui->routePlot->addItem(line);
    }

    this->configureXYAxisTicks();

    ui->routePlot->replot();
}

void MainWindow::clearPlotsObjects(void)
{
    // deleting all objects in backup lists in case that some of them still exists from previous animation
    while(routeMathAnimationBackup.count() > 0)
    {
        delete routeMathAnimationBackup.at(0);
        routeMathAnimationBackup.removeAt(0);
    }
    while(routeAnimationCircleBackup.count() > 0)
    {
        delete routeAnimationCircleBackup.at(0);
        routeAnimationCircleBackup.removeAt(0);
    }
    while(routeAnimationEllipseLeftBackup.count() > 0)
    {
        delete routeAnimationEllipseLeftBackup.at(0);
        routeAnimationEllipseLeftBackup.removeAt(0);
    }
    while(routeAnimationEllipseRightBackup.count() > 0)
    {
        delete routeAnimationEllipseRightBackup.at(0);
        routeAnimationEllipseRightBackup.removeAt(0);
    }

    if(leftAntenna!=NULL)
    {
        delete leftAntenna;
        leftAntenna = NULL;
    }
    if(rightAntenna!=NULL)
    {
        delete rightAntenna;
        rightAntenna = NULL;
    }
}

void MainWindow::configureXYAxisTicks()
{
    /**
      * @brief This function set constant xy axis ticks and prevents auto rescale ratio of xy axis
      *
      **/


    if(realAreaWidth>realAreaHeight)
    {
        ui->routePlot->xAxis->rescale();

        // when rescaled, increment the border ranges for better estethic view (to arise the distance between border range and points)
        ui->routePlot->xAxis->setRange(ui->routePlot->xAxis->range().lower, ui->routePlot->xAxis->range().upper);
        ui->routePlot->yAxis->setRange(0.0, ui->routePlot->xAxis->range().upper*2.0);
    }
    else {
        ui->routePlot->yAxis->rescale();

        // when rescaled, increment the border ranges for better estethic view (to arise the distance between border range and points)
        ui->routePlot->xAxis->setRange((-1.0)*(ui->routePlot->yAxis->range().upper)/2.0, ui->routePlot->yAxis->range().upper/2.0);
        ui->routePlot->yAxis->setRange(0.0, ui->routePlot->yAxis->range().upper);
    }

    ui->routePlot->xAxis->setTickStep(1.0);

    ui->routePlot->yAxis->setTickStep(1.0);
}

double MainWindow::quantizeNumber(double num)
{
    /**
      * @brief This function will take the number passed in argument and
      * do the quantization algorithm. Its return value will be rounded
      * due to quantization parameter specified.
      *
      **/

    qDebug() << "Considered num: " << num << " quantizer: " << this->quantization;

    double productor = 1000.0; // we will use 3 decimals of quantization number -> x decimals use productor=10^x
    int quantization_parameter = (double)(quantization*productor);
    int considered_val = (double)(num*productor); // remove redundant part of number

    int error_val = considered_val%quantization_parameter; // getting remanet
    int diff_error_val = quantization_parameter - error_val;

    if((double)(error_val) >= (double)((double)(quantization_parameter)/2.0))
    {
        // number is closer to upper multiple of quantization parameter
        considered_val += diff_error_val;
    }
    else
    {
        // number is closer to lower multiple of quantization parameter
        considered_val -= error_val;
    }

    // converting back to double in metres
    double quantized_val = (double)(considered_val)/productor;

    qDebug() << "Rounded: " << quantized_val;

    return quantized_val;
}

void MainWindow::animationSequenceSlot()
{
    /**
      * @brief This is the animation sequence. It will run the functions
      * needed for calculation new coordinates and create QCPItemCircle
      * objects to move the in specified time intervals according these
      * values.
      *
      **/

    if(!this->animationRunning) {

        // ANIMATION IS NOT RUNNING YET

            if(!ui->radarsPlotList->currentItem() || !ui->routesPlotList->currentItem())
            {
                QMessageBox::information(this, tr("Radar or route is not selected"), tr("To plot route you must select the radar and the route to start plotting sequence"), QMessageBox::Ok);
                return;
            }


            ui->routePlot->clearGraphs();
            ui->routePlot->clearPlottables();
            ui->routePlot->clearItems();

            this->clearPlotsObjects();

            // get selected radar
            radarItem * selectedRadar = this->radarList.at(ui->radarsPlotList->currentItem()->data(Qt::UserRole).toInt());
            // create object for transformation of coordinates for radar selected
            if(this->radarAnimationBackup!=NULL) delete this->radarAnimationBackup;

            this->radarAnimationBackup = new transformation2D(0.0, 0.0, selectedRadar->x()*widthRatio, selectedRadar->y()*heightRatio, selectedRadar->realAngle());


            QList<QListWidgetItem *> selectedRoutes = ui->routesPlotList->selectedItems();
            this->routeMathAnimationBackup.clear(); // delete all previous objects if some exist
            this->routeAnimationCircleBackup.clear(); // delete all previous objects if some exist
            this->routeAnimationEllipseLeftBackup.clear();
            this->routeAnimationEllipseRightBackup.clear();

            Q_FOREACH(QListWidgetItem *route, selectedRoutes)
            {
               if(this->routeList.at(route->data(Qt::UserRole).toInt())!=NULL) {
                    // create new calculation objects
                    this->routeMathAnimationBackup.append(
                    new routemath(this->routeList.at(route->data(Qt::UserRole).toInt()), *this->routeList.at(route->data(Qt::UserRole).toInt())->speed,
                                                                            this->timeStep, this->widthRatio, this->heightRatio));
                    this->routeAnimationCircleBackup.append(new QCPItemCircle(ui->routePlot, 0.0, 0.0, (this->monitoredAreaScene->sceneRect().width()*this->widthRatio)/100.0,
                                                                              this->routeList.at(route->data(Qt::UserRole).toInt())->point->brush().color()));
                    if(drawEllipses)
                    {
                        // if drawing ellipses is enabled
                        this->routeAnimationEllipseLeftBackup.append(new QCPItemEllipse(ui->routePlot));
                        this->routeAnimationEllipseLeftBackup.last()->setPen(QPen(this->routeList.at(route->data(Qt::UserRole).toInt())->point->brush().color()));
                        this->routeAnimationEllipseRightBackup.append(new QCPItemEllipse(ui->routePlot));
                        this->routeAnimationEllipseRightBackup.last()->setPen(QPen(this->routeList.at(route->data(Qt::UserRole).toInt())->point->brush().color()));
                    }
               }
            }

            if(drawEllipses)
            {
                // displaying antennas position
                leftAntenna = new QCPItemRect(ui->routePlot);
                rightAntenna = new QCPItemRect(ui->routePlot);

                double widthOfMarker = (this->monitoredAreaScene->sceneRect().width()*this->widthRatio)/100.0;

                leftAntenna->topLeft->setCoords(QPointF((-1.0)*left_antenna_dist-widthOfMarker, widthOfMarker));
                leftAntenna->bottomRight->setCoords(QPointF((-1.0)*left_antenna_dist+widthOfMarker, (-1.0)*widthOfMarker));

                rightAntenna->topLeft->setCoords(QPointF(right_antenna_dist-widthOfMarker, widthOfMarker));
                rightAntenna->bottomRight->setCoords(QPointF(right_antenna_dist+widthOfMarker, (-1.0)*widthOfMarker));

                leftAntenna->setBrush(QBrush(Qt::red));
                rightAntenna->setBrush(QBrush(Qt::red));
            }

            // setting and starting new timer according to timeStepSpecified (real time animation)
            this->animationTimer = new QTimer;
            connect(animationTimer, SIGNAL(timeout()), this, SLOT(animationSequenceNextStepSlot()));
            this->animationTimer->start(this->timeStep*1000);

            ui->runAnimationButton->setText(tr("Stop animation"));
            this->animationRunning = true;

            // plot area borders, vectors starts clockwisely from left down corner
            //QCPCurve * borders = new QCPCurve(ui->routePlot->xAxis, ui->routePlot->yAxis);
            //borders->setPen(QPen(QBrush(QColor(0,0,0)), 3.0));
            //borders->setBrush(QBrush(QColor(0,0,0, 35)));
            QVector <double> xBorderVector, yBorderVector;

            double radarPicHeight = radarList.at(0)->radarPicHeight()*2;
            double sceneWidth = this->monitoredAreaScene->sceneRect().width() - radarPicHeight;
            double sceneHeight = this->monitoredAreaScene->sceneRect().height() - radarPicHeight;

            radarAnimationBackup->setUpOldCoordinates(((-1.0)*sceneWidth/2.0)*this->widthRatio, 0.0);
            xBorderVector.append(radarAnimationBackup->getX());
            yBorderVector.append(radarAnimationBackup->getY());

            radarAnimationBackup->setUpOldCoordinates((sceneWidth/2.0)*this->widthRatio, 0.0);
            xBorderVector.append(radarAnimationBackup->getX());
            yBorderVector.append(radarAnimationBackup->getY());

            radarAnimationBackup->setUpOldCoordinates((sceneWidth/2.0)*this->widthRatio, sceneHeight*this->heightRatio);
            xBorderVector.append(radarAnimationBackup->getX());
            yBorderVector.append(radarAnimationBackup->getY());

            radarAnimationBackup->setUpOldCoordinates(((-1.0)*sceneWidth/2.0)*this->widthRatio, sceneHeight*this->heightRatio);
            xBorderVector.append(radarAnimationBackup->getX());
            yBorderVector.append(radarAnimationBackup->getY());

            radarAnimationBackup->setUpOldCoordinates(((-1.0)*sceneWidth/2.0)*this->widthRatio, 0.0);
            xBorderVector.append(radarAnimationBackup->getX());
            yBorderVector.append(radarAnimationBackup->getY());

            //borders->setData(xBorderVector, yBorderVector);
            for(int i = 0; i<xBorderVector.count(); i++)
            {
                QCPItemLine * line = new QCPItemLine(ui->routePlot);
                line->start->setCoords(xBorderVector.at(i), yBorderVector.at(i));
                if((i+1)==xBorderVector.count()) line->end->setCoords(xBorderVector.at(0), yBorderVector.at(0));
                else line->end->setCoords(xBorderVector.at(i+1), yBorderVector.at(i+1));
                line->setPen(QPen(QBrush(Qt::black), 2.0));
                ui->routePlot->addItem(line);
            }

            //ui->routePlot->addPlottable(borders);

            configureXYAxisTicks();
    }
    else {
        // ANIMATION IS RUNNING, STOPPING THE ANIMATION

            this->animationTimer->stop();
            delete this->animationTimer; this->animationTimer = NULL;

            delete this->radarAnimationBackup; this->radarAnimationBackup = NULL;

            // setting initial values
            ui->runAnimationButton->setText(tr("Run animation"));

            this->animationRunning = false;
    }

}

void MainWindow::animationSequenceNextStepSlot()
{
    /**
      * @brief The following function will go over all routemath object in
      * backup list and calculate new coordinates. As far as this list was
      * generated exatctly same way as circle object list, indexes are same
      * for both. If user wants, algorithm is able to simulate observation
      * of two antennas situated one on the left and one on the right side
      * of transmitter.
      *
      **/

    bool allFinished = true; // initially set to true. If only one is not at end of route, this is set to false
    double * xy;

    for(int i = 0; i < this->routeMathAnimationBackup.count(); i++)
    {
        xy = routeMathAnimationBackup.at(i)->nextStep();

        radarAnimationBackup->setUpOldCoordinates(xy[0], xy[1]);

        double x = radarAnimationBackup->getX();
        double y = radarAnimationBackup->getY();

        // target is behind the radar
        if(y<0.0)
        {
            routeAnimationCircleBackup.at(i)->setVisible(false);
            if(drawEllipses)
            {
                routeAnimationEllipseLeftBackup.at(i)->setVisible(false);
                routeAnimationEllipseRightBackup.at(i)->setVisible(false);
            }
        }
        else
        {
            routeAnimationCircleBackup.at(i)->setVisible(true);
            if(drawEllipses)
            {
                routeAnimationEllipseLeftBackup.at(i)->setVisible(true);
                routeAnimationEllipseRightBackup.at(i)->setVisible(true);
            }
        }


        // add distortion
        if(this->use_distortion)
        {
            x = this->makeDistortion(x);
            y = this->makeDistortion(y);
        }

        routeAnimationCircleBackup.at(i)->centerOn(x, y);
        if(drawEllipses)
        {
            // draw ellipses
            this->centerEllipseItems(x, y, i);

        }

        // check if is at end
        if(!routeMathAnimationBackup.at(i)->end()) allFinished = false;
    }

    ui->routePlot->replot();
    // automatically runs slot to stop animation
    if(allFinished) this->animationSequenceSlot();
}

void MainWindow::centerEllipseItems(double x, double y, int index)
{
    /**
      * @brief Function sets new values for QCPItemEllipse objects and sets up corners
      * so ellipses can be drawn into the plot during animation sequence.
      *
      **/

    // we suppose that reciever antennas are on the same line with transmitter antenna
    double TOA1 = this->getTOA1(x, y);
    double TOA2 = this->getTOA2(x, y);

    // ellipse for left antenna
    double TOA1Half = TOA1/2.0;
    double DistanceTRHalf = left_antenna_dist/2.0;
    double leftX = ((-1.0)*DistanceTRHalf) - TOA1Half;
    double leftY = sqrt(pow(TOA1Half, 2.0) - pow(DistanceTRHalf, 2.0));
    double rightY = (-1.0)*leftY;
    double rightX = leftX + TOA1;

    this->routeAnimationEllipseLeftBackup.at(index)->topLeft->setCoords(QPointF(leftX, leftY));
    this->routeAnimationEllipseLeftBackup.at(index)->bottomRight->setCoords(QPointF(rightX, rightY));

    // ellipse for right antenna

    double TOA2Half = TOA2/2.0;
    DistanceTRHalf = right_antenna_dist/2.0;
    rightX = DistanceTRHalf + TOA2Half;
    leftY = sqrt(pow(TOA2Half, 2.0) - pow(DistanceTRHalf, 2.0));
    rightY = (-1.0)*leftY;
    leftX = rightX - TOA2;

    this->routeAnimationEllipseRightBackup.at(index)->topLeft->setCoords(QPointF(leftX, leftY));
    this->routeAnimationEllipseRightBackup.at(index)->bottomRight->setCoords(QPointF(rightX, rightY));
}

double MainWindow::getTOA1(double x, double y)
{
    /**
      * @brief Function return distance between transmitter antenna and left
      * reciever antenna.
      *
      **/

    double transDist = this->calculateDistance(x, y, 0.0 ,0.0);
    double leftAntDist = this->calculateDistance(x, y, (-1.0)*left_antenna_dist, 0.0);
    return transDist+leftAntDist;
}

double MainWindow::getTOA2(double x, double y)
{
    /**
      * @brief Function return distance between transmitter antenna and right
      * reciever antenna.
      *
      **/

    double transDist = this->calculateDistance(x, y, 0.0 ,0.0);
    double rightAntDist = this->calculateDistance(x, y, right_antenna_dist, 0.0);
    return transDist+rightAntDist;
}

double MainWindow::calculateDistance(double x1, double y1, double x2, double y2)
{
    /**
      * @brief Function returns the distance between two points.
      *
      **/

    double dx = x2 - x1;
    if(dx < 0.0) dx *= -1.0;
    double dy = y2 - y1;
    if(dy < 0.0) dy *= -1.0;

    return sqrt(pow(dx, 2.0)+pow(dy,2.0));
}

void MainWindow::deleteRoute(struct route *rt)
{
    /**
      * @brief The following function will delete all graphics
      * objects in the scene with the relation to the passed route
      *
      **/

    struct route *temp = rt;

    while(1)
    {
        // iterate through all items and release memory
        if(temp->speed) delete temp->speed;
        if(temp->line) delete temp->line;
        if(temp->point) delete temp->point;
        if(temp->next!=NULL)
        {
            temp = temp->next;
            delete temp->previous;
        }
        else
        {
            delete temp;
            break;
        }
    }
}
