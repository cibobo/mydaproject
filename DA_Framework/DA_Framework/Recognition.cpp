#include "stdafx.h"
#include "Recognition.hpp"

/*****************************
 * Definition of RecognitionResult
 ****************************/
RecognitionResult::RecognitionResult(){
	weight = 0;
	modelIndex = -1;
	lifeTime = 0;
	lastSumP = 3;
	isFind = true;
}

RecognitionResult::RecognitionResult(RecognitionResult::NodePairs nodePair, float weight, int modelIndex){
	this->nodePair = nodePair;
	this->weight = weight;
	this->modelIndex = modelIndex;
	lifeTime = 0;
}

RecognitionResult::RecognitionResult(RecognitionResult::NodePairs nodePair, Node *center, float weight, int modelIndex){
	this->nodePair = nodePair;
	this->center = center;
	this->weight = weight;
	this->modelIndex = modelIndex;
	lifeTime = 0;
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

	isInCurrentFound = false;
	evaIsoRecogCount = 0;
	evaNodeCount = 0;

	//set the first recognized statistic value to 0
	this->statisticResult.push_back(0);
	this->statisticResult.push_back(0);
}

Recognition::~Recognition(){
	for(int i=0;i<this->modelList.size();i++){
		delete modelList[i];
	}
}


