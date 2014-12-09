#ifndef MONITOREDAREASCENESUBCLASS_H
#define MONITOREDAREASCENESUBCLASS_H

#include <QGraphicsScene>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>

#include <stdlib.h> // because of random numbers
#include <time.h> // because of random numbers
#include "mainwindow.h"

class monitoredAreaSceneSubclass : public QGraphicsScene
{

private:
    class MainWindow *mainWindowPointer;
    int r,g,b;
    bool newRoute;

public:
    monitoredAreaSceneSubclass(class MainWindow *parent);
    void refreshNewRoute(void) { newRoute = true; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif // MONITOREDAREASCENESUBCLASS_H
