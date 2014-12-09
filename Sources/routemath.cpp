#include "routemath.h"

routemath::routemath(struct route * currRoute, double speedVal, double timeStepL, double widthR, double heightR)
{
    // we will suppose that in main window, work area is 500x500
    widthRatio = widthR;
    heightRatio = heightR;

    timeStep = timeStepL;
    startTime = 0.0;
    currentTime = startTime;

    speed = speedVal;
    route = currRoute;

    newCoordinates = new double[2];

    FINISHED = false;

    // SETUP NEW DATA
    if(route->next!=NULL) this->UPDATEDATA();
}

void routemath::UPDATEDATA(void)
{
    this->newCoordinates[0] = this->route->point->x()*this->widthRatio;
    this->newCoordinates[1] = this->route->point->y()*this->heightRatio;

    this->calculateLineConstants(this->route->point->x()*this->widthRatio, this->route->point->y()*this->heightRatio, this->route->next->point->x()*this->widthRatio, this->route->next->point->y()*this->heightRatio);
    this->xSpeedComponent(this->constantSlopeM, this->speed, this->timeStep);
    this->majorDistance = distance(this->route->point->x()*this->widthRatio, this->route->point->y()*this->heightRatio, this->route->next->point->x()*this->widthRatio, this->route->next->point->y()*this->heightRatio);
    this->currentDistance = 0.0;
}

double * routemath::nextStep()
{
    /**
      * @brief The function calculate the next position of moving objects and
      * returns the array containing two values: (x, y); the new coordinates
      *
      **/

    if(route->next == NULL)
    {
        // no more points (if the route was one point only)
        FINISHED = true;
        this->newCoordinates[0] = this->route->point->x()*this->widthRatio;
        this->newCoordinates[1] = this->route->point->y()*this->heightRatio;
        return newCoordinates;
    }
    else
    {
        // still more to calculate

        this->calculateNextCoordinate(this->newCoordinates[0], this->newCoordinates[1], this->speedDirection);

        // update the current distance between the start point and the current position
        this->currentDistance = this->distance(this->route->point->x()*this->widthRatio, this->route->point->y()*this->heightRatio, this->newCoordinates[0], this->newCoordinates[1]);

        if(this->majorDistance > this->currentDistance)
        {
            // object is still before the endpoint
            return newCoordinates;
        }
        else
        {
            // object reached the endpoint and is behind him. We need to update direction, constants etc.
            this->route = this->route->next;
            if(this->route->next == NULL)
            {
                // no more points
                FINISHED = true;
                this->newCoordinates[0] = this->route->point->x()*this->widthRatio;
                this->newCoordinates[1] = this->route->point->y()*this->heightRatio;
                return newCoordinates;
            }
            else
            {
                // there is continuing path, so we update all needed data and get new coordinates
                this->UPDATEDATA();
                return this->nextStep();
            }

        }
    }

    return NULL;

}

void routemath::xSpeedComponent(double slope, double speed, double timeSt)
{
    /**
      * @brief The function returns x component of speed vector what is used
      * for iteration of x direction of moving object. Iterated value is then
      * used to determine the y coordinate of the next position.
      *
      **/

    if(this->constantSlopeUndefined)
    {
        this->speedForward = speed*timeSt;
        if(this->route->next!=NULL)
        {
            if(this->route->point->y()*this->heightRatio < this->route->next->point->y()*this->heightRatio)
            {
                // object is moving DOWN->UP
                this->speedDirection = true;
            } else {
                // object is moving UP->DOWN
                this->speedDirection = false;
            }
        }
    }
    else
    {

        double angle = atan(slope);
        double xComponent = speed*cos(angle);

        if(xComponent < 0.0) xComponent *= (-1); // absolute value

        this->speedForward = xComponent*timeSt;
        if(this->route->next!=NULL)
        {
            if(this->route->point->x()*this->widthRatio < this->route->next->point->x()*this->widthRatio)
            {
                // object is moving LEFT->RIGHT
                this->speedDirection = true;
            } else {
                // object is moving RIGHT->LEFT
                this->speedDirection = false;
            }
        }
    }
}

void routemath::calculateLineConstants(double x1, double y1, double x2, double y2)
{
    /**
      * @brief The function will calculate the constantSlopeM and constantC which will
      * represent the equation of line specified by given two points.
      *
      **/

    double DX = x2-x1;
    double DY = y2-y1;

    /* if DX == 0.0 that means that tangent must be infinite, what represent vertical line
       and may cause problems while calculating new y coordinate.
       Thats why we use helper variable constantSlopeUndefined. */

    if(qFuzzyCompare(DX, 0.0)) this->constantSlopeUndefined = true;
    else {
        this->constantSlopeUndefined = false;

        if(qFuzzyCompare(DY, 0.0)) this->constantSlopeM = 0.0;
        else this->constantSlopeM = DY/DX;

        // constantC will be availible from the line equation y = m*x + c
        // we will use for example the second x,y coordinates
        this->constantC = y2 - this->constantSlopeM*x2;
    }
}

void routemath::calculateNextCoordinate(double cx, double cy, bool direction)
{
    /**
      * @brief From the current position xc, cy; using the constantSlopeM,
      * constantC, constantSlopeUndefined will this function calculate the next
      * possible position of object using the line equation
      *
      * NOTE: IF DIRECTION IS TRUE, THEN MOVE IS UP->DOWN OR LEFT->RIGHT
      *
      **/

    if(this->constantSlopeUndefined) {
        // vertical line
        this->newCoordinates[0] = cx;
        if(direction) this->newCoordinates[1] = cy + this->speedForward;
        else this->newCoordinates[1] = cy - this->speedForward;
    }
    else
    {
        // some other than vertical line
        double new_x = 0.0;
        if(this->speedDirection) new_x = cx + this->speedForward;
        else new_x = cx - this->speedForward;

        this->newCoordinates[0] = new_x;
        this->newCoordinates[1] = (this->constantSlopeM)*new_x + constantC;
    }
}

double routemath::distance(double x1, double y1, double x2, double y2)
{
    /**
      * @brief Returns the distance between two points represented by given coordinates
      *
      **/

    return sqrt(pow(x1-x2, 2.0)+pow(y1-y2, 2.0));
}

routemath::~routemath()
{
    delete newCoordinates;
}
