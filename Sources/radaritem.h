#ifndef RADARITEM_H
#define RADARITEM_H

#include <QGraphicsItemGroup>
#include <QtGlobal>
#include <QObject>
#include <QPen>
#include <math.h>

class radarItem : public QObject, public QGraphicsItemGroup
{
    /**
      * IMPORTANT NOTE > Inheritance of QObject is important for using signal/slot system!!!
      *
      **/
    Q_OBJECT

public:
    radarItem(const QString &radarName,
              int radarPicX = 60, int radarPicY = 30,
              int signalPicX = 60, int signalPicY = 40,
              double sceneX = 562.0, double sceneY = 562.0);
    QRectF boundingRect() const;

    int radarPicWidth(void) { return radarPic.width; }
    int radarPicHeight(void) { return radarPic.height; }
    int signalPicWidth(void) { return signalPic.width; }
    int signalPicHeight(void) { return signalPic.height; }

    void setCode(int c) { code = c; }
    void setPenultimateCode(int c) { penultimatecode = c; }
    int lastCode(void) { return code; }
    int lastPenultimateCode(void) { return penultimatecode; }

    void setAngle(float a) { angleOfRadar = a; }
    float currentAngle(void) { return angleOfRadar; }
    void setAngleOfItem(float a) { this->setRotation(a); }
    double realAngle(void);

    QString *radarName() { return name; }
    void setRadarName(QString *newRadarName) { delete name; name = newRadarName; }

    ~radarItem();

private:
    bool mousePressed;
    int code; // holds information about on what trace is item currently positioned
    int penultimatecode; // when item enters corner, here is the last trace code remembered

    float angleOfRadar; // angle of item depends on position on the scene, but radar angle is relative
    QString *name;

    struct radarPicStruct { // holds information about image size : radar_pic.png
        int width;
        int height;
    } radarPic;
    struct signalPicStruct { // holds information about image size : signal_pic.png
        int width;
        int height;
    } signalPic;
    struct sceneXYStruct { // holds information about scene size in pixels
        double width;
        double height;
    } sceneXY;

    // items
    QGraphicsPixmapItem *radarPicItem;
    QGraphicsPixmapItem *signalPicItem;
    QGraphicsLineItem *lineHorizontal;
    QGraphicsLineItem *lineVertical;
    QGraphicsLineItem *lineUnderlineHorizontal;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

signals:
    void radarItemIsClickedSignal(void); // signal is recieved by parent widget slot and will store pointer to this item
    void radarItemIsMovedSignal(void); // signal is recieved by parent widget slot and will update the position coordinates in visible radar panel
};

#endif // RADARITEM_H
