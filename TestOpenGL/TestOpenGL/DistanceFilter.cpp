#include "stdafx.h"
#include "DistanceFilter.hpp"

DistanceFilter::DistanceFilter(){
	this->origArray = new float[204*204];
	this->eps = 0.05;
	this->diffRate = 0.05;
}

DistanceFilter::DistanceFilter(float *dis){
	this->origArray = new float[204*204];
	for(int i=0;i<204*204;i++){
		origArray[i] = dis[i];
	}
	this->eps = 0.05;
	this->diffRate = 0.05;
}

DistanceFilter::DistanceFilter(float *dis, float epsilon, float rate){
	this->origArray = new float[204*204];
	for(int i=0;i<204*204;i++){
		origArray[i] = dis[i];
	}
	this->eps = epsilon;
	this->diffRate = rate;
}

DistanceFilter::~DistanceFilter(){
	delete []this->origArray;
}

void DistanceFilter::Upgrade(float *dis){
	for(int i=0;i<204*204;i++){
		//if(fabs(dis[i]-origArray[i]) < this->eps){
			this->origArray[i] = (this->origArray[i] + dis[i])/2;
		//}
	}
}

bool DistanceFilter::Filte(float *dis, float *src, float *dst){
	this->eps = 0.05;
	// count how many data are different
	int diffCount = 0;
	this->diffRate = 0.22;
	for(int i=0;i<204*204;i++){
		if(fabs(dis[i]-origArray[i]) < eps){
			dst[i] = 0;
		} else {
			dst[i] = src[i];
			diffCount ++;
		}
	}

	float factor = (float)diffCount/(204*204);
	std::cout<<"The different rate is: "<<factor<<std::endl;
	if(factor > diffRate){
		return true;
	} else {
		return false;
	}
}
