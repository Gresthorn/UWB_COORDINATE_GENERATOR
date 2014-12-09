#ifndef ROUTEMATH_H
#define ROUTEMATH_H

#include <qmath.h>

#include "structures.h"

class routemath
{
    /**
      * @brief The following class is used to compute the position of
      * specific object in given time. Each route represents one
      * object. Depending on speed, by using the method step(time)
      * will object return the x and y coordinates at specific time.
      *
      * @brief IMPORTANT NOTE > While in file generating, method nextstep(void)
      * is used instead of step(time) because of performance issues.
      * Nextstep(void) is computing from last known position and does
      * not need to compute from the beginning each time it is called.
      * It needs to know timestep value which is passed in constructor.
      * Default value of timestep is 1/13 of second.
      *
      **/
//-------------------------------------------------------------------------------------
public:
    routemath(struct route * currRoute, double speedVal, double timeStepL, double widthRatio, double heightRatio);
    ~routemath();

    double * nextStep();
    double getNewX() { return newCoordinates[0]; }
    double getNewY() { return newCoordinates[1]; }
    bool end(void) { return FINISHED; }
private:
    bool calculationFinished(void) { return FINISHED; }

    void xSpeedComponent(double slope, double speed, double timeSt);
    double distance(double x1, double y1, double x2, double y2);

    void calculateLineConstants(double x1, double y1, double x2, double y2);
    void calculateNextCoordinate(double cx, double cy, bool direction);

    void UPDATEDATA(void); // this function will use the functions above to calculate  new constants

    double * newCoordinates;

    double widthRatio;
    double heightRatio;

    double speed;
    double speedForward; // is the x component of speed vector, in special cases direct speed (speed value)
    bool speedDirection; // if true the move is DOWN->UP or LEFT->RIGHT

    bool constantSlopeUndefined;
    double constantSlopeM;
    double constantC;

    double majorDistance; // distance between two points that object is currently moving
    double currentDistance; // distance between the first point (A) that object left and his current position

    struct route * route;

    double timeStep;
    double currentTime;
    double startTime;

    bool FINISHED; // if all calculation is done (no more points to move), value is true
};
#endif // ROUTEMATH_H
