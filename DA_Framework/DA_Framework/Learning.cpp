#include "stdafx.h"
#include "Learning.hpp"

Learning::Learning(){
	spatialCombiEps = 0.18;
	spatialCombiMinPts = 1;

	associateVariance = 0.02;
	associateRate = 0.76;
	this->isAssSuccess = false;

	isTKFilter = true;
	isQKFilter = false;

	this->pObject = new Object();
	tempR = Mat::eye(3,3,CV_32FC1);
	tempT = Mat::zeros(3,1,CV_32FC1);

	updateDThreshold = 0.004;
	updateTThreshold = 25;

	sumMaxValue = -1;

	initKalmanFilter();
}

void Learning::initKalmanFilter(){
	tFilter = KalmanFilter(6,3,0);
	tFilter.statePre = Mat::zeros(6,1,CV_32FC1);

	//setIdentity(kFilter.transitionMatrix);
	tFilter.transitionMatrix = *(Mat_<float>(6,6) <<1,0,0,1,0,0,  0,1,0,0,1,0,  0,0,1,0,0,1,  0,0,0,1,0,0,  0,0,0,0,1,0,  0,0,0,0,0,1);
	setIdentity(tFilter.measurementMatrix);

	setIdentity(tFilter.processNoiseCov, Scalar::all(0.1));
	setIdentity(tFilter.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(tFilter.errorCovPost, Scalar::all(0.1));

	//init the quaternion kalmanfilter
	qFilter = KalmanFilter(3,3,0);
	qFilter.statePre = Mat::zeros(3,1,CV_32FC1);
	setIdentity(qFilter.transitionMatrix);
	setIdentity(qFilter.measurementMatrix);

	setIdentity(qFilter.processNoiseCov, Scalar::all(0.1));
	setIdentity(qFilter.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(qFilter.errorCovPost, Scalar::all(0.1));
}

//void Learning::updateKalmanFilter(Mat measure, Mat &estimate){
//	for(int i=0;i<10;i++){
//		Mat prediction = tFilter.predict();
//		estimate = tFilter.correct(measure);
//		//cout<<"loop: "<<i<<"  "<<estimate<<endl;
//	}
//}

void Learning::setTrainingFeatures(vector<PMDPoint> features){
	this->trainingFeatures = features;
}

void Learning::setCurrentBildData(BildData *curBildData){
	this->curBildData = curBildData;
}

void Learning::setHistoricalBildData(BildData *hisBildData){
	this->hisBildData = hisBildData;
}

void Learning::findMaxPMDPointSet(vector<vector<PMDPoint>> features, vector<PMDPoint> &maxSet){
	int maxSize = 0;
	int maxSizeIndex = 0;
	for(int i=0;i<features.size();i++){
		if(features[i].size()>maxSize){
			maxSize = features[i].size();
			maxSizeIndex = i;
		}
	}
	if(maxSize > 0){
		maxSet = features[maxSizeIndex];
	}
}

void Learning::findObjectFeatures(){
	vector<vector<PMDPoint>> caliResult;
	ImageProcess::DBSCANPMDPoint(caliResult, 
								 this->trainingFeatures, 
								 this->spatialCombiEps, 
								 this->spatialCombiMinPts);
	this->curBildData->features.clear();
	this->findMaxPMDPointSet(caliResult, this->curBildData->features);
}


void Learning::findAssociations(){
	if(this->curBildData->features.size()>0 && this->hisBildData->features.size()>0){
		float avrDis, disPE;
		this->isAssSuccess = ImageProcess::featureAssociatePMD(this->hisBildData->features, 
															   this->curBildData->features, 
															   this->associateVariance, 
															   this->associateRate,
															   this->hisAssPoints, 
															   this->curAssPoints, 
															   avrDis, disPE, this->sumMaxValue);
		if(avrDis < 0.02){			
			this->associateVariance = avrDis;
		} else {
			this->associateVariance = 0.02;
		}		
	}
}

void Learning::findTransformation(){
	vector<Point3f> his3DResult, cur3DResult;
	vector<Point2f> his2DResult, cur2DResult;
	ImageProcess::decPMDPointVector(this->hisAssPoints, his3DResult, his2DResult);
	ImageProcess::decPMDPointVector(this->curAssPoints, cur3DResult, cur2DResult);
	if(this->isTKFilter){
		ImageProcess::UQFindRAndT(his3DResult, cur3DResult, this->tempR, this->tempT, &(this->tFilter));
	} else {
		ImageProcess::UQFindRAndT(his3DResult, cur3DResult, this->tempR, this->tempT);
	}
}

void Learning::updateObject(){
	vector<Point3f> features3D;
	vector<Point2f> features2D;
	ImageProcess::decPMDPointVector(this->curBildData->features, features3D, features2D);
	this->pObject->update(features3D, this->tempR, this->tempT, this->updateDThreshold, this->updateTThreshold);
}

void Learning::updateObject(vector<PMDPoint> features, Mat R, Mat T){
	vector<Point3f> features3D;
	vector<Point2f> features2D;
	ImageProcess::decPMDPointVector(features, features3D, features2D);
	this->pObject->update(features3D, R, T, this->updateDThreshold, this->updateTThreshold);
}

float Learning::getCorresRate(float e){
	if(this->curBildData->features.size()<=0){
		return -1;
	} else {
		if(this->pObject->nodeList.size()<=0){
			return -1;
		} else {
			int coorPoints = 0;
			int graphSize = this->pObject->nodeList.size();
			int featureSize = this->curBildData->features.size();
			for(int i=0;i<graphSize;i++){
				Mat oldPoint = Mat(this->pObject->nodeList[i]->getPoint());
				Mat newPoint = this->tempR*oldPoint + this->tempT;
				for(int j=0;j<featureSize;j++){
					if(fabs(newPoint.at<float>(0,0) - this->curBildData->features[j].coord.x) < e &&
					   fabs(newPoint.at<float>(1,0) - this->curBildData->features[j].coord.y) < e &&
					   fabs(newPoint.at<float>(2,0) - this->curBildData->features[j].coord.z) < e){
						   // if find the coorespondence point
						   coorPoints ++;
							   break;
					} 
				}
			}
			float rate = float(coorPoints)/featureSize;
			cout<<"The coorespondence rate: "<<rate<<" = "<<coorPoints<<"/"<<featureSize<<endl;
			return rate;
		}
	}
}

Point2i Learning::getCenter(){
	int size = this->curBildData->features.size();
	int x = 0;
	int y = 0;
	for(int i=0;i<size;i++){
		x += this->curBildData->features[i].index.x;
		y += this->curBildData->features[i].index.y;
	}
	if(size == 0){
		return Point2i(0,0);
	} else {
		return Point2i(x,y);
	}
}