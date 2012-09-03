#include "stdafx.h"
#include "Recognition.hpp"

/*****************************
 * Definition of RecognitionResult
 ****************************/
RecognitionResult::RecognitionResult(){
	weight = 0;
	modelIndex = -1;
}

RecognitionResult::RecognitionResult(RecognitionResult::NodePairs nodePair, float weight, int modelIndex){
	this->nodePair = nodePair;
	this->weight = weight;
	this->modelIndex = modelIndex;
}

RecognitionResult::RecognitionResult(RecognitionResult::NodePairs nodePair, Node *center, float weight, int modelIndex){
	this->nodePair = nodePair;
	this->center = center;
	this->weight = weight;
	this->modelIndex = modelIndex;
}

void RecognitionResult::mark(){
	NodePairs::iterator it = this->nodePair.begin();
	for(;it!=this->nodePair.end();it++){
		it->first->color = 9;
		//it->second->color = 9;
	}
}
void RecognitionResult::markColorful(){
	NodePairs::iterator it = this->nodePair.begin();
	int i=1;
	for(;it!=this->nodePair.end();it++, i++){
		if(i>9){
			i=0;
		}
		it->first->color = i;
		it->second->color = i;
	}
}

/*****************************
 * Definition of Recoginition
 *****************************/
Recognition::Recognition(){
	distanceProportion = 0.05;
	nodesCountProportion = 0.5;
	distanceThreshold = 0.008;

	spatialCombiEps = 0.17;
	spatialCombiMinPts = 3;

	isTest = true;
	isLoad = false;
	isCreate = false;

	maxListLength = 5;

	//Object *obj1 = new Object("Box1");
	//this->objectList.push_back(obj1);

	//Object *obj2 = new Object("Box2");
	//this->objectList.push_back(obj2);

	//Object *obj3 = new Object("Box3");
	//this->objectList.push_back(obj3);

	//Object *obj4 = new Object("Box4");
	//this->objectList.push_back(obj4);

	//Object *obj1 = new Object("Box_all4");
	//obj1->setColor(0);
	//this->modelList.push_back(obj1);

	//Object *obj2 = new Object("Box2_all2");
	//obj2->setColor(1);
	//this->modelList.push_back(obj2);



	//graph = new Graph();
	
	timmer = 0;
}

Recognition::~Recognition(){
	for(int i=0;i<this->modelList.size();i++){
		delete modelList[i];
	}
}


void Recognition::loadModels(){
	Object *obj1 = new Object("Box_all4");
	obj1->setColor(0);
	this->modelList.push_back(obj1);

	Object *obj2 = new Object("Box2_all2");
	obj2->setColor(1);
	this->modelList.push_back(obj2);
}

void Recognition::loadModels(std::vector<string> names){
	//If the name vectors are not empty
	if(names.size()>0){
		for(int i=0;i<names.size();i++){
			Object *obj = new Object(names[i].data());
			obj->setColor(i);
			this->modelList.push_back(obj);
		}
	} else {
	//Else, load the default test models
		this->loadModels();
	}
}
/***************************************************************
 *
 * At first only one object will be focused
 *
 **************************************************************/
