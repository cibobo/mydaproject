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
 * Definition of Recoginition
 *****************************/
Recognition::Recognition(){
	Object *obj1 = new Object("Box1");
	this->objectList.push_back(obj1);

	Object *obj2 = new Object("Box2");
	this->objectList.push_back(obj2);

	Object *obj3 = new Object("Box3");
	this->objectList.push_back(obj3);

	//Object *obj1 = new Object("Box4");
	//this->objectList.push_back(obj1);

	Object *obj4 = new Object("Box5");
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
		Graph *newGraph = new Graph();

		vector<Point3f> point3D;
		vector<Point2f> point2D;
		decPMDPointVector(caliResult[0], point3D, point2D);

		newGraph->createCompleteGraph(point3D);
		// create the new map for the static date
		createStatisticMap();
		for(int i=0;i<this->objectList.size();i++){
			compResult = newGraph->isEqual(objectList[i]);
			if(compResult){
				cout<<"Find the Object! "<<i<<endl;
				int appear = this->Statistic[0].find(i)->second.appear(timmer);
				//cout<<"The appearence rate is: "<<float(appear)/timmer<<endl;

				// show the result
				Scalar color = scalarColorList[i];
				circle(drawMat, Point2f(i*10+5, 5), 5, color, -1);
				//break;
			}
		}

		// show the appearence rate
		cout<<"The appearance rate are:"<<endl;
		for(int i=0;i<Statistic[0].size();i++){
			cout<<Statistic[0].find(i)->second.getAppearanceRate(timmer)<<"   ";
			//cout<<Statistic[0].find(i)->second.maxContinueFrames<<"   ";
		}
		cout<<endl;
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


