#include "transformation.h"

transformation2D::transformation2D(double r, double s, double a1, double a2, double angle)
{  
    rotationMatrix = NULL;
    rotationMatrixInverse = NULL;
    newSystemPositionMatrix = NULL;
    newSystemPositionMatrixReverse = NULL;
    oldCoordinatesMatrix = NULL;

    R = r;
    S = s;
    A1 = a1;
    A2 = a2;
    this->convertToRadians(angle);


    // The following matrixes are fundamental for calculating coordinates
    this->buildRotationMatrix();
    this->buildNewSystemPositionMatrix();
    this->buildOldCoordinatesMatrix();

    this->computeXY();

}

transformation2D::~transformation2D()
{
    this->deleteMatrix(newSystemPositionMatrix);
    this->deleteMatrix(rotationMatrix);
    this->deleteMatrix(oldCoordinatesMatrix);
}

void transformation2D::convertToRadians(double angle)
{
    // use this function before the calculation if you pass degrees as parameter

    this->rotAngle = angle*(this->PI/180);
}

double transformation2D::getDegrees(double angle)
{
    return angle*(180.0/this->PI);
}

void transformation2D::buildRotationMatrix()
{
    if(rotationMatrix!=NULL) this->deleteMatrix(rotationMatrix);

    rotationMatrix = new vector< vector<double> * >;

    double cosAngle = cos(this->rotAngle);
    double sinAngle = sin(this->rotAngle);

    double firstLine [] = { cosAngle, (-1.0)*sinAngle };
    double secondLine [] = { sinAngle, cosAngle };

    this->addRow(rotationMatrix, firstLine, sizeof(firstLine));
    this->addRow(rotationMatrix, secondLine, sizeof(secondLine));

    if(rotationMatrixInverse!=NULL) this->deleteMatrix(rotationMatrixInverse);
    // calculating this matrix in advance will speed up computeXY()
    this->rotationMatrixInverse = this->inverseMatrix(this->rotationMatrix);
}

void transformation2D::buildNewSystemPositionMatrix()
{
    if(newSystemPositionMatrix!=NULL) this->deleteMatrix(newSystemPositionMatrix);

    newSystemPositionMatrix = new vector< vector<double> * >;

    double firstColumn [] = { this->A1, this->A2 };

    newSystemPositionMatrix = this->createColumnMatrix(firstColumn, sizeof(firstColumn));

    if(newSystemPositionMatrixReverse!=NULL) this->deleteMatrix(newSystemPositionMatrixReverse);
    // calculating this matrix in advance will speed up computeXY()
    newSystemPositionMatrixReverse = this->productMatrixWithNumber(this->newSystemPositionMatrix, (-1.0));
}

void transformation2D::buildOldCoordinatesMatrix()
{
    if(oldCoordinatesMatrix!=NULL) this->deleteMatrix(oldCoordinatesMatrix);


    oldCoordinatesMatrix = new vector< vector<double> * >;

    double firstColumn [] = { this->R, this->S };

    oldCoordinatesMatrix = this->createColumnMatrix(firstColumn, sizeof(firstColumn));
}

void transformation2D::computeXY()
{
    vector< vector<double> * > * diffMatrix = this->sumMatrix(this->oldCoordinatesMatrix, this->newSystemPositionMatrixReverse);
    vector< vector<double> * > * result = this->productMatrix(this->rotationMatrixInverse, diffMatrix);

    this->X = this->getVal(1, 1, result);
    this->Y = this->getVal(2, 1, result);

    this->deleteMatrix(diffMatrix);
    this->deleteMatrix(result);
}

void transformation2D::setUpOldCoordinates(double x, double y)
{
    this->R = x;
    this->S = y;

    this->buildOldCoordinatesMatrix();

    this->computeXY();
}

void transformation2D::setUpRotationAngle(double angle)
{
    this->convertToRadians(angle);
    this->buildRotationMatrix();
    this->computeXY();
}

void transformation2D::setUpNewSystemPosition(double x, double y)
{
    this->A1 = x;
    this->A2 = y;

    this->buildNewSystemPositionMatrix();
    this->computeXY();
}

bool transformation2D::isBehindRadar(double Xcor, double Ycor)
{
    double considered_angle = rotAngle;
    double tang = tan(considered_angle);

    considered_angle = getDegrees(considered_angle);

    if(considered_angle<0.0) considered_angle *= (-1.0);

    if((considered_angle>(180.0-1.0) && considered_angle<(180.0+1.0)) ||
            (considered_angle>(90.0-1.0) && considered_angle<(90.0+1.0)) ||
            (considered_angle>(0.0-1.0) && considered_angle<(0.0+1.0)) ||
            (considered_angle>(270.0-1.0) && considered_angle<(270.0+1.0)))
    { return false; } // because it is impossible to see nothing according to our scene

    double pos = tang*(Xcor-A1) + A2;


    if(considered_angle>90.0 && considered_angle<270.0)
    {
        if(Ycor>pos) return true;
        else return false;
    }
    if(Ycor<pos) return true;
    return false;
}