void Recognition::loadModels(){
	Object *obj1 = new Object("Box1");
	obj1->setColor(0);
	this->modelList.push_back(obj1);



	Object *obj2 = new Object("Box2");
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

void Recognition::loadModels(string names){
	int findPos;
	vector<string> namesVec;
	//find the names, which are separated with ','
	while((findPos=names.find(','))!=string::npos){
		namesVec.push_back(names.substr(0, findPos));
		names.erase(names.begin(), names.begin()+findPos+1);
	}
	if(names.size()>0){
		namesVec.push_back(names);
	}
	//Load models with these names
	this->loadModels(namesVec);
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

	updateMultiResultList();

	bool compResult = false;
	//if(caliResult.size()>0 && caliResult[0].size()>0){
	// loop for all possible objekt
	this->graphList.clear();

	// If there is just one Objet
	//if(segResult.size() == 1){
	//	if(this->segResult[0].size()>0){
	//		vector<Point3f> point3D;
	//		vector<Point2f> point2D;
	//		ImageProcess::decPMDPointVector(this->segResult[0], point3D, point2D);

	//		Graph *graph = new Graph();
	//		graph->createCompleteGraph(point3D);
	//		this->graphList.push_back(graph);

	//		// create the new map for the static date
	//		NodePairs resultPair;
	//		for(int i=0;i<this->modelList.size();i++){
	//			// reset the color
	//			modelList[i]->setColor(i);

	//			compResult = graph->isEqual(modelList[i], resultPair, 
	//										this->distanceProportion, 
	//										this->nodesCountProportion,
	//										this->distanceThreshold);
	//			if(compResult){
	//				cout<<"Find the Object! "<<i<<endl;
	//				updateResultList(i, resultPair);
	//			}
	//		}
	//	}
	//	int maxIndex = this->findBestResult();
	//	cout<<"The max Index: "<<maxIndex<<endl;
	//	if(maxIndex != -1){
	//		graphList[0]->setColor(resultList[maxIndex].modelIndex);
	//		this->resultList[maxIndex].mark();
	//		// transformate the object
	//		updateObjectPosition(resultList[maxIndex].modelIndex, resultList[maxIndex].nodePair);
	//	}
	//} else {


		for(int j=0;j<this->multiResultList.size();j++){
			isInCurrentFound = false;
			multiResultList[j]->weight = 0;
			if(this->multiResultList[j]->nodes.size() > 0 && this->multiResultList[j]->isFind){
				vector<Point3f> point3D;
				vector<Point2f> point2D;
				ImageProcess::decPMDPointVector(this->multiResultList[j]->nodes, point3D, point2D);

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
						//Update result list. Using size of the ResultPair as the weight, if there are more than one Objects
						if(resultPair.size() > multiResultList[j]->weight){
							multiResultList[j]->modelIndex = i;
							multiResultList[j]->nodePair = resultPair;
							multiResultList[j]->weight = resultPair.size();
							//Evaluation's Help Parameters
							isInCurrentFound = true;
							evaIsoRecogCount++;
							evaNodeCount+=multiResultList[j]->weight;
						}
					} 
						
				}
				if(multiResultList[j]->modelIndex != -1){
					multiResultList[j]->mark();
					updateObjectPosition(multiResultList[j]->modelIndex, multiResultList[j]->nodePair);

					graph->setColor(multiResultList[j]->modelIndex);	
					//if(isInCurrentFound){
						this->statisticResult[multiResultList[j]->modelIndex]++;
					//}
				}
						
			}
		}

	//}

	//for(int j=0;j<this->segResult.size();j++){
	//	isInCurrentFound = false;
	//	if(this->segResult[j].size()>0){
	//		RecognitionResult *resultArray = new RecognitionResult[modelList.size()];
	//		
	//		vector<Point3f> point3D;
	//		vector<Point2f> point2D;
	//		ImageProcess::decPMDPointVector(this->segResult[j], point3D, point2D);

	//		Graph *graph = new Graph();
	//		graph->createCompleteGraph(point3D);
	//		this->graphList.push_back(graph);

	//		// create the new map for the static date
	//		NodePairs resultPair;
	//		for(int i=0;i<this->modelList.size();i++){
	//			// reset the color
	//			modelList[i]->setColor(i);

	//			compResult = graph->isEqual(modelList[i], resultPair, 
	//										this->distanceProportion, 
	//										this->nodesCountProportion,
	//										this->distanceThreshold);
	//			if(compResult){
	//				cout<<"Find the Object! "<<i<<endl;
	//				isInCurrentFound = true;
	//				//int appear = this->Statistic[0].find(i)->second.appear(timmer);
	//				//cout<<"The appearence rate is: "<<float(appear)/timmer<<endl;

	//				// show the result

	//				
	//				
	//				//if(this->segResult.size() == 1){
	//				//	// for just one Object
	//				//	updateResultList(i, resultPair);
	//				////updateObjectPosition(i, resultPair);
	//				//} else {
	//					// more than 2 objects
	//					resultArray[i].nodePair = resultPair;
	//					resultArray[i].weight++;

	//					evaNodeCount+=resultPair.size();
	//				//}
	//				
	//			}

	//		}


	//		//int maxIndex = this->findBestResult(j);
	//		//cout<<"The max Index: "<<maxIndex<<endl;
	//		//if(maxIndex != -1){
	//		//	this->graphList[j]->setColor(maxIndex);

	//		//	//this->resultMap[j][maxIndex].markColorful();
	//		//	this->resultMap[j][maxIndex].mark();
	//		//	// transformate the object
	//		//	updateObjectPosition(resultMap[j][maxIndex].modelIndex, resultMap[j][maxIndex].nodePair);
	//		//	// set the color of the graph
	//		//	
	//		//}


	//		//if(this->segResult.size() == 1){
	//		//	// for just one object
	//		//	int maxIndex = this->findBestResult();
	//		//	cout<<"The max Index: "<<maxIndex<<endl;
	//		//	if(maxIndex != -1){
	//		//		graphList[j]->setColor(resultList[maxIndex].modelIndex);
	//		//		this->resultList[maxIndex].mark();
	//		//		// transformate the object
	//		//		updateObjectPosition(resultList[maxIndex].modelIndex, resultList[maxIndex].nodePair);
	//		//		this->statisticResult[0]++;
	//		//	}
	//		//} else {
	//			// for more objects
	//			bool isfind = false;
	//			for(int i=0;i<modelList.size();i++){
	//				if(resultArray[i].weight > 0){
	//					graphList[j]->setColor(i);
	//					resultArray[i].mark();
	//					updateObjectPosition(i, resultArray[i].nodePair);
	//					this->statisticResult[i]++;
	//					break;
	//				}
	//			}
	//		//}
	//		delete []resultArray;

	//		// show the appearence rate
	//		//cout<<"The appearance rate are:"<<endl;
	//		//for(int i=0;i<Statistic[0].size();i++){
	//		//	cout<<Statistic[0].find(i)->second.getAppearanceRate(timmer)<<"   ";
	//		//	//cout<<Statistic[0].find(i)->second.maxContinueFrames<<"   ";
	//		//}
	//		//cout<<endl;
	//	}
	//}
}


void Recognition::updateMultiResultList(){
	float sigma = 0.02;
	float assRate = 0.76;
	//Copy of the Segmentation
	vector<vector<PMDPoint>> tempSegResult = this->segResult;

	namedWindow("OpenCVCorrespondenz", CV_WINDOW_AUTOSIZE);
	Mat testImg = Mat(V_BILDSIZE, H_BILDSIZE*2, CV_8UC3);
	Mat left = Mat(testImg, Range::all(), Range(0,H_BILDSIZE));
	Mat right = Mat(testImg, Range::all(), Range(H_BILDSIZE,H_BILDSIZE*2));

	//Set all result as not found
	for(int j=0;j<multiResultList.size();j++){
		multiResultList[j]->isFind = false;
	}

	//Loop for all segmentations
	for(int i=0;i<tempSegResult.size();i++){
		bool isUsed = false;
		//Loop for all existing Result
		for(int j=0;j<this->multiResultList.size();j++){
			vector<PMDPoint> tempNew, tempOld;
			float avrDis, disPE, sumP;
			//Find the Association between the segmentation and the nodes saved in Result
			ImageProcess::featureAssociatePMD(multiResultList[j]->nodes, tempSegResult[i], sigma, assRate,
				tempOld, tempNew, avrDis, disPE, sumP);

			//If a besser Association has been found
			if(sumP>multiResultList[j]->lastSumP){

				//Test OpenCV Draw
				Scalar color = scalarColorList[j];
				for(int k=0;k<multiResultList[j]->nodes.size();k++){
					circle(left, multiResultList[j]->nodes[k].index, 2, color, -1);
				}
				for(int k=0;k<tempSegResult[i].size();k++){
					circle(right, tempSegResult[i][k].index, 2, color, -1);
				}
				for(int k=0;k<tempOld.size();k++){
					Point2f trans(204,0);
					line(testImg, tempOld[k].index, tempNew[k].index+trans, color);
				}
				//End Test

				//Update the Result
				multiResultList[j]->isFind = true;
				multiResultList[j]->lastSumP = sumP;
				multiResultList[j]->nodes = tempSegResult[i];
				multiResultList[j]->lifeTime += 2;
				isUsed = true;
			}
		}
		//Delete the associated segmentation
		if(isUsed){
			tempSegResult.erase(tempSegResult.begin()+i);
			i--;
		}
	}

	imshow("OpenCVCorrespondenz", testImg);
	char c = waitKey(10);
	
	//Update the life time of the Results, and delete the unused Result, which may match to the objet, that will be no more appeared.
	for(int j=0;j<multiResultList.size();j++){
		//The reduce of the life time is acorroding to the size of the MultiResultList
		multiResultList[j]->lifeTime -= (multiResultList.size()>2?multiResultList.size():2);
		if(multiResultList[j]->lifeTime < 0){
			multiResultList.erase(multiResultList.begin()+j);
			j--;
		} else {
			//Reset the Threshold of the Association
			multiResultList[j]->lastSumP = 3;
		}
	}
	
	//Add the rest Segementations als the new Result into the ResultList
	for(int i=0;i<tempSegResult.size();i++){
		RecognitionResult *newResult = new RecognitionResult();
		newResult->nodes = tempSegResult[i];
		this->multiResultList.push_back(newResult);
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