void Recognition::objectRecognition(std::vector<PMDPoint> inputPoints){
	timmer++;

	this->segResult.clear();
	ImageProcess::DBSCANPMDPoint(this->segResult, inputPoints, this->spatialCombiEps, this->spatialCombiMinPts);

	bool compResult = false;
	//if(caliResult.size()>0 && caliResult[0].size()>0){
	// loop for all possible objekt
	this->graphList.clear();
	for(int j=0;j<this->segResult.size();j++){
		if(this->segResult[j].size()>0){
			RecognitionResult *resultArray = new RecognitionResult[modelList.size()];
			
			vector<Point3f> point3D;
			vector<Point2f> point2D;
			ImageProcess::decPMDPointVector(this->segResult[j], point3D, point2D);

			Graph *graph = new Graph();
			graph->createCompleteGraph(point3D);
			this->graphList.push_back(graph);

			// create the new map for the static date
			NodePairs resultPair;
			for(int i=0;i<this->modelList.size();i++){
				// reset the color
				modelList[i]->setColor(i);

				compResult = graph->isEqual(modelList[i], resultPair, 
											this->distanceProportion, 
											this->nodesCountProportion,
											this->distanceThreshold);
				if(compResult){
					cout<<"Find the Object! "<<i<<endl;
					//int appear = this->Statistic[0].find(i)->second.appear(timmer);
					//cout<<"The appearence rate is: "<<float(appear)/timmer<<endl;

					// show the result

					
					
					if(this->segResult.size() == 1){
						// for just one Object
						updateResultList(i, resultPair);
					//updateObjectPosition(i, resultPair);
					} else {
						// more than 2 objects
						resultArray[i].nodePair = resultPair;
						resultArray[i].weight++;
					}
					
				}

			}


			//int maxIndex = this->findBestResult(j);
			//cout<<"The max Index: "<<maxIndex<<endl;
			//if(maxIndex != -1){
			//	this->graphList[j]->setColor(maxIndex);

			//	//this->resultMap[j][maxIndex].markColorful();
			//	this->resultMap[j][maxIndex].mark();
			//	// transformate the object
			//	updateObjectPosition(resultMap[j][maxIndex].modelIndex, resultMap[j][maxIndex].nodePair);
			//	// set the color of the graph
			//	
			//}


			if(this->segResult.size() == 1){
				// for just one object
				int maxIndex = this->findBestResult();
				cout<<"The max Index: "<<maxIndex<<endl;
				if(maxIndex != -1){
					graphList[j]->setColor(resultList[maxIndex].modelIndex);
					this->resultList[maxIndex].mark();
					// transformate the object
					updateObjectPosition(resultList[maxIndex].modelIndex, resultList[maxIndex].nodePair);
				}
			} else {
				// for more objects
				bool isfind = false;
				for(int i=0;i<modelList.size();i++){
					if(resultArray[i].weight > 0){
						graphList[j]->setColor(i);
						resultArray[i].mark();
						updateObjectPosition(i, resultArray[i].nodePair);
						break;
					}
				}
			}
			delete []resultArray;

			// show the appearence rate
			//cout<<"The appearance rate are:"<<endl;
			//for(int i=0;i<Statistic[0].size();i++){
			//	cout<<Statistic[0].find(i)->second.getAppearanceRate(timmer)<<"   ";
			//	//cout<<Statistic[0].find(i)->second.maxContinueFrames<<"   ";
			//}
			//cout<<endl;
		}
	}
}


void Recognition::updateResultList(int index, NodePairs resultPair, Node *center){
	// The proportion of object and neighbors
	float objProp = 0.01;
	float neiProp = 0.05;

	// The basic weight for the new result
	float objWeight = 1;
	float neiWeight = 0;

	// Try to find the point in same object and the neighbors
	vector<RecognitionResult>::iterator it = this->resultList.begin();
	for(;it!=this->resultList.end();it++){
		// If in same object
		if(it->modelIndex == index){
			objWeight += it->weight * objProp;
		}

		// If is the same point
		if(it->center == center){
			neiWeight = it->weight;
		}

		// If find a neighbor
		if(it->center->hasNeighbor(center)){
			neiWeight += it->weight * neiProp;
		}
		
	}
	
	// If the result list is full
	if(this->resultList.size()>=this->maxListLength){
		this->resultList.erase(this->resultList.begin());
	}

	// Add new Result into List
	RecognitionResult newResult(resultPair, center, objWeight+neiWeight, index);
	this->resultList.push_back(newResult);
}

