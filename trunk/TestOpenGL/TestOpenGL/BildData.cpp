#include "stdafx.h"
#include "BildData.hpp"

BildData::BildData(){
	this->dataSize = 204*204;
	this->disData = new float[dataSize];
	this->intData = new float[dataSize];
    this->ampData = new float[dataSize];
}

BildData::~BildData(){
	delete [] this->disData;
	delete [] this->intData;
	delete [] this->ampData;
}
