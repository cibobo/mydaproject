#include "stdafx.h"
#include "MyFeatureDetector.hpp"

MyFeatureDetector::MyFeatureDetector(int maxFeatureCount, int minFeatureCount, int maxLoopCount){
	this->maxFeatureCount = maxFeatureCount;
	this->minFeatureCount = minFeatureCount;
	this->maxLoopCount = maxLoopCount;
	
	balance = 200;
	contrast = 10;
	detecParam = 70;
}

void MyFeatureDetector::usingSTAR(){
	//parameter for STAR Detector
	int MAXSIZE = 8;
	//int RESPONSETHRESHOLD = 70;
	int LINETHRESHOLDPROJECTED = 5;
	int LINETHRESHOLDBINARIZED = 6;
	int SUPPRESSNONMAXSIZE = 1;

	
	namedWindow("STARDetector", CV_WINDOW_AUTOSIZE);
	
	unsigned char tempData[H_BILDSIZE*V_BILDSIZE];
	unsigned char drawData[H_BILDSIZE*V_BILDSIZE*3];

	transFloatTo3Char(detectedData, drawData, balance, contrast);
	Mat drawMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3, drawData);

	for(int i=0;i<this->maxLoopCount;i++){
		transFloatToChar(detectedData, tempData, balance, contrast);
		detectedMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC1, tempData);

		keypoints.clear();
		StarDetector detector = StarDetector(MAXSIZE, detecParam, LINETHRESHOLDPROJECTED, LINETHRESHOLDBINARIZED, SUPPRESSNONMAXSIZE);
		detector(detectedMat, keypoints);

		int vectorSize = keypoints.size();

		// OpenCV Draw
		//Mat drawMat;
		//detectedMat.copyTo(drawMat);


		cout<<"find "<<vectorSize<<" features!"<<endl;
		for(int j=0;j<vectorSize;j++){
			circle(drawMat, keypoints[j].pt, 1, Scalar(0,0,255,0), -1); 
		}
		imshow("STARDetector", drawMat);


		if(!brightnessControllSTAR(tempData)){
			cout<<"The Brightness Controll is failed!"<<endl;
			break;
		}
	}
}

void MyFeatureDetector::setDetectedData(float *data){
	//memcpy(this->detectedData, data, sizeof(float)*H_BILDSIZE*V_BILDSIZE);
	this->detectedData = data;
}

/*
 * return false, if the brighntness controll is failed; else return true.
 */
