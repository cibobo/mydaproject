#include "stdafx.h"
#include "FrameIterator.hpp"

FrameIterator::FrameIterator(){
	this->bestValue = -1;

	this->qualityCheckThreshold = 0.005;
	this->qualityCheckMinAcceptRate = 0.35;
	
	DATABUFFERLENGTH = 2;
	MAXJUMPEDFEATURES = 5;
	jumpedFeatures = 0;
}

void FrameIterator::initDataBuffer(BildData *bildData){
	if(bildDataBuffer.size()>=DATABUFFERLENGTH){
		//delete the first element from the list
		bildDataBuffer.pop_front();
	}
	//add the new element into the end of the List
	bildDataBuffer.push_back(bildData);
}

void FrameIterator::updateDataBuffer(BildData *bildData){
	// Pop the first element of the Buffer, and the destructor will be called automatically 
	// if sepcial process begin, the old data would not be removed
	if(jumpedFeatures<1){
		bildDataBuffer.pop_front();
	}
	bildDataBuffer.push_back(bildData);
}

BildData* FrameIterator::getCurrentBildData(){
	return this->bildDataBuffer.back();
}

BildData* FrameIterator::getHistoricalBildData(){
	return this->bildDataBuffer.front();
}

void FrameIterator::framesControl(Learning *pLearning){
	if(pLearning->getCorresRate(this->qualityCheckThreshold) > qualityCheckMinAcceptRate 
		&& pLearning->isAssSuccess){
		cout<<"The frame is not noised: "<<endl;
		pLearning->updateObject();

		if(jumpedFeatures > 0){
			bildDataBuffer.erase(--(--bildDataBuffer.end()));
			jumpedFeatures = 0;
			this->bestValue = -1;
		}
	} else {
		cout<<"The frame has big noise, and will be throw out!"<<endl;
		if(!pLearning->isAssSuccess){
			pLearning->sumMaxValue = 0;
		}
		// if the noised frame has a besser messurement
		if(pLearning->sumMaxValue > this->bestValue){
			this->bestValue = pLearning->sumMaxValue;
			// save the current rotation and translation matrix
			pLearning->tempR.copyTo(this->bestR);
			pLearning->tempT.copyTo(this->bestT);
			
			//TODO: using all features to update object
			this->bestFeatures = this->getCurrentBildData()->features;

			// if this noised frame is not the first frame, that means, there is already a noised frame before this frame 
			if(jumpedFeatures > 0){
				// remove the frame before
				bildDataBuffer.erase(--(--bildDataBuffer.end()));
			}
		} else {
			bildDataBuffer.pop_back();
		}
		cout<<"Jumped! The jumped features: "<<jumpedFeatures<<endl;
		jumpedFeatures++;

		if(jumpedFeatures>MAXJUMPEDFEATURES){
			cout<<"The frame has noise, but the maximal number of the allowed jumped frames are arrived!"<<endl;

			pLearning->updateObject(this->bestFeatures, this->bestR, this->bestT);

			jumpedFeatures = 0;
			this->bestValue = -1;
		}
	}
}