//Client app for the development in c++ using code::blocks
#include"opencv2/highgui.hpp"
#include<iostream>

#include"include/ArMoments.h"

using namespace cv;
using namespace std;

int minArea = 10000;
int maxArea = 400000;
int scaleFactor = 2;


ArMoments arMom(minArea, maxArea, scaleFactor);//service of ar using hu moments


int main(int argc, char** argv){


	//getting file names from path
    string dataBaseDir = argc <2 ? "img" : argv[1];

	string fileDir = dataBaseDir; fileDir +=  "/parts"; //directory of the images for the training
	string queryFile = dataBaseDir; queryFile += "/input/camInput.png"; //input image

	//arMom.assemblyDir = dataBaseDir; arMom.assemblyDir += "/assembly"; // assembly directory

	cout << "Loading Folder: " << fileDir << endl;

	namedWindow("src", CV_WINDOW_AUTOSIZE);
	namedWindow("threshold", CV_WINDOW_AUTOSIZE);


	//training
	int successTraining = arMom.training(fileDir);//train knn with the images in dir
	if(successTraining != 0){return -1; }




	cout << "  -CLASIFYING" << endl;
    while(1){
		//classify
        Mat output = imread(queryFile, CV_LOAD_IMAGE_UNCHANGED);
        resize(output, output, output.size()/4);

        int successClassify  = arMom.classify(output);



        int key = waitKey(30);


        if (key == 27){
                cout << "User has escaped" << endl;
                break;
        }else if (key == 32) {// press space, next step of the assembly
                    arMom.nextStep();
        }

        imshow("threshold", arMom.srcThresh);
        imshow("src", output);
    }


	destroyAllWindows();

	return 0;

}