void Recognition::updateResultList(int index, NodePairs resultPair){
	// The proportion of object and neighbors
	float objProp = 0.01;
	float neiProp = 0.01;

	// The basic weight for the new result
	float objWeight = 1;
	float neiWeight = 0;

	// Try to find the point in same object and the neighbors
	vector<RecognitionResult>::iterator it = this->resultList.begin();
	for(;it!=this->resultList.end();it++){
		// If in same object
		if(it->modelIndex == index){
			objWeight += it->weight * objProp;
		}

		// Compare the neighborhood
		NodePairs::iterator resultPairIt = resultPair.begin();
		for(;resultPairIt != resultPair.end();resultPairIt++){
			NodePairs::iterator oldPairIt = it->nodePair.begin();
			for(;oldPairIt!=it->nodePair.end();oldPairIt++){
				// if identical point is found
				if(resultPairIt->first == oldPairIt->first){
					neiWeight += it->weight * neiProp * 2;
					break;
				}

				// if neighborhood is found
				if(resultPairIt->first->hasNeighbor(oldPairIt->first)){
					neiWeight += it->weight * neiProp;
				}
			}
		}
	}
	
	// If the result list is full
	if(this->resultList.size()>=this->maxListLength){
		this->resultList.erase(this->resultList.begin());
	}

	// Add new Result into List
	RecognitionResult newResult(resultPair, objWeight+neiWeight, index);
	this->resultList.push_back(newResult);
}

void Recognition::updateResultList(int objIndex, int modelIndex, NodePairs resultPair){
	// The proportion of object and neighbors
	float objProp = 0.01;
	float neiProp = 0.01;

	// The basic weight for the new result
	float objWeight = 1;
	float neiWeight = 0;

	// get the old result by index of object and model
	RecognitionResult *oldResult = &(this->resultMap[objIndex][modelIndex]);
	// first result

	//if(oldResult->modelIndex == -1){
	//	oldResult->modelIndex = modelIndex;
	//} else {
	//	if(oldResult->modelIndex == modelIndex){
	//		objWeight += oldResult->weight * objProp;
	//	}
	//}

	// Compare the neighborhood
	NodePairs::iterator resultPairIt = resultPair.begin();
	for(;resultPairIt != resultPair.end();resultPairIt++){
		NodePairs::iterator oldPairIt = oldResult->nodePair.begin();
		for(;oldPairIt!=oldResult->nodePair.end();oldPairIt++){
			// if identical point is found
			if(resultPairIt->first == oldPairIt->first){
				neiWeight += oldResult->weight * neiProp * 2;
				break;
			}

			// if neighborhood is found
			if(resultPairIt->first->hasNeighbor(oldPairIt->first)){
				neiWeight += oldResult->weight * neiProp;
			}
		}
	}

	// Update result in result map
	oldResult->modelIndex = modelIndex;
	oldResult->weight += objWeight+neiWeight;
	oldResult->nodePair = resultPair;
}

void Recognition::updateObjectPosition(int index, NodePairs resultPair){
	Mat R = Mat::eye(3,3,CV_32FC1);
	Mat T = Mat::zeros(3,1,CV_32FC1);
	vector<Point3f> newPoints;
	vector<Point3f> oldPoints;
	NodePairs::iterator it=resultPair.begin();
	for(;it!=resultPair.end();it++){
		// The old points are the points, which were saved in model
		oldPoints.push_back(it->first->getPoint());
		newPoints.push_back(it->second->getPoint());
	}
	ImageProcess::UQFindRAndT(oldPoints, newPoints, R, T);
	modelList[index]->transformate(R,T);
}

int Recognition::findBestResult(){
	// Parameter to find the best result
	float maxWeight = -1;
	int maxIndex = -1;
	for(int i=0;i<this->resultList.size();i++){
		// find the best result
		if(this->resultList[i].weight > maxWeight){
			maxIndex = i;
			maxWeight = this->resultList[i].weight;
		}
	}
	cout<<"The Max Weight: "<<maxWeight<<endl;
	return maxIndex;
}

int Recognition::findBestResult(int objIndex){
	// Parameter to find the best result
	float maxWeight = -1;
	int maxIndex = -1;
	for(int i=0;i<this->resultMap[objIndex].size();i++){
		// find the best result
		if(this->resultMap[objIndex][i].weight > maxWeight){
			maxIndex = i;
			maxWeight = this->resultMap[objIndex][i].weight;
		}
		cout<<"The Weight of Object "<<objIndex<<" for Model "<<this->resultMap[objIndex][i].modelIndex<<" is: "<<maxWeight<<endl;
	}
	
	return maxIndex;
}

