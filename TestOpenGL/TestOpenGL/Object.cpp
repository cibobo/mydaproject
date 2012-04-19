#include "stdafx.h"
#include "Object.hpp"

Object::Object(){
	sumR = Mat::eye(3,3,CV_32FC1);
	sumT = Mat::zeros(3,1,CV_32FC1);

	tFilter = KalmanFilter(6,3,0);
	qFilter = KalmanFilter(3,3,0);

	initKalmanFilter();
}


Object::~Object(){
}

void Object::update(vector<Point3f> points, Mat R, Mat T){
	curR = R;
	curT = T;

	//updateKalmanFilter();

	updateGraph(points, curR, curT);
	
	//if lesser than 3 fixed node have been found, which means, that no plane can be identified. Than reset the rotationsmatrix
	if(this->fixNodeCount>=3){
		sumR *= R;
		sumT += T;
	}
	//cout<<"The number of fixed nodeeeeeeeeeeeeeeeeeeeeeeee: "<<this->fixNodeCount<<endl;
}

void Object::initKalmanFilter(){
	tFilter.statePre = Mat::zeros(6,1,CV_32FC1);

	//setIdentity(kFilter.transitionMatrix);
	tFilter.transitionMatrix = *(Mat_<float>(6,6) <<1,0,0,1,0,0,  0,1,0,0,1,0,  0,0,1,0,0,1,  0,0,0,1,0,0,  0,0,0,0,1,0,  0,0,0,0,0,1);
	setIdentity(tFilter.measurementMatrix);

	setIdentity(tFilter.processNoiseCov, Scalar::all(0.1));
	setIdentity(tFilter.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(tFilter.errorCovPost, Scalar::all(0.1));

	//init the quaternion kalmanfilter
	qFilter.statePre = Mat::zeros(3,1,CV_32FC1);
	setIdentity(qFilter.transitionMatrix);
	setIdentity(qFilter.measurementMatrix);

	setIdentity(qFilter.processNoiseCov, Scalar::all(0.1));
	setIdentity(qFilter.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(qFilter.errorCovPost, Scalar::all(0.1));
}

void Object::updateKalmanFilter(){
	for(int i=0;i<10;i++){
		Mat prediction = tFilter.predict();
		curT = tFilter.correct(curT);
		//cout<<"loop: "<<i<<"  "<<estimate<<endl;
	}

	for(int i=0;i<3;i++){
		Mat prediction = qFilter.predict();
		curR = qFilter.correct(curR);
		//cout<<"loop: "<<i<<"  "<<estimate<<endl;
	}
}

void Object::getMiddelPoint(Point3f &mid){
	mid = Point3f(0,0,0);
	for(int i=0;i<this->fixedNodeList.size();i++){
		mid += fixedNodeList[i]->getPoint();
	}

	if(this->fixNodeCount > 0){
		mid.x = mid.x/this->fixedNodeList.size();
		mid.y = mid.y/this->fixedNodeList.size();
		mid.z = mid.z/this->fixedNodeList.size();
	}
}