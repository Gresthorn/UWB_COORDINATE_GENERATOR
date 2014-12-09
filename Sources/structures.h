#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "QGraphicsEllipseItem"
#include "QGraphicsLineItem"

struct route {
    struct route *previous;
    struct route *next;
    QGraphicsEllipseItem *point;
    QGraphicsLineItem *line; // the line that ends up in that 'point'!!!
    double *speed;
};

#endif // STRUCTURES_H
