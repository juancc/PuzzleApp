#ifndef knnSimple
#define knnSimple

#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

class KnnSimple {
    private:
        vector<string> classes;
        vector<double*> descriptors;
        int descSize;
        double euclideanDist(double*,double*);


    public:
        void train(double* , string , int);
        string findNearest(double*, double&);
        vector<double*> getDesc();
        void printDatabase();
        double isThis(int, double*);
        string getName(int classIdx);
};




#endif // knnSimple
