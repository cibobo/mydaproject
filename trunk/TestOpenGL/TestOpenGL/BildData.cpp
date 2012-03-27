#include "stdafx.h"
#include "BildData.hpp"

BildData::BildData(){
	this->width = 204;
	this->length = 204;
	int dataSize = width * length;
	this->disData = new float[dataSize];
	this->intData = new float[dataSize];
    this->ampData = new float[dataSize];
	this->threeDData = new float[3*dataSize];
	//clusterLabel = Mat::zeros(dataSize, 1, CV_8UC1);
}

BildData::~BildData(){
	delete [] this->disData;
	delete [] this->intData;
	delete [] this->ampData;
	delete [] this->threeDData;
}

void BildData::disCorrection(){
	for(int i=0;i<width;i++){
		for(int j=0;j<length;j++){
			////claculate the distance between the current point and the middel point
			//float bSquare = (i-102)*(i-102) + (j-102)*(j-102);
			////get the distance of the current point
			//float c = this->disData[i*width + j];
			////calculate the actual distance and save it back into the disData
			//this->disData[i*width + j] = sqrt(c*c - bSquare/10000);
			//this->disData[i*width + j] *= 0.1;
		}
	}
}

//BildData* BildData::operator =(BildData *oldData){
//	BildData *newData = new BildData();
//
//	newData->width = oldData->width;
//	newData->length = oldData->length;
//
//	int dataSize = sizeof(float)*width*length;
//	memcpy(newData->disData, oldData->disData, dataSize);
//	memcpy(newData->ampData, oldData->ampData, dataSize);
//	memcpy(newData->intData, oldData->intData, dataSize);
//
//	newData->features = oldData->features;
//
//	return newData;
//}