//bool MyFeatureDetector::brightnessControllSTAR(unsigned char *tempData){
//	//int MINFEATURECOUNT = 22;
//	//int MAXFEATURECOUNT = 45;
//
//	//int MINFEATURECOUNT = 42;
//	//int MAXFEATURECOUNT = 65;
//
//	int MINFEATURECOUNT = this->minFeatureCount;
//	int MAXFEATURECOUNT = this->maxFeatureCount;
//
//	float MINSTANDARDENERGY = 300000.0;
//	float MAXSTANDARDENERGY = 450000.0;
//
//	float MINCONTRAST = 2;
//	float MAXCONTRAST = 18;
//
//	float MINRESPONSETHRESHOLD = 40;
//	float MAXRESPONSETHRESHOLD = 130;
//
//	double energie = 0;
//	//if lesser than 7 features have been found
//	if(keypoints.size() < MINFEATURECOUNT){
//
//#ifdef BRIGHT_TEST
//		cout<<"Case 1111111111111111111111111111111"<<endl;
//#endif
//		//calculate the Energy of the frame
//		energie = 0;
//		for(int k = 0;k<H_BILDSIZE*V_BILDSIZE;k++){
//			energie += tempData[k];
//		}
//
//#ifdef BRIGHT_TEST
//		cout<<"The energy of the data is: "<<energie<<endl;
//#endif
//		//if(energie < 50000) break;
//
//		//compare the energy with the standard energy
//		if(energie < MINSTANDARDENERGY){
//			//if the frame is too dark
//			//float eFactor = energie/MINSTANDARDENERGY;
//			contrast -= 0.5;
//
//#ifdef BRIGHT_TEST
//			//cout<<"Too Dark!! The energy factor is: "<<eFactor<<endl;
//			cout<<"Too Dark! The current contrast is "<<contrast<<endl;
//#endif
//			if(contrast<MINCONTRAST) {
//				cout<<"Algo fehld, break!"<<endl;
//				contrast = MINCONTRAST;
//				//break;
//				return false;
//			}
//		} else if(energie > MAXSTANDARDENERGY){
//			//if the frame is too bright
//			//float eFactor = energie/MAXSTANDARDENERGY;
//			//contrast *= eFactor;
//			//cout<<"Too Bright!! The energy factor is: "<<eFactor<<endl;
//			
//			contrast += 0.5;
//
//#ifdef BRIGHT_TEST
//			cout<<"Too Bright! The current contrast is "<<contrast<<endl;
//#endif
//
//			if(contrast>MAXCONTRAST) {
//				cout<<"Algo fehld, break!"<<endl;
//				contrast = MAXCONTRAST;
//				//break;
//				return false;
//			}
//		} else {
//			//the energy is acceptable, but still can not find enough features. So change the parameter of STAR Detector
//			detecParam -= 5;
//
//#ifdef BRIGHT_TEST
//			cout<<"Brightness is OK!! The current detecParam is "<<detecParam<<endl;
//#endif
//
//			if(detecParam < MINRESPONSETHRESHOLD){
//				//break;
//				return false;
//			}
//		}						
//	} else if(keypoints.size() > MAXFEATURECOUNT){
//
//#ifdef BRIGHT_TEST
//		cout<<"Case 222222222222222222222222222"<<endl;
//#endif
//
//		if(detecParam > MAXRESPONSETHRESHOLD){
//			//if the detecParam is too big
//			//break;
//			return false;
//		} else {
//			//else increase the detecParam
//			detecParam += 4;
//		}
//
//#ifdef BRIGHT_TEST
//		cout<<"Too Many Features!! The current detecParam is "<<detecParam<<endl;
//#endif
//
//	} else {
//
//#ifdef BRIGHT_TEST
//		cout<<"Case 333333333333333333333333333"<<endl;
//#endif
//		//break;
//		return false;
//	}
//	return true;
//}
//

bool MyFeatureDetector::brightnessControllSTAR(unsigned char *tempData){
	int MINFEATURECOUNT = this->maxFeatureCount;
	int MAXFEATURECOUNT = this->minFeatureCount;

	float MINSTANDARDENERGY = 300000.0;
	float MAXSTANDARDENERGY = 450000.0;

	float MINCONTRAST = 2;
	float MAXCONTRAST = 18;

	float MINRESPONSETHRESHOLD = 40;
	float MAXRESPONSETHRESHOLD = 130;

	double energie = 0;
	//if lesser than 7 features have been found
	if(keypoints.size() < MINFEATURECOUNT){
		//calculate the Energy of the frame
		energie = 0;
		for(int k = 0;k<H_BILDSIZE*V_BILDSIZE;k++){
			energie += tempData[k];
		}
		//if(energie < 50000) break;

		//compare the energy with the standard energy
		if(energie < MINSTANDARDENERGY){
			//if the frame is too dark
			//float eFactor = energie/MINSTANDARDENERGY;
			contrast -= 0.5;
			if(contrast<MINCONTRAST) {
				cout<<"Algo fehld, break!"<<endl;
				contrast = MINCONTRAST;
				//break;
				return false;
			}
		} else if(energie > MAXSTANDARDENERGY){
			//if the frame is too bright	
			contrast += 0.5;
			if(contrast>MAXCONTRAST) {
				cout<<"Algo fehld, break!"<<endl;
				contrast = MAXCONTRAST;
				//break;
				return false;
			}
		} else {
			//the energy is acceptable, but still can not find enough features. So change the parameter of STAR Detector
			detecParam -= 5;
			if(detecParam < MINRESPONSETHRESHOLD){
				//break;
				return false;
			}
		}						
	} else if(keypoints.size() > MAXFEATURECOUNT){
		if(detecParam > MAXRESPONSETHRESHOLD){
			//if the detecParam is too big
			//break;
			return false;
		} else {
			//else increase the detecParam
			detecParam += 4;
		}
	} else {
		//break;
		return false;
	}
	return true;
}