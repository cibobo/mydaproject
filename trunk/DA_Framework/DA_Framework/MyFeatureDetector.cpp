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

	detectedMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC1);
	testMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC1);
}

void MyFeatureDetector::usingSTAR(){
	unsigned char tempData[H_BILDSIZE*V_BILDSIZE];

	for(int i=0;i<MAXDETECTLOOPS;i++){
		ImageProcess::transFloatToChar(detectedData, tempData, balance, contrast);
		detectedMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC1, tempData);
		ImageProcess::transFloatToMat(detectedData, testMat, balance, contrast);

		keypoints.clear();
		StarDetector detector = StarDetector(MAXSIZE, 
									         responseThreshold, 
											 LINETHRESHOLDPROJECTED, 
											 LINETHRESHOLDBINARIZED, 
											 SUPPRESSNONMAXSIZE);
		detector(detectedMat, keypoints);

		int vectorSize = keypoints.size();

		int result = brightnessControllSTAR(tempData);
		if(result > 0){
			if(result != 31){
				cout<<"The Brightness Controll is failed: "<<result<<endl;
			}
			break;
		}
	}
}

void MyFeatureDetector::setDetectedData(float *data){
	this->detectedData = data;
}

int MyFeatureDetector::brightnessControllSTAR(unsigned char *tempData){
	double energie = 0;
	//if lesser than 7 features have been found
	if(keypoints.size() < MINFEATURECOUNT){
		//calculate the Energy of the frame
		energie = 0;
		double eTest = 0;
		for(int i=0;i<H_BILDSIZE;i++){
			for(int j=0;j<V_BILDSIZE;j++){
				eTest += this->testMat.at<char>(i,j);
			}
		}
		for(int k = 0;k<H_BILDSIZE*V_BILDSIZE;k++){
			energie += tempData[k];
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