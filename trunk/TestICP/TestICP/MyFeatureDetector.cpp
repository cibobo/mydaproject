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
	
	//unsigned char tempData[H_BILDSIZE*V_BILDSIZE];
	//unsigned char drawData[H_BILDSIZE*V_BILDSIZE*3];

	//transFloatTo3Char(detectedData, drawData, balance, contrast);
	//drawMat =new Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);
	//cvCvtColor(&detectedMat, drawMat, CV_GRAY2BGR );
	//detectedMat.copyTo(drawMat);

	IplImage *object = &IplImage(detectedMat);
	IplImage *object_color = cvCreateImage(cvGetSize(object), 8, 3);
	//The resource matrix of this function must have a head of IplImage
	cvCvtColor(object, object_color, CV_GRAY2BGR);

	float oldContrast = contrast;

	for(int i=0;i<this->maxLoopCount;i++){
		//transFloatToChar(detectedData, tempData, balance, contrast);
		//detectedMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC1, tempData);
		detectedMat = detectedMat * (oldContrast/contrast);
		oldContrast = contrast;

		keypoints.clear();
		StarDetector detector = StarDetector(MAXSIZE, detecParam, LINETHRESHOLDPROJECTED, LINETHRESHOLDBINARIZED, SUPPRESSNONMAXSIZE);
		detector(detectedMat, keypoints);

		int vectorSize = keypoints.size();

		// OpenCV Draw
		//Mat drawMat;
		//detectedMat.copyTo(drawMat);


		cout<<"find "<<vectorSize<<" features!"<<endl;
		drawMat = Mat(object_color);
		for(int j=0;j<vectorSize;j++){
			circle(drawMat, keypoints[j].pt, 1, Scalar(0,0,255,0), -1); 
		}
		imshow("STARDetector", drawMat);
		//cvShowImage("STARDetector", object_color);


		int controllValue = brightnessControllSTAR();
		if(controllValue>0){
			cout<<"The Brightness Controll is failed! "<<controllValue<<endl;
			break;
		}

		Sleep(100);
	}
}

void MyFeatureDetector::setDetectedData(float *data){
	//memcpy(this->detectedData, data, sizeof(float)*H_BILDSIZE*V_BILDSIZE);
	this->detectedData = data;
}

void MyFeatureDetector::setDetectedMat(Mat data){
	data.copyTo(this->detectedMat);
}

/*
 * return false, if the brighntness controll is failed; else return true.
 */
int MyFeatureDetector::brightnessControllSTAR(){
	int MINFEATURECOUNT = this->maxFeatureCount;
	int MAXFEATURECOUNT = this->minFeatureCount;

	float MINSTANDARDENERGY = 1500.0;
	float MAXSTANDARDENERGY = 3500.0;

	float MINCONTRAST = 2;
	float MAXCONTRAST = 18;

	float MINRESPONSETHRESHOLD = 40;
	float MAXRESPONSETHRESHOLD = 130;

	double energie = 0;

	//if lesser than 7 features have been found
	if(keypoints.size() < MINFEATURECOUNT){
		//calculate the Energy of the frame
		energie = 0;
		//for(int k = 0;k<H_BILDSIZE*V_BILDSIZE;k++){
		//	energie += tempData[k];
		//}

		for(int i=0;i<detectedMat.rows;i++){
			Mat col = detectedMat.rowRange(i,i+1);
			Scalar avrCol = mean(col);
			energie += avrCol(0);
		}
		cout<<"The energie: "<<energie<<endl;
		//energie = energie*contrast + balance*204*204;
		
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
			detecParam -= 5;
			if(detecParam < MINRESPONSETHRESHOLD){
				//break;
				return 13;
			}
		}						
	} else if(keypoints.size() > MAXFEATURECOUNT){
		if(detecParam > MAXRESPONSETHRESHOLD){
			//if the detecParam is too big
			//break;
			return 21;
		} else {
			//else increase the detecParam
			detecParam += 4;
		}
	} else {
		//break;
		return 31;
	}
	return 0;
}

void MyFeatureDetector::getFeaturePoints(vector<Point2f> &features, float eps){
	vector<vector<Point2f>> groupFeatures;
	// to save the set Nr. for each feature
	vector<int> pointer;
	// the beginning set Nr. is -1 
	for(int i=0;i<keypoints.size();i++){
		pointer.push_back(-1);
	}
	for(int i=0;i<keypoints.size();i++){
		vector<Point2f> temp;
		int index;
		// if the set Nr. for the current feature is not be set
		if(pointer.at(i) == -1){
			// create a new set, and add the current feature to it
			temp.push_back(keypoints[i].pt);
			// get the set Nr.
			index = groupFeatures.size();
			for(int j=0;j<keypoints.size();j++){
				//calculate the distance between two features
				float xDis = fabs(keypoints[i].pt.x - keypoints[j].pt.x);
				float yDis = fabs(keypoints[i].pt.y - keypoints[j].pt.y);
				//if they are too close
				if((xDis*xDis + yDis*yDis)<eps*eps){
					// if the feature j is not include in this set
					if(pointer.at(j) != index){
						// add the feature j into the set of close features for feature i 
						temp.push_back(keypoints[j].pt);
						// set the set Nr. for feature j
						pointer.at(j) = index;
					}
				}
			}
			// add the new set to the groupFeatures
			groupFeatures.push_back(temp);
		// if the set Nr. for the current feature has been already set 
		} else {
			// get the set Nr.
			index = pointer.at(i);
			// get the set
			temp = groupFeatures.at(index);
			for(int j=0;j<keypoints.size();j++){
				//calculate the distance between two features
				float xDis = fabs(keypoints[i].pt.x - keypoints[j].pt.x);
				float yDis = fabs(keypoints[i].pt.y - keypoints[j].pt.y);
				//if they are too close
				//if(xDis<eps && yDis<eps){
				if((xDis*xDis + yDis*yDis)<eps*eps){
					// if the feature j is not include in this set
					if(pointer.at(j) != index){
						temp.push_back(keypoints[j].pt);
						pointer.at(j) = index;
					}
				}
			}
			// reset the set of groupFeatures 
			groupFeatures.at(index) = temp;
		}
	}

	for(int i=0;i<groupFeatures.size();i++){
		Mat rowMat = Mat(groupFeatures[i]);
		Scalar avr = mean(rowMat);
		Point2f point = Point2f(avr[0], avr[1]);
		features.push_back(point);
	}

}