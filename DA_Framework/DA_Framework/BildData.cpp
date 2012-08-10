#include "stdafx.h"
#include "BildData.hpp"

PMDPoint::PMDPoint(Point3f coord, Point2f index){
	this->coord = coord;
	this->index = index;
}

BildData::BildData(){
	this->width = H_BILDSIZE;
	this->length = V_BILDSIZE;
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
