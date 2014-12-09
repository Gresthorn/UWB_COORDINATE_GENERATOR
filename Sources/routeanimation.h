#ifndef ROUTEANIMATION_H
#define ROUTEANIMATION_H

#include "qcustomplot.h"

// derived ellipse item for easy centering -> displaying circle
class QCPItemCircle : public QCPItemEllipse
{
public:
    QCPItemCircle(QCustomPlot *plot, double X, double Y, double R, QColor color);
    void centerOn(double X, double Y);
    void setWidth(double R);

private:
    void makePosition(void);

    double x;
    double y;
    double r;

};

#endif // ROUTEANIMATION_H
