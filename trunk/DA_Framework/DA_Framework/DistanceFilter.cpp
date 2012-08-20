#include "stdafx.h"
#include "DistanceFilter.hpp"

DistanceFilter::DistanceFilter(){
	this->origArray = new float[H_BILDSIZE*V_BILDSIZE];
	this->eps = 0.05;
	this->diffRate = 0.05;
}

DistanceFilter::DistanceFilter(float *dis){
	this->origArray = new float[H_BILDSIZE*V_BILDSIZE];
	for(int i=0;i<H_BILDSIZE*V_BILDSIZE;i++){
		origArray[i] = dis[i];
	}
	this->eps = 0.05;
	this->diffRate = 0.05;
}

DistanceFilter::DistanceFilter(float *dis, float epsilon, float rate){
	this->origArray = new float[H_BILDSIZE*V_BILDSIZE];
	for(int i=0;i<H_BILDSIZE*V_BILDSIZE;i++){
		origArray[i] = dis[i];
	}
	this->eps = epsilon;
	this->diffRate = rate;
}

DistanceFilter::DistanceFilter(BildData *bildData){
	this->origArray = new float[H_BILDSIZE*V_BILDSIZE];
	for(int i=0;i<H_BILDSIZE*V_BILDSIZE;i++){
		origArray[i] = bildData->threeDData[i*3+2];
	}
	this->eps = 0.05;
	this->diffRate = 0.05;
}

DistanceFilter::DistanceFilter(BildData *bildData, float epsilon, float rate){
	this->origArray = new float[H_BILDSIZE*V_BILDSIZE];
	for(int i=0;i<H_BILDSIZE*V_BILDSIZE;i++){
		origArray[i] = bildData->threeDData[i*3+2];
	}
	this->eps = epsilon;
	this->diffRate = rate;
}

DistanceFilter::~DistanceFilter(){
	delete []this->origArray;
}

void DistanceFilter::Upgrade(float *dis){
	for(int i=0;i<H_BILDSIZE*V_BILDSIZE;i++){
		//if(fabs(dis[i]-origArray[i]) < this->eps){
			this->origArray[i] = (this->origArray[i] + dis[i])/2;
		//}
	}
}

void DistanceFilter::Upgrade(BildData *bildData){
	for(int i=0;i<H_BILDSIZE*V_BILDSIZE;i++){
		//if(fabs(dis[i]-origArray[i]) < this->eps){
			this->origArray[i] = (this->origArray[i] + bildData->threeDData[i*3+2])/2;
		//}
	}
}

bool DistanceFilter::Filte(float *dis, float *src, float *dst){
	this->eps = 0.05;
	// count how many data are different
	int diffCount = 0;
	this->diffRate = 0.17;
	for(int i=0;i<H_BILDSIZE*V_BILDSIZE;i++){
		if(fabs(dis[i]-origArray[i]) < eps){
			dst[i] = 0;
		} else {
			dst[i] = src[i];
			diffCount ++;
		}
	}

	float factor = (float)diffCount/(H_BILDSIZE*V_BILDSIZE);
	std::cout<<"The different rate is: "<<factor<<std::endl;
	if(factor > diffRate){
		return true;
	} else {
		return false;
	}
}

bool DistanceFilter::Filte(BildData *bildData, float *dst){
	//this->eps = 0.1;
	//this->diffRate = 0.08;

	// count how many data are different
	int diffCount = 0;

	//bildData->filted3DData.clear();
	for(int i=0;i<H_BILDSIZE*V_BILDSIZE;i++){
		if(fabs(bildData->threeDData[i*3+2]-origArray[i]) < eps){
			dst[i] = 0;
		} else {
			dst[i] = bildData->ampData[i];
			//Point3f filted3DPoint = Point3f(bildData->threeDData[i*3], bildData->threeDData[i*3 +1], bildData->threeDData[i*3 +2]);
			//Point2f filted2DPoint = Point2f(bildData->threeDData[i*3], bildData->threeDData[i*3 +1]);
			//bildData->filted3DData.push_back(filted3DPoint);
			//bildData->filted2DData.push_back(filted2DPoint);
			//bildData->filtedPointIndex.push_back(i);
			diffCount ++;
		}
	}

	float factor = (float)diffCount/(H_BILDSIZE*V_BILDSIZE);
	std::cout<<"The different rate is: "<<factor<<std::endl;
	if(factor > diffRate){
		return true;
	} else {
		return false;
	}
}

