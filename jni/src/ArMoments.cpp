/* 
Self included class for Object Recognition and AR guidance
Calculates de Hu momento of a binary image and clasiffy with a KNN previously trained

JCA
*/
#include "../include/ArMoments.h"

ArMoments::ArMoments(int minArea, int maxArea, int scaleFactor): minArea(minArea), maxArea(maxArea), scaleFactor(scaleFactor)
{
    // Class variables

    //minArea = 120000;
    //maxArea = 100000;
    //scaleFactor = 4;// image source scale
    searched = 0;

    //filter
    //for threshold
    thresh = 60;
    maxValue = 255;

    //for adaptative threshold
    blockSize = 217;
    C = 20;

    //filter
    contrast = 2;
    brightness = 0;
    blurFact = 3;

    status = -1;//0= train, 1= classify

    //own knn - simplified version
    minDist = 0.00011;//min distance of two objects to be considered a candidate
    objDist = 0.000008; // distance that is considered the same object

    //AR interface
	// Needs to be changed
    hints = new string[3]{
		"Pick highlighted part",
		"Join highlighted part with previous one",
		"Join highlighted part with the assembly"
};
    hintsH =40;
    rectColor = Scalar(42,183, 202);
    fontColor = Scalar(244,244, 248);
    miniatureColor = Scalar(254,215, 102);
    partColor = Scalar(254,74, 73);

}

ArMoments::~ArMoments()
{
    //destructor
}

int ArMoments::classify(Mat &src){
	status = 1;
	if(src.empty()){
		cout << "err opening query image" << endl;
	}
	calcMoment(0,0, src);
	return 0;
}


void ArMoments::calcMoment (int className, int position, Mat &src){
	/*
		All-in-one function: this function is used both for training and prediction.
		Given a src image depending of the mode it perform segmentation and find the contours on
		the image. Classify/learn based its hu moments and draw the hints 
	*/
    Mat srcGray;
    cvtColor(src, srcGray, CV_RGB2GRAY);

	if(status == 1){//classifying
        //filter image
		src.convertTo(srcGray, -1, contrast, brightness);
        resize(srcGray, srcGray, srcGray.size()/scaleFactor);
        cvtColor(srcGray, srcGray, CV_RGB2GRAY);
        threshold(srcGray, srcThresh, thresh, maxValue,  CV_THRESH_BINARY_INV | CV_THRESH_OTSU); // | CV_THRESH_OTSU
	}
	else{//training
		threshold(srcGray, srcThresh, thresh, maxValue, CV_THRESH_BINARY_INV);
	}
	findContours(srcThresh, contours, hierarchy,  CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
    //cout << "No. Regions: " << contours.size() << endl;
    vector<Obj> candidates; //possible objects, is the regions in a frame that its distance is less that the specified min distance
	for( int idx = 0; idx >= 0; idx= hierarchy[idx][0]){
		Mat queryPart = Mat::zeros(srcThresh.rows, srcThresh.cols, CV_8UC1);
	    drawContours( queryPart, contours, idx, Scalar(255,255,255), CV_FILLED, 8, hierarchy );
	    drawContours( srcThresh, contours, idx, Scalar(255,255,255), CV_FILLED, 8, hierarchy );

		Moments oMoments = moments(queryPart);
        double dArea = oMoments.m00;
		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		if(status == 0){ //training
			double hu[7];
			HuMoments(oMoments, hu);//calculate Hu moments

			cout << "Moments" << endl;
			cout << "Area: " << dArea << " dM01: " << dM01 << " dM10: " << dM10 << endl;
			cout << "Part no: " << position << endl;

			//training
			simpleKnn.train(hu, classes[className], 7);
			//for opencv algorithm
			/*
			trainingClasses.at<float>(position,0) = className;
			for(int j=0; j<7; j++){
				cout << "Hu moment: " << hu[j] << endl;
				trainingData.at<float>(position,j) = hu[j];
			}
			*/
			//save image for displaying
			//resize(src, src, src.size()/2);
			Mat img = Mat::zeros(srcThresh.rows, srcThresh.cols, CV_8UC4);
			drawContours( img, contours, idx, Scalar(rand() % 256 ,rand() % 256,rand() % 256), CV_FILLED, 8, hierarchy );
			resize(img, img, Size(150, 150));
			partsImg[ atoi(classes[className].c_str()) -1] = img;

		}else if(status == 1){//classifying
			if(dArea > minArea && dArea < maxArea){
				//cout << "    -- Region identified --" << endl;
				//finding nearest openCV algorithm
	/*
				for(int i=0; i<7; i++){
					query.at<float>(i,0) = hu[i];
				}
				//knn->findNearest(query, K, result, neighbor, dist);
	*/
				double hu[7];
				double dst;
				HuMoments(oMoments, hu);//calculate Hu moments
				dst = simpleKnn.isThis(searched, hu);
				//string objetcName = simpleKnn.findNearest(hu, dst);
				//cout << "DIST REGION: " << dst<<endl;
				int posX = dM10/dArea;
				int posY = dM01/dArea;
				//draw AR components
				if (dst < minDist){
					//cout << "here" << endl;
					Obj posObj(posX, posY, idx, simpleKnn.getName(searched), dst, hu);//posibble object
					candidates.push_back(posObj);
					AR(posObj, src); //draw candidates
				}
			}
		}
	}
	//cout << "No. candidates: " << candidates.size() << endl;
	if(status == 1){//defining the object
            if (candidates.size() > 1){//getting the smallest
                int candIdx = 0;
                double sDist = candidates[0].dst;
                for(unsigned int i=0; i< candidates.size(); i++){
                    for (unsigned int j=i+1; j<candidates.size(); j++ ){
                        if(candidates[j].dst < sDist){
                            candIdx = j;
                            sDist = candidates[j].dst;
                        }
                    }
                }
                if(sDist < objDist){//is close enough to be considered an object
                    AR(candidates[candIdx], src);
                } else{// if there isn't a point close enough
                    //start remove the candidates that are very similar to other parts of the
                    //assembly
                    //cout<< "candidate" << endl;
                    for(unsigned int i=0; i< candidates.size(); i++){
                        double dst;
                        string objetcName = simpleKnn.findNearest(candidates[i].hu, dst);
                        if(objetcName != classes[searched] && dst < minDist){// if the candidate is very similar to other element of the assembly
                             //cout << "objName: " << objetcName << " searched: " <<  classes[searched] << endl;
                             candidates.erase(candidates.begin()+i);
                        }else{
                             //AR(candidates[i], src);
                        }
                    }
                    //cout << "candidates lenght: " << candidates.size() << endl;
                }
            }else if(candidates.size() == 1 && candidates[0].dst < minDist/2){// is the only one and it is enough close
                AR(candidates[0], src);
            }else{
                cout << "Not Objects identified or not enough close" << endl;
            }
             drawMain(src);//draw main elements of the gui
	}
}


int ArMoments::training(string fileDir){
	cout << "  -TRAINING" << endl;
	int numFiles = 0;
	sys::ls(fileDir, classes);

	for(unsigned int i=0;i<classes.size(); i++){//getting the number of files
		vector<string> files;
		sys::ls(fileDir+"/"+classes[i], files);
		numFiles += files.size();
	}

		cout << "  Total number of files: " << numFiles << endl;
		partsImg = new Mat[numFiles];//asigning size for the array containing the images, for display purposes

        //knn matrices
        //trainingData = Mat::zeros(numFiles, 7, CV_32FC1);
        //trainingClasses = Mat::zeros(numFiles, 1, CV_32FC1);

	status = 0;

	for(unsigned int i=0;i<classes.size(); i++){//looping for categories
		cout << "    " <<classes[i] << endl;
		vector<string> files;
		sys::ls(fileDir+"/"+classes[i], files);

		for(unsigned int j=0; j<files.size(); j++){
			//loading image
			Mat src = imread(fileDir + "/" + classes[i] + "/" + files[j], CV_LOAD_IMAGE_UNCHANGED);
			string fileName = fileDir + "/" + classes[i] + "/" + files[j];
			cout << "        " << fileName << endl;
			if(src.empty()){
				cout << "Error, empty image!" << endl;
				return -1;
			}
			resize(src, src, src.size()/scaleFactor);
			calcMoment(i, i+j, src);
		}
	}
    simpleKnn.printDatabase();
	//KNN training
	//knn->train(trainingData, ml::ROW_SAMPLE, trainingClasses);
	return 0;
}

void ArMoments::AR(Obj obj, Mat &src){
	//string objectName = classes[objectIdx];// more friends
	//string objectName = classes[neighbor.at<float>(0,0)];// closer friend
	//string objectName = classes[result.at<float>(0,0)];//pondered of distance of friend
	//cout << "closer: " << neighbor.at<float>(0,0) << " class: " << classes[neighbor.at<float>(0,0)] << endl;
	//converting string to int
	//int classNumber;
	//stringstream(classes[neighbor.at<float>(0,0)]) >> classNumber;
	//if( dist.at<float>(0,0) < 0.0000000005 && classNumber == searched){//verify if the point is closer enought and is the searched object
		Mat mask = Mat::zeros(src.rows/scaleFactor, src.cols/scaleFactor, CV_8UC4);
		drawContours( mask, contours, obj.idx, partColor, CV_FILLED, 8, hierarchy );// actual
		resize(mask, mask, src.size());
		src += mask;
		putText(src, obj.objName, Point(obj.x*scaleFactor, obj.y*scaleFactor), FONT_HERSHEY_SIMPLEX, 0.8, fontColor );
	//}
}

void ArMoments::nextStep(){
    if (searched < classes.size()-1){
        if(searched == 0){ //first miniature draw
                    assemblyStatus = partsImg[0];
                }
                searched++;
                assemblyStatus += Scalar(-40, -40, -40);
                assemblyStatus += partsImg[searched];
    } else{
        //assemblyStatus = Mat::zeros(assemblyStatus.rows, assemblyStatus.cols, CV_8UC4);
        assemblyStatus = partsImg[0];
        searched = 0;
    }
	cout << "Searched part: " << searched << endl;
}


void ArMoments::drawMain(Mat &src){//Draw main elements of the gui
	if(searched == 0){ //first miniature draw
		assemblyStatus = partsImg[0];
	}
	rectangle(src, Point(0, src.rows- hintsH), src.size(), rectColor, CV_FILLED, 8);//the lenght of the rect is equals to the height of the miniature image
	string text_hint = hints[2];
	if(searched < 2){
		text_hint = hints[searched];
	}
	putText(src, text_hint, Point(5, src.rows -10), FONT_HERSHEY_SIMPLEX, 0.7, fontColor  ); //hint text

	//draw miniatures
	int minX = src.cols - partsImg[searched].cols;//position of the miniature
	int minY = src.rows - partsImg[searched].rows;//position of the miniature
	assemblyStatus.copyTo(src(Rect(minX,minY,partsImg[searched].cols, partsImg[searched].rows))); //draw miniature

}


