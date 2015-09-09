#include"../include/KNN.h"



void KnnSimple::train(double *descriptor, string className, int descSize){
    cout<< "--ADDING OBJECT-- " << className << endl;
    this->descSize = descSize;
    classes.push_back(className);

    double *arr = new double[descSize];
    for(unsigned int i=0; i<descSize; i++ ){
        arr[i] = descriptor[i];
    }


    descriptors.push_back(arr);
}


vector<double*> KnnSimple::getDesc(){
    return descriptors;
}

void KnnSimple::printDatabase(){
    cout << "  --PRINTING KNN DATABASE--" << endl;
    for(unsigned int i=0; i< classes.size(); i++){
        cout << "Class name: " << classes[i] << endl;
        for (unsigned int j=0; j< descSize -1; j++){
            cout << "  - " << descriptors[i][j] << endl;
        }
    }

}


string KnnSimple::findNearest(double* query, double &dst){
    //cout<<"  --Finding KNN nearest--" << endl;

    double dist[classes.size()];//distance of the objects

    //calculate distances
    for(unsigned int i=0; i< classes.size(); i++){
        dist[i] = euclideanDist(query, descriptors[i]);
    }

    //get the index with the smalles distance
    int idx=0;//closets index
    double sDist = dist[0]; //smalles dist
    for(unsigned int i=0; i< classes.size(); i++){
        for (unsigned int j=i+1; j<classes.size(); j++ ){
            if(dist[j] < sDist){
                idx = j;
                sDist = dist[j];
            }
        }
    }
    //cout<< "Closest: " <<classes[idx]<< " at: " << dist[idx]<< endl;
    dst = dist[idx];
    return classes[idx];
}

//compare the moments of the specified class with the query
double KnnSimple::isThis(int classNo, double* query){

    return euclideanDist(query, descriptors[classNo]);
}


//return the name of a class based on the index
string KnnSimple::getName(int classIdx){
    return classes[classIdx];
}


//calculates eucludean distance of a multiple dimension vector. The number of dimensions
// have to be explicit in the training
double KnnSimple::euclideanDist(double* query, double* descriptor){
    double distTemp =0;
    for (unsigned int j=0; j< descSize -1; j++){
        distTemp += pow((query[j] - descriptor[j]), 2);
    }
    return sqrt(distTemp);
}

