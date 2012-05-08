#include "stdafx.h"
#include "Recognition.hpp"

Recognition::Recognition(){
	//Object *obj1 = new Object("Box1");
	//this->objectList.push_back(obj1);

	//Object *obj2 = new Object("Box2");
	//this->objectList.push_back(obj2);

	//Object *obj3 = new Object("Box3");
	//this->objectList.push_back(obj3);

	Object *obj1 = new Object("Box4");
	this->objectList.push_back(obj1);

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
	//bool result = newObj->isEqual(this->objectList[0]);
	//cout<<"The compare result is: "<<result<<endl;

	bool compResult = false;
	if(caliResult.size()>0 && caliResult[0].size()>0){
		Graph *newGraph = new Graph();

		vector<Point3f> point3D;
		vector<Point2f> point2D;
		decPMDPointVector(caliResult[0], point3D, point2D);

		newGraph->createCompleteGraph(point3D);
		for(int i=0;i<this->objectList.size();i++){
			compResult = newGraph->isEqual(objectList[i]);
			if(compResult){
				cout<<"Find the Object! "<<i<<endl;
				// show the result
				Scalar color;
				switch(i){
					case 0: color = Scalar(0,0,255,0);
							break;
					case 1: color = Scalar(0,255,255,0);
							break;
					case 2: color = Scalar(255,255,0,0);
							break;
				}
				circle(drawMat, Point2f(i*10+5, 5), 5, color, -1);
				//break;
			}
		}
		//compResult = newGraph->isEqual(objectList[0]);
		//if(compResult){
		//	cout<<"Find the Object!"<<endl;
		//}
	}
	imshow("Recognition", drawMat);
}



