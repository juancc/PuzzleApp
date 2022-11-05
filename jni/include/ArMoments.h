#ifndef ARMOMENTS_H
#define ARMOMENTS_H


#include"opencv2/highgui.hpp"
#include"opencv2/imgproc.hpp"
#include"opencv2/ml.hpp"

#include<iostream>
#include <sstream>

#include"../include/sys.h"
#include"../include/KNN.h"

#include"../include/obj.h"


using namespace cv;
using namespace std;



class ArMoments
{
    public:
        ArMoments(int, int, int);
        virtual ~ArMoments();

        int classify(Mat&);
        int training(string fileDir);
        void nextStep();//when user press next step

        // Gloval variables
        int minArea;
        int maxArea;
        int scaleFactor;// image source scale but for processing
        int searched;//searched object index

        string queryFile;
        //string assemblyDir;
        //string fileDir; // where the training images are located

        //filter
        //for threshold
        Mat srcThresh;
        int thresh;
        int maxValue;

        //for adaptative threshold
        int blockSize;
        int C;

        //filter
        int contrast;
        int brightness;
        int blurFact;

        Mat parts;
        Mat *partsImg; //array containing al the images of the training
        Mat assemblyStatus; // displayed image of how the assembly goes

        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;

        int status;//0= train, 1= classify

        //knn
        //const int K = 3;
        //Mat neighbor(1,K, CV_32FC1);
        //Mat dist (1, K, CV_32FC1);
        //Mat result (1, K, CV_32FC1);
        //Mat query(1,7, CV_32FC1);

        //Mat trainingData;
        //Mat trainingClasses;
        //Ptr<ml::KNearest>  knn(ml::KNearest::create());//knn

        //own knn - simplified version
        KnnSimple simpleKnn;
        double minDist;//min distance of two objects to be considered a candidate
        double objDist;//same object distance

        string *hints;

        int hintsH; //height of the hints rectangle
        Scalar rectColor;
        Scalar fontColor;
        Scalar miniatureColor;
        Scalar partColor;


vector<string> classes;

    private:
        void drawMain(Mat &src);
        void calcMoment( int, int, Mat&);//callback that calculate moment on source, changing the thresh value
        void filter();
        void AR(Obj, Mat&);//return the name of the object
};

#endif // ARMOMENTS_H
