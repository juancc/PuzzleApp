/*
 * Android client for the library developed in c++ of AR using hu moments
 */
#include <jni.h>
#include <iostream>
#include <sstream>
#include <android/log.h>
#include <opencv2/highgui/highgui.hpp>
#include "include/ArMoments.h"
#define  LOG_TAG    "native"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

using namespace std;
using namespace cv;

extern "C" {
	//Global variables
	int minArea = 10000;
	int maxArea = 200000;
	int scaleFactor = 4;// image source scale
	//AR library
	ArMoments arMom(minArea, maxArea, scaleFactor);//service of ar using hu moments
	JNIEXPORT int JNICALL Java_com_example_puzzle_Augmentation_training(JNIEnv*, jobject){
		string fileDir = "/sdcard/Puzzle/img/parts";
		arMom.training(fileDir) == 0 ? LOGI(" -- Training Done --") :  LOGI(" -- Training Failed --");
	}
	//CLASSIFY
	JNIEXPORT void JNICALL Java_com_example_puzzle_Augmentation_classify(JNIEnv*, jobject, jlong addrRgba){
		Mat& output = *(Mat*)addrRgba;
		//Mat src = output.clone();
//		Mat src = imread("/sdcard/Puzzle/img/input/input.png", CV_LOAD_IMAGE_UNCHANGED);
		//Mat src = imread("/sdcard/Puzzle/img/input/input.png", CV_LOAD_IMAGE_UNCHANGED);

		// ----------------- CLASIFYING -----------------
		resize(output, output, output.size()/2);
		int successClassify  = arMom.classify(output);
		resize(output, output, output.size()*2);
	}
	//CLASSIFY
	JNIEXPORT void JNICALL Java_com_example_puzzle_Augmentation_next(JNIEnv*, jobject, jlong addrRgba){
		arMom.nextStep();
	}
}
