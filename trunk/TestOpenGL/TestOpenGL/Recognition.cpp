#include "stdafx.h"
#include "Recognition.hpp"

Recognition::Recognition(){
	Object *obj1 = new Object("Box1");
	this->objectList.push_back(obj1);

	Object *obj2 = new Object("Box2");
	this->objectList.push_back(obj2);

	Object *obj3 = new Object("Box3");
	this->objectList.push_back(obj3);

	graph = new Graph();

}

Recognition::~Recognition(){
}

/***************************************************************
 *
 * At first only one object will be focused
 *
 **************************************************************/
void Recognition::objectRecognition(std::vector<PMDPoint> inputPoints){
	vector<vector<PMDPoint>> caliResult;
	float CALIEPS3D = 0.17;
	int minPts = 2;

	DBSCANPMDPoint(caliResult, inputPoints, CALIEPS3D, minPts);

		
	namedWindow("Recognition", CV_WINDOW_AUTOSIZE);

	drawMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC1);
	
	for(int i=0;i<caliResult.size();i++){
		for(int j=0;j<caliResult[i].size();j++){
			circle(drawMat, caliResult[i][j].index, 2, Scalar(0,0,255,0), -1);
			for(int k=j+1;k<caliResult[i].size();k++){
				line(drawMat, caliResult[i][j].index, caliResult[i][k].index, Scalar(0,255,255,0),1);
			}
		}
	}

	imshow("Recognition", drawMat);
}



