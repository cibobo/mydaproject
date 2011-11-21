#include "stdafx.h"
#include "DistanceFilter.hpp"

DistanceFilter::DistanceFilter(){
	this->origArray = new float[204*204];
}

DistanceFilter::DistanceFilter(float *dis){
	this->origArray = new float[204*204];
	for(int i=0;i<204*204;i++){
		origArray[i] = dis[i];
	}
}

DistanceFilter::~DistanceFilter(){
	delete []this->origArray;
}

void DistanceFilter::Filte(float *dis, float *src, float *dst){
	float eps = 0.1;
	for(int i=0;i<204*204;i++){
		if(fabs(dis[i]-origArray[i]) < eps){
			dst[i] = 0;
		} else {
			dst[i] = src[i];
		}
	}
}
