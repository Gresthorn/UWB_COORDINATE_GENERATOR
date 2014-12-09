#include "monitoredareascenesubclass.h"

monitoredAreaSceneSubclass::monitoredAreaSceneSubclass(class MainWindow *parent) : QGraphicsScene(parent)
{    
    /**
      * Base settings are merely realized in mainwindow.cpp after creating new object
      **/

    mainWindowPointer = parent;
    // random numbers used for generating different colors
    srand(NULL);
    newRoute = true;
}

void monitoredAreaSceneSubclass::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(mainWindowPointer->drawingModeIsEnabled()) {
         // getting relative coordinates in the scene area
         double x = event->scenePos().rx();
         double y = event->scenePos().ry();


         // setting conditions for allowed drawing
         if((x>=250.0) || (x<=-250.0) || (y>=500.0) || (y<=0.0))
         {
             QMessageBox::warning(NULL, tr("Cannot add such point"), tr("You cannot add a point "
                                                                        "outside the monitored area"),
                                  QMessageBox::Ok);
         }
         else
         {
             // points drawing
             QGraphicsEllipseItem *dot = new QGraphicsEllipseItem(-1.5,-1.5,3.0,3.0);
             if(newRoute)
             {
                 // first circle is bigger
                 dot->setRect(-3.0,-3.0,6.0,6.0);
                 // set random color for new route
                 r = rand() % 256; g = rand() % 256; b = rand() % 256;
                 newRoute = false;
             }
             QColor clr(r,g,b);
             QBrush brush(clr);
             QPen pen(clr);

             dot->setPos(x, y);
             dot->setBrush(brush);
             dot->setPen(pen);
             this->addItem(dot);

             mainWindowPointer->addNewPoint(dot, clr);
         }
    }

    QGraphicsScene::mousePressEvent(event);
}
