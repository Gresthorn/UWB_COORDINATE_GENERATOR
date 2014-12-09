#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class transformation2D
{

private:
    vector <vector <double> * > * rotationMatrix;
    vector <vector <double> * > * newSystemPositionMatrix;
    vector <vector <double> * > * oldCoordinatesMatrix;

    vector <vector <double> * > * newSystemPositionMatrixReverse;
    vector <vector <double> * > * rotationMatrixInverse;

    double rotAngle; // angle to rotate coordinates

    const double PI = 3.1415926535897932384626433832795028841971693; // used for conversion to radians or degrees if needed

    double R; // coordinate X in OPERATOR VIEW
    double S; // coordinate Y in OPERATOR VIEW

    double X; // new (transformated) X
    double Y; // new (transformated) Y

    double A1; // X position of new coordinate system in relation to OPERATOR coordination system
    double A2; // Y position of new coordinate system in relation to OPERATOR coordination system

    void convertToRadians(double angle); // converts the current angle to radians from degrees
    void buildRotationMatrix();
    void buildNewSystemPositionMatrix();
    void buildOldCoordinatesMatrix();

    void computeXY(void);

    /*------------------------------------BASIC MATRIX WORKAROUND START-----------------------------------------------*/

    template <typename TYPE> // adds new row to matrix filled with values in array
    void addRow(vector < vector <TYPE> * > * matrix, TYPE * array, int array_size)
    {
        matrix->push_back(new vector<TYPE>);

        int elements_in_array = array_size/sizeof(TYPE);
        // now we can iterate over all elements in array
        for(int i=0; i<elements_in_array; i++)
            matrix->back()->push_back(array[i]);
    }

    template <typename TYPE2> // useable only in console
    void printMatrix(vector <vector <TYPE2> * > * matrix)
    {
        for(int row = 0; row < matrix->size(); row++)
        {
            for(int column = 0; column < matrix->at(row)->size(); column++)
                cout << matrix->at(row)->at(column) << "\t";

            cout << endl;
        }
    }

    template <typename TYPE3>
    TYPE3 getVal(int row, int column, vector <vector <TYPE3> * > * matrix)
    {
        return matrix->at(row-1)->at(column-1);
    }

    template <typename TYPE4>
    vector <vector <TYPE4> * > * productMatrix(vector <vector <TYPE4> * > * A, vector <vector <TYPE4> * > * B)
    {
        vector <vector <TYPE4> * > * resultMatrix = new vector <vector <TYPE4> * >;

        if(!this->checkMatrix(A)) return NULL;
        if(!this->checkMatrix(B)) return NULL;

        // now when we know that matrix are correct, we check for product condition
        if(A->at(0)->size()!=B->size()) return NULL;

        for(int nrows = 0; nrows < A->size(); nrows++)
        {
            resultMatrix->push_back(new vector <TYPE4>);
            for(int ncolumns = 0; ncolumns < B->at(0)->size(); ncolumns++)
            {
                TYPE4 value = 0;
                for(int i = 0; i < B->size(); i++)
                    value += A->at(nrows)->at(i)*B->at(i)->at(ncolumns);

                resultMatrix->back()->push_back(value);
            }
        }

        return resultMatrix;
    }

    template <typename TYPE5> // check if matrix is correct: if each row has the same number of elements
    bool checkMatrix(vector <vector <TYPE5> * > * matrix)
    {
        int count = matrix->at(0)->size();

        for(int i = 1; i < matrix->size(); i++)
            if(matrix->at(i)->size() != count) return false;

        return true;
    }

    template <typename TYPE6> // creates matrix with one row filled with values
    vector <vector <TYPE6> * > * createRowMatrix(TYPE6 * array, int array_size)
    {
        vector <vector <TYPE6> * > * new_vector = new vector <vector <TYPE6> * >;
        new_vector->push_back(new vector<TYPE6>);

        int number_of_elements = array_size/sizeof(TYPE6);

        for(int i = 0; i < number_of_elements; i++)
            new_vector->back()->push_back(array[i]);

        return new_vector;
    }

    template <typename TYPE7> // creates matrix with one column filled with values
    vector <vector <TYPE7> * > * createColumnMatrix(TYPE7 * array, int array_size)
    {
        int number_of_elements = array_size/sizeof(TYPE7);
        vector <vector <TYPE7> * > * new_vector = new vector <vector <TYPE7> * >;

        for(int i = 0; i < number_of_elements; i++)
        {
             new_vector->push_back(new vector <TYPE7>);
             new_vector->back()->push_back(array[i]);
        }

        return new_vector;
    }

    template <typename TYPE8> // sums two matrixes
    vector <vector <TYPE8> * > * sumMatrix(vector <vector <TYPE8> * > * A, vector <vector <TYPE8> * > * B)
    {
        if(!this->checkMatrix(A) || !this->checkMatrix(B)) return NULL;

        // check for basic sum condition
        if((A->size() != B->size()) || (A->at(0)->size() != B->at(0)->size())) return NULL;

        vector <vector <TYPE8> * > * sum = new vector <vector <TYPE8> * >;

        for(int rows = 0; rows < A->size(); rows++)
        {
            sum->push_back(new vector <TYPE8>);

            for(int columns = 0; columns < A->at(0)->size(); columns++)
            {
                sum->back()->push_back(A->at(rows)->at(columns) + B->at(rows)->at(columns));
            }
        }

        return sum;
    }

    template <typename TYPE9>
    vector <vector <TYPE9> * > * productMatrixWithNumber(vector <vector <TYPE9> * > * matrix, TYPE9 number)
    {
        if(!checkMatrix(matrix)) return NULL;

        vector <vector <TYPE9> * > * new_matrix = new vector <vector <TYPE9> * >;

        for(int row = 0; row < matrix->size(); row++)
        {
            new_matrix->push_back(new vector <TYPE9>);

            for(int column = 0; column < matrix->at(0)->size(); column++)
            {
                new_matrix->back()->push_back(matrix->at(row)->at(column)*number);
            }
        }

        return new_matrix;
    }

    template <typename TYPE10>
    void deleteMatrix(vector <vector <TYPE10> * > * matrix)
    {
        for(int counter = 0; counter < matrix->size(); counter++)
        {
            delete matrix->at(counter);
        }

        delete matrix;
    }

    template <typename TYPE11>
    double laplaceDeterminant(vector <vector <TYPE11> * > * matrix, bool checkMatrix = true)
    {
        if(checkMatrix)
        {
            if(!this->checkMatrix(matrix)) return -1.0;

            // checking for square matrix
            if(matrix->at(0)->size()!=matrix->size()) return -1.0;
        }


        // recursive calculating until we are at the end of firs line

        if(matrix->size() == 1) return this->getVal(1, 1, matrix);
        else {
            double a1j = 0.0;
            double result = 0.0;
            vector <vector <TYPE11> * > * cutMatrix;

            for(int j = 1; j <= matrix->at(0)->size(); j++)
            {
                a1j = this->getVal(1, j, matrix);

                cutMatrix = this->deleteRow(1, this->deleteColumn(j, matrix)); // matrix without first row and j-th column

                result = result + a1j*(pow((-1.0), 1+j))*this->laplaceDeterminant(cutMatrix, false);

                this->deleteMatrix(cutMatrix);
            }

            return result;
        }

        return -1;
    }

    template <typename TYPE12>
    vector <vector <TYPE12> * > * deleteRow(int row, vector <vector <TYPE12> * > * matrix)
    {
         vector <vector <TYPE12> * > * newMatrix = this->copyMatrix(matrix);

         /*for(int i = 0; i < matrix->size(); i++)
         {
             newMatrix->push_back(new vector <TYPE12> );
             for(int z = 0; z < matrix->at(i)->size(); z++)
             {
                newMatrix->back()->push_back(matrix->at(i)->at(z));
             }
         }*/

         newMatrix->erase(newMatrix->begin() + (row-1));

         return newMatrix;
    }

    template <typename TYPE13>
    vector <vector <TYPE13> * > * deleteColumn(int column, vector <vector <TYPE13> * > * matrix)
    {
         vector <vector <TYPE13> * > * newMatrix = this->copyMatrix(matrix);

         /*for(int i = 0; i < matrix->size(); i++)
         {
             newMatrix->push_back(new vector <TYPE13> );
             for(int z = 0; z < matrix->at(i)->size(); z++)
             {
                newMatrix->back()->push_back(matrix->at(i)->at(z));
             }
         }*/

         for(int size = 0; size < newMatrix->size(); size++)
            newMatrix->at(size)->erase(newMatrix->at(size)->begin() + (column-1));

         return newMatrix;
    }

    template <typename TYPE14>
    vector <vector <TYPE14> * > * copyMatrix(vector <vector <TYPE14> * > * matrix)
    {
         vector <vector <TYPE14> * > * newMatrix = new vector <vector <TYPE14> * >;

         for(int i = 0; i < matrix->size(); i++)
         {

             newMatrix->push_back(new vector <TYPE14> );

             for(int z = 0; z < matrix->at(i)->size(); z++)
             {
                newMatrix->back()->push_back(matrix->at(i)->at(z));
             }
         }

         return newMatrix;
    }

    template <typename TYPE15>
    vector <vector <TYPE15> * > * inverseMatrix(vector <vector <TYPE15> * > * matrix)
    {
         vector <vector <TYPE15> * > * newMatrix = new vector <vector <TYPE15> * >;
         vector <vector <TYPE15> * > * subdeterminantMatrix;

         double determinant = this->laplaceDeterminant(matrix);

         if(determinant == 0.0) return NULL; // non regular matrix

         for(int r = 1; r <= matrix->size(); r++)
         {
             newMatrix->push_back(new vector <TYPE15> );

             for(int c = 1; c <= matrix->back()->size(); c++)
             {
                subdeterminantMatrix = this->deleteRow(c, this->deleteColumn(r, matrix));


                // !!! INDEXES MUST BE IN REVERSE ORDER otherwise we get transposed inv. matrix
                double subdeterminant = this->laplaceDeterminant(subdeterminantMatrix);

                newMatrix->back()->push_back((pow((-1.0), r+c)*subdeterminant)/determinant);

                this->deleteMatrix(subdeterminantMatrix);
             }
         }

         return newMatrix;
    }

    template <typename TYPE16>
    vector <vector <TYPE16> * > * transposeMatrix(vector <vector <TYPE16> * > * matrix)
    {
         vector <vector <TYPE16> * > * newMatrix = new vector <vector <TYPE16> * >;

         for(int r = 1; r <= matrix->size(); r++)
         {
             newMatrix->push_back(new vector <TYPE16> );

             for(int c = 1; c <= matrix->back()->size(); c++)
             {
                   newMatrix->back()->push_back(this->getVal(c, r, matrix));
             }
         }

         return newMatrix;
    }

    /*------------------------------------BASIC MATRIX WORKAROUND END-------------------------------------------------*/


public:
    transformation2D(double r, double s, double a1, double a2, double angle);
    ~transformation2D();

    void setUpOldCoordinates(double x, double y);
    void setUpRotationAngle(double angle);
    void setUpNewSystemPosition(double x, double y);

    double getX(void) { return this->X; }
    double getY(void) { return this->Y; }
    double getAngle(void) { return this->rotAngle; }
    bool isBehindRadar(double Xcor, double Ycor);
};

#endif // END TRANSFORMATION_H
