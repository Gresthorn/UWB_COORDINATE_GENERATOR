#include "radaritem.h"
#include <QDebug>
#include <QLabel>

radarItem::radarItem(const QString &radarName, int radarPicX, int radarPicY, int signalPicX, int signalPicY, double sceneX, double sceneY)
{

    mousePressed = false;
    this->setAcceptHoverEvents(true); // enable hover effects
    code = 6;
    angleOfRadar = 0.0;
    name = new QString(radarName);

    /**
      * Storing information about image sizes and scene sizes
      * is used for exact positioning of image in right coordinates
      * and ensures that user cannot drag it outside the visible range
      * or other functionality.
      *
      **/

    radarPic.width = radarPicX;
    radarPic.height = radarPicY;
    signalPic.width = signalPicX;
    signalPic.height = signalPicY;
    sceneXY.width = sceneX;
    sceneXY.height = sceneY;

    /**
      * Setting up the items carrying graphic elements representing
      * radar. Items are positioned according to their image sizes.
      * Firstly is the image centered to 0 x 0 coordinate, but then
      * it is moved about -radarPic.height/2 down the y coordinate.
      * This is done because of later use in the scene. Border/Wall of the
      * scene is making line through 0 x 0 coordinate of the radar item
      * so it crosses the image in the middle. It is the aim to have the top of
      * image in the same level as the border/wall in the scene.
      *
      **/

    radarPicItem = new QGraphicsPixmapItem(*(new QPixmap(":/mainPics/pics/radar_pic.png")),this);
    signalPicItem = new QGraphicsPixmapItem(*(new QPixmap(":/mainPics/pics/signal_pic.png")),this);
    radarPicItem->setPos(-(radarPic.width/2), -2*(radarPic.height/2));
    signalPicItem->setPos(-(signalPic.width/2), -2*(signalPic.height/2)+radarPic.height);
    this->addToGroup(radarPicItem);
    this->addToGroup(signalPicItem);

    /**
      * Setting up the lines displayed after clicking the radar icon.
      * Length of line is the maximum possible visible line in the view
      * which is length of diagonal (or distance between left upper and
      * right bottom corner) of graphics scene widget.
      *
      **/

    double diagonal = sqrt(pow(sceneXY.height, 2.0) + pow(sceneXY.width, 2.0));
    lineVertical = new QGraphicsLineItem(0.0, 0.0, 0.0, diagonal, this);
    lineVertical->setPen(QPen(QBrush(Qt::black), 1, Qt::DotLine));
    lineHorizontal = new QGraphicsLineItem(-(diagonal), 0.0, diagonal, 0.0, this);
    lineHorizontal->setPen(QPen(QBrush(Qt::black), 1, Qt::DotLine));
    lineUnderlineHorizontal = new QGraphicsLineItem(-(radarPic.width/2), -2*(radarPic.height/2), radarPic.width/2, -2*(radarPic.height/2), this);
    lineUnderlineHorizontal->setPen(QPen(QBrush(Qt::gray), 2, Qt::SolidLine));
    this->addToGroup(lineVertical);
    this->addToGroup(lineHorizontal);
    this->addToGroup(lineUnderlineHorizontal);

    // Hiding items that are only visible after clicking
    signalPicItem->setVisible(false);
    lineVertical->setVisible(false);
    lineHorizontal->setVisible(false);
    lineUnderlineHorizontal->setVisible(false);

    this->setRotation(angleOfRadar);
}

QRectF radarItem::boundingRect() const
{
    // needed for hover effects
    return QRectF(-(radarPic.width/2), -2*(radarPic.height/2), radarPic.width, radarPic.height);
}

radarItem::~radarItem()
{
    delete name;
    delete radarPicItem;
    delete signalPicItem;
    delete lineVertical;
    delete lineHorizontal;
    delete lineUnderlineHorizontal;
}

void radarItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    mousePressed = true;

    signalPicItem->setVisible(true);
    lineVertical->setVisible(true);
    lineHorizontal->setVisible(true);

    QGraphicsItemGroup::mousePressEvent(event);

    emit radarItemIsClickedSignal();
}

void radarItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    mousePressed = false;

    signalPicItem->setVisible(false);
    lineVertical->setVisible(false);
    lineHorizontal->setVisible(false);

    QGraphicsItemGroup::mouseReleaseEvent(event);
}

void radarItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItemGroup::hoverEnterEvent(event);

    lineUnderlineHorizontal->setVisible(true);
}

void radarItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsItemGroup::hoverLeaveEvent(event);

    lineUnderlineHorizontal->setVisible(false);
}

void radarItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    /**
     * When moving with radar, this is allowed to move only along
     * the border/wall of monitored area. When the item is clicked
     * and mouse is moved, we need to check and maintain required
     * position.
     */

    QGraphicsItem::mouseMoveEvent(event);


    QPointF position = this->pos(); // current position of item (mouse)

    if(position.x()<-250.0) this->setX(-250.0);
    if(position.y()<0.0) this->setY(0.0);
    if(position.x()>250.0) this->setX(250.0);
    if(position.y()>500.0) this->setY(500.0);

    /*===========================================================================*/

    // we suppose area to be 500 x 500 pixels
    float trace1 = -250.0; // x-axis trace
    float trace2 = 0.0; // y-axis trace
    float trace3 = 250.0; // x-axis trace
    float trace4 = 500.0; // y-axis trace


    /**
      * If the item is in a corner, a special code for each corner is
      * set to 'code'. When we move the item, we can see for 'code' value
      * and make decision about setting a new route. Once when the route
      * is set, the item can be moved only on this route until it reaches
      * new corner. In each corner there are two possible routes to select.
      * Decision is made according to amount of pixels that item has moved
      * into one direction. More pixels will be meant to be operative
      * when it comes to direction.
      *
      **/

    float rx = this->pos().rx();
    float ry = this->pos().ry();
    if(qFuzzyCompare(rx,trace1) && qFuzzyCompare(ry,trace2)) {
        //code = 1
        setCode(1);
    }
    else if(qFuzzyCompare(rx,trace1) && qFuzzyCompare(ry,trace4)) {
        //code = 2
        setCode(2);
    }
    else if(qFuzzyCompare(rx,trace3) && qFuzzyCompare(ry,trace4)) {
        //code = 3
        setCode(3);
    }
    else if(qFuzzyCompare(rx,trace3) && qFuzzyCompare(ry,trace2)) {
        //code = 4
        setCode(4);
    }
    else {

        // when the item is not in the corner
        if(lastCode()<5) { // just moved from corner
            float dx = 0.0;
            float dy = 0.0;
            switch(lastCode()) {
                case 1:
                    // possible paths are trace1 or trace2
                    dx = trace1 - rx; (dx<0.0) ? dx*=(-1.0) : dx;
                    dy = trace2 - ry; (dy<0.0) ? dy*=(-1.0) : dy;
                    if(dx>dy) { this->setY(trace2); setCode(6); }
                    else if(dx<dy) { this->setX(trace1); setCode(5); }
                    else /*differentials are equal*/ this->setPos(trace1,trace2);
                    break;
                case 2:
                    // possible paths are trace1 or trace4
                    dx = trace1 - rx; (dx<0.0) ? dx*=(-1.0) : dx;
                    dy = trace4 - ry; (dy<0.0) ? dy*=(-1.0) : dy;
                    if(dx>dy) { this->setY(trace4); setCode(8); }
                    else if(dx<dy) { this->setX(trace1); setCode(5); }
                    else /*differentials are equal*/ this->setPos(trace1,trace4);
                    break;
                case 3:
                    // possible paths are trace3 or trace4
                    dx = trace3 - rx; (dx<0.0) ? dx*=(-1.0) : dx;
                    dy = trace4 - ry; (dy<0.0) ? dy*=(-1.0) : dy;
                    if(dx>dy) { this->setY(trace4); setCode(8); }
                    else if(dx<dy) { this->setX(trace3); setCode(7); }
                    else /*differentials are equal*/ this->setPos(trace3,trace4);
                    break;
                case 4:
                    // possible paths are trace3 or trace2
                    dx = trace3 - rx; (dx<0.0) ? dx*=(-1.0) : dx;
                    dy = trace2 - ry; (dy<0.0) ? dy*=(-1.0) : dy;
                    if(dx>dy) { this->setY(trace2); setCode(6); }
                    else if(dx<dy) { this->setX(trace3); setCode(7); }
                    else /*differentials are equal*/ this->setPos(trace3,trace2);
                    break;
                default: break;
            }
        }
        else {
           // moving between two points
           this->setPenultimateCode(this->lastCode()); // remember the trace
           switch(lastCode()) {
                case 5:
                    this->setX(trace1);
                    this->setRotation(-90.0 + angleOfRadar);
                    break;
                case 6:
                    this->setY(trace2);
                    this->setRotation(0.0 + angleOfRadar);
                    break;
                case 7:
                    this->setX(trace3);
                    this->setRotation(90.0 + angleOfRadar);
                    break;
                case 8:
                    this->setY(trace4);
                    this->setRotation(180.0 + angleOfRadar);
                    break;
                default: break;
           }
        }
    }

    // to update values in radar panel
    emit radarItemIsMovedSignal();
}

double radarItem::realAngle()
{
    /**
     * Note that angle saved in radarItem object is only the angle in relation
     * to its local coordinate system. This is rotating while user is moving radar
     * in the scene along the walls. The code variable indicates at which wall is
     * the radar currently positioned.
     *
     * IMPORTANT NOTE: Negative value of rotation angle is clockwise. The value
     * displayed at radarPanel in application is inversed.
     *
     * We need to use penultimate code because if we enter the corner for first time
     * radar will not rotate until we do not enter new trace but this action will change
     * the code to the corner code. Therefore we need to know from which trace the radar
     * came out.
     *
     * CODES:
     * 1 = "LEFT BOTTOM CORNER"
     * 2 = "LEFT UPPER CORNER"
     * 3 = "RIGHT UPPER CORNER"
     * 4 = "RIGHT BOTTOM CORNER"
     * 5 = "LEFT WALL"
     * 6 = "BOTTOM WALL"
     * 7 = "RIGHT WALL"
     * 8 = "UPPER WALL"
     *
     */


    double angle = this->currentAngle();

    switch(this->lastPenultimateCode())
    {
        case 5: angle += -90.0;
                break;
        case 6: angle += 0.0;
                break;
        case 7: angle += +90.0;
                break;
        case 8: angle += +180.0; // also could be -180.0
                break;
        default: break;
    }

    return angle;
}
