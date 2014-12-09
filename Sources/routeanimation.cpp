#include "routeanimation.h"

/* QCPITEM CIRCLE CLASS */

QCPItemCircle::QCPItemCircle(QCustomPlot *plot, double X, double Y, double R, QColor color)
    : QCPItemEllipse(plot)
{
    this->x = X;
    this->y = Y;
    this->r = R;

    this->setBrush(QBrush(QColor(color)));

    this->makePosition();
}

void QCPItemCircle::centerOn(double X, double Y)
{
    this->x = X;
    this->y = Y;

    this->makePosition();
}

void QCPItemCircle::setWidth(double R)
{
    this->r = R;

    this->makePosition();
}

void QCPItemCircle::makePosition()
{
    /**
     * @brief This function will set the new position for circle correctly
     * so only x, y values should be passed to method centerOn();
     *
     * This method will also calculate correct 'r' for height and width
     * as far as the ratio of width and height could not be always 1
     *
     */

    double x_lower = this->parentPlot()->xAxis->range().lower;
    double x_upper = this->parentPlot()->xAxis->range().upper;
    double y_lower = this->parentPlot()->yAxis->range().lower;
    double y_upper = this->parentPlot()->yAxis->range().upper;

    double x_w = (x_upper - x_lower) < 0.0 ? (-1.0)*(x_upper - x_lower) : (x_upper - x_lower);
    double y_w = (y_upper - y_lower) < 0.0 ? (-1.0)*(y_upper - y_lower) : (y_upper - y_lower);

    double ratio = x_w/y_w;

    this->topLeft->setCoords(QPointF(x-r*ratio, y+r));
    this->bottomRight->setCoords(QPointF(x+r*ratio, y-r));
}
