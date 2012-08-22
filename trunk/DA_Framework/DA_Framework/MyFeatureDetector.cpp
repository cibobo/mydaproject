#include "stdafx.h"
#include "MyFeatureDetector.hpp"

MyFeatureDetector::MyFeatureDetector(){
	MINFEATURECOUNT = 42;
	MAXFEATURECOUNT = 65;
	MAXDETECTLOOPS = 30;
	BEGINBRIGHTNESS = 200;
	BEGINCONTRAST = 10;

	MAXSIZE = 8;
	responseThreshold = 70;
	LINETHRESHOLDPROJECTED = 5;
	LINETHRESHOLDBINARIZED = 6;
	SUPPRESSNONMAXSIZE = 1;

	balance = this->BEGINBRIGHTNESS;
	contrast = this->BEGINCONTRAST;

	MINSTANDARDENERGY = 300000.0;
	MAXSTANDARDENERGY = 450000.0;

	MINCONTRAST = 2;
	MAXCONTRAST = 18;

	MINRESPONSETHRESHOLD = 40;
	MAXRESPONSETHRESHOLD = 130;

	planeCombiEps = 5;
	planeCombiMinPts = 2;

	detectedMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC1);
}

void MyFeatureDetector::usingSTAR(){
	for(int i=0;i<MAXDETECTLOOPS;i++){
		ImageProcess::transFloatToMat(detectedData->filteredData, detectedMat, balance, contrast);

		keypoints.clear();
		StarDetector detector = StarDetector(MAXSIZE, 
									         responseThreshold, 
											 LINETHRESHOLDPROJECTED, 
											 LINETHRESHOLDBINARIZED, 
											 SUPPRESSNONMAXSIZE);
		detector(detectedMat, keypoints);

		int vectorSize = keypoints.size();

		int result = brightnessControllSTAR();
		if(result > 0){
			if(result != 31){
				cout<<"The Brightness Controll is failed: "<<result<<endl;
			}
			break;
		}
	}
}

void MyFeatureDetector::setDetectedData(BildData *data){
	this->detectedData = data;
}

int MyFeatureDetector::brightnessControllSTAR(){
	double energie = 0;
	//if lesser than 7 features have been found
	if(keypoints.size() < MINFEATURECOUNT){
		//calculate the Energy of the frame
		energie = 0;

		for(int i=0;i<H_BILDSIZE;i++){
			for(int j=0;j<V_BILDSIZE;j++){
				energie += this->detectedMat.at<unsigned char>(i,j);
			}
		}

		//compare the energy with the standard energy
		if(energie < MINSTANDARDENERGY){
			//if the frame is too dark
			//float eFactor = energie/MINSTANDARDENERGY;
			contrast -= 0.5;
			if(contrast<MINCONTRAST) {
				cout<<"Algo fehld, break!"<<endl;
				contrast = MINCONTRAST;
				//break;
				return 11;
			}
		} else if(energie > MAXSTANDARDENERGY){
			//if the frame is too bright	
			contrast += 0.5;
			if(contrast>MAXCONTRAST) {
				cout<<"Algo fehld, break!"<<endl;
				contrast = MAXCONTRAST;
				//break;
				return 12;
			}
		} else {
			//the energy is acceptable, but still can not find enough features. So change the parameter of STAR Detector
			responseThreshold -= 5;
			if(responseThreshold < MINRESPONSETHRESHOLD){
				//break;
				return 13;
			}
		}						
	} else if(keypoints.size() > MAXFEATURECOUNT){
		if(responseThreshold > MAXRESPONSETHRESHOLD){
			//if the detecParam is too big
			//break;
			return 21;
		} else {
			//else increase the detecParam
			responseThreshold += 4;
		}
	} else {
		//The brightness controll is successful!
		//break;
		return 31;
	}
	return 0;
}

void MyFeatureDetector::planeFeaturesCombination(){
	this->planeFeatureGroups.clear();
	if(this->keypoints.size()>0){
		vector<Point2f> tempFeatures;
		for(int i=0;i<this->keypoints.size();i++){
			tempFeatures.push_back(keypoints[i].pt);
		}
		ImageProcess::DBSCAN(this->planeFeatureGroups, tempFeatures, this->planeCombiEps, this->planeCombiMinPts);
	}
	int size = this->planeFeatureGroups.size();
}

void MyFeatureDetector::createPMDFeatures(){
	this->planeFeaturesCombination();
	this->PMDFeatures.clear();
	for(int i=0;i<planeFeatureGroups.size();i++){
		float avrX2D = 0;
		float avrY2D = 0;

		float avrX3D = 0;
		float avrY3D = 0;
		float avrZ3D = 0;
		int groupSize = planeFeatureGroups[i].size();
		for(int j=0;j<groupSize;j++){
			//TODO: Why is the x and y exchanged
			//int indexI = int(groupFeatures[i][j].pt.y);
			//int indexJ = int(groupFeatures[i][j].pt.x);
			int indexI = int(planeFeatureGroups[i][j].y);
			int indexJ = int(planeFeatureGroups[i][j].x);

			avrX2D += indexJ;
			avrY2D += indexI;

			avrX3D += detectedData->threeDData[(indexI*204 + indexJ)*3];
			avrY3D += detectedData->threeDData[(indexI*204 + indexJ)*3 + 1];
			avrZ3D += detectedData->threeDData[(indexI*204 + indexJ)*3 + 2];
		}

		Point2f p2(avrX2D/groupSize, avrY2D/groupSize);
		Point3f p3(avrX3D/groupSize, avrY3D/groupSize, avrZ3D/groupSize);
		
		//currentBildData->features2D.push_back(p2);
		////currentBildData->features.push_back(p3);
		//summerizedFeatures.push_back(p3);
		
		PMDPoint newPMDPoint = PMDPoint(p3,p2);
		PMDFeatures.push_back(newPMDPoint);
	}
}