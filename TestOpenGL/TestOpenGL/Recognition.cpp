#include "stdafx.h"
#include "Recognition.hpp"


/****************************
 * Definition of Static Date
 ****************************/
StatisticDate::StatisticDate(){
	appearances = 0;
	lifetime = 0;
	continueFrames = 0;
	maxContinueFrames = 0;
	timmerDiff = 0;
}

//StatisticDate::StatisticDate(int index){
//	objectIndex = index;
//	appearances = 0;
//	lifetime = 0;
//}

int StatisticDate::appear(int timmer){
	appearances++;
	lifetime++;
	
	// get the max number of continually frames
	if(continueFrames > maxContinueFrames){
		maxContinueFrames = continueFrames;
	}
	// if the first time or the result doen't appear continually
	if(timmer-appearances > timmerDiff){
		timmerDiff = timmer-appearances;

		continueFrames = 0;
	} else {
		continueFrames++;
		//maxContinueFrames++;
	}

	return appearances;
}

float StatisticDate::getAppearanceRate(int timmer){
	// das Gewicht von dem vorkommende Bilder ist gleich wie das Gewicht von dem Maximale Anzahl der kontinueliche bildern
	float frameCount = appearances * 0.5 + maxContinueFrames*10 * 0.5;
	return (frameCount/timmer)>1 ? 1 : frameCount/timmer;
}
/*****************************
 * Definition of RecognitionResult
 ****************************/
RecognitionResult::RecognitionResult(){
	weight = 0;
}

RecognitionResult::RecognitionResult(RecognitionResult::NodePairs nodePair, float weight, int objIndex){
	this->nodePair = nodePair;
	this->weight = weight;
	this->objIndex = objIndex;
}

RecognitionResult::RecognitionResult(RecognitionResult::NodePairs nodePair, Node *center, float weight, int objIndex){
	this->nodePair = nodePair;
	this->center = center;
	this->weight = weight;
	this->objIndex = objIndex;
}

void RecognitionResult::mark(){
	NodePairs::iterator it = this->nodePair.begin();
	for(;it!=this->nodePair.end();it++){
		it->first->color = 1;
	}
	this->center->color = 2;
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
	maxListLength = 5;

	//Object *obj1 = new Object("Box1");
	//this->objectList.push_back(obj1);

	//Object *obj2 = new Object("Box2");
	//this->objectList.push_back(obj2);

	//Object *obj3 = new Object("Box3");
	//this->objectList.push_back(obj3);

	//Object *obj4 = new Object("Box4");
	//this->objectList.push_back(obj4);

	Object *obj4 = new Object("Box_all4");
	this->objectList.push_back(obj4);



	graph = new Graph();
	
	timmer = 0;
}

Recognition::~Recognition(){
}

/***************************************************************
 *
 * At first only one object will be focused
 *
 **************************************************************/
void Recognition::objectRecognition(std::vector<PMDPoint> inputPoints){
	timmer++;

	vector<vector<PMDPoint>> caliResult;
	float CALIEPS3D = 0.17;
	int minPts = 1;

	Point3f center;
	for(int i=0;i<inputPoints.size();i++){
		center += inputPoints[i].coord;
	}
	center.x = center.x/inputPoints.size();
	center.y = center.y/inputPoints.size();
	center.z = center.z/inputPoints.size();

	DBSCANPMDPoint(caliResult, inputPoints, center, CALIEPS3D, minPts);

		
	namedWindow("Recognition", CV_WINDOW_AUTOSIZE);

	drawMat = Mat::zeros(H_BILDSIZE, V_BILDSIZE, CV_8UC3);
	
	for(int i=0;i<caliResult.size();i++){
		for(int j=0;j<caliResult[i].size();j++){
			circle(drawMat, caliResult[i][j].index, 2, Scalar(0,0,255,0), -1);
			for(int k=j+1;k<caliResult[i].size();k++){
				line(drawMat, caliResult[i][j].index, caliResult[i][k].index, Scalar(0,255,255,0),1);
			}
		}
	}

	

	//Object *newObj = new Object("Box2");
	//bool result = newObj->isEqual(this->objectList[3]);
	//cout<<"The compare result is: "<<result<<endl;
	//cout<<"The edge lenth of Box 2 and 5 are:"<<endl;
	//for(int i=0;i<this->objectList[1]->nodeList.size();i++){
	//	for(int j=i+1;j<this->objectList[1]->nodeList.size();j++){
	//		cout<<this->objectList[1]->nodeList[i]->distanceTo(this->objectList[1]->nodeList[j])<<"    ";
	//		cout<<this->objectList[3]->nodeList[i]->distanceTo(this->objectList[3]->nodeList[j])<<endl;
	//	}
	//	cout<<endl;
	//}

	bool compResult = false;
	if(caliResult.size()>0 && caliResult[0].size()>0){
		//graph = new Graph();

		vector<Point3f> point3D;
		vector<Point2f> point2D;
		decPMDPointVector(caliResult[0], point3D, point2D);

		graph->createCompleteGraph(point3D);

		// create new thread to show the graph
		//if(_beginthread (drawGraphWithOpenGL, 0, NULL) == -1){
		//	cout<<"Failed to create draw thread"<<endl;
		//}

		// create the new map for the static date
		createStatisticMap();
		for(int i=0;i<this->objectList.size();i++){
			float e = 0.013;
			float rate = 0.0;
			float error = 0;
			NodePairs resultPair;
			Node* center;
			//compResult = graph->isEqualAdvance(objectList[i], e, rate, resultPair, center, error);
			compResult = graph->isEqualAdvance(objectList[i], 0.05, 0.8, 4, resultPair);
			if(compResult){
				cout<<"Find the Object! "<<i<<endl;
				//int appear = this->Statistic[0].find(i)->second.appear(timmer);
				//cout<<"The appearence rate is: "<<float(appear)/timmer<<endl;

				// show the result
				Scalar color = scalarColorList[i];
				circle(drawMat, Point2f(i*10+5, 5), 5, color, -1);
				
				// Update result list
				updateResultList(i, resultPair);
				//updateObjectPosition(i, resultPair);
				
			}
			int maxIndex = this->findBestResult();
			cout<<"The max Index: "<<maxIndex<<endl;
			if(maxIndex != -1){
				this->resultList[maxIndex].markColorful();
				// transformate the object
				updateObjectPosition(i, resultPair);
			}
		}

		// show the appearence rate
		//cout<<"The appearance rate are:"<<endl;
		//for(int i=0;i<Statistic[0].size();i++){
		//	cout<<Statistic[0].find(i)->second.getAppearanceRate(timmer)<<"   ";
		//	//cout<<Statistic[0].find(i)->second.maxContinueFrames<<"   ";
		//}
		//cout<<endl;
	}
	imshow("Recognition", drawMat);
}

void Recognition::createStatisticMap(){
	map<int, StatisticDate> statisticDate;
	for(int i=0;i<this->objectList.size();i++){
		statisticDate.insert(pair<int, StatisticDate>(i, StatisticDate()));
	}
	this->Statistic.push_back(statisticDate);
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
		if(it->objIndex == index){
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
		if(it->objIndex == index){
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
	UQFindRAndT(oldPoints, newPoints, R, T);
	objectList[index]->transformate(R,T);
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


//void Recognition::drawGraphWithOpenGL(){
//	static OpenGLWinUI *pObjGLWinUI = new OpenGLWinUI;
//	OpenGLContext *pObjViewContext = new OpenGLContext;
//
//	TlsSetValue(1001, pObjGLWinUI);
//
//	if(!CreateOpenGLWindow("Recognition Graph Window", 0, 0, 750, 750, PFD_TYPE_RGBA, 0, pObjGLWinUI, pObjViewContext)){
//		exit(0);
//	}
//
//	cout<<"The Object Structur OpenGL Windows is running"<<endl;
//
//	MSG msg;
//	bool bDone = false;
//
//	while(!bDone){
//		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
//			if(msg.message==WM_QUIT){
//				bDone = TRUE;
//			} else if(msg.message==WM_RBUTTONDOWN){
//				// klick right button of mouse to stop and rerun the input of frame
//				//if(isPause){
//				//	cout<<"frame running!"<<endl;
//				//	LeaveCriticalSection(&pauseCrs);
//				//	isPause = false;
//				//} else {
//				//	cout<<"frame pause!"<<endl;
//				//	EnterCriticalSection(&pauseCrs);
//				//	isPause = true;
//				//}
//			} else{
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//				//display(pOpenGLWinUI, disData, intData, ampData);
//			}
//		} else {
//			//EnterCriticalSection (&calcCrs);
//			display(pObjGLWinUI, graph);
//			SwapBuffers(pObjViewContext->hDC);
//			glEnable(GL_LIGHTING);
//			//LeaveCriticalSection(&calcCrs);
//		}
//	}
//	delete pObjViewContext;
//	delete pObjGLWinUI;
//}