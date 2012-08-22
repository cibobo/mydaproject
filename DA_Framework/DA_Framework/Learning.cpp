#include "stdafx.h"
#include "Learning.hpp"

Learning::Learning(){
	spatialCombiEps = 0.18;
	spatialCombiMinPts = 1;

	associateVariance = 0.02;
	associateRate = 0.76;
}

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
		float avrDis, disPE, sumP;
		bool isAssSuccess = ImageProcess::featureAssociatePMD(this->hisBildData->features, 
															  this->curBildData->features, 
															  this->associateVariance, 
															  this->hisAssPoints, 
															  this->curAssPoints, 
															  avrDis, disPE, sumP);
		if(avrDis < 0.02){			
			this->associateVariance = avrDis;
		} else {
			this->associateVariance = 0.02;
		}		
	}
}