#include "stdafx.h"
#include "Object.hpp"

Object::Object(){
	sumR = Mat::eye(3,3,CV_32FC1);
	sumT = Mat::zeros(3,1,CV_32FC1);

	tFilter = KalmanFilter(6,3,0);
	qFilter = KalmanFilter(3,3,0);

	initKalmanFilter();

	defaultDataPath = "VTKData/";
}

Object::Object(const char *name){
	sumR = Mat::eye(3,3,CV_32FC1);
	sumT = Mat::zeros(3,1,CV_32FC1);

	tFilter = KalmanFilter(6,3,0);
	qFilter = KalmanFilter(3,3,0);

	initKalmanFilter();

	defaultDataPath = "VTKData/";
	loadFromVTKFile(name);
}

Object::~Object(){
}

void Object::update(vector<Point3f> points, Mat R, Mat T){
	curR = R;
	curT = T;

	//updateKalmanFilter();

	updateGraph(points, curR, curT);
	
	//if lesser than 3 fixed node have been found, which means, that no plane can be identified. Than reset the rotationsmatrix
	if(this->fixNodeCount>=3){
		sumR *= R;
		sumT += T;
	}
	//cout<<"The number of fixed nodeeeeeeeeeeeeeeeeeeeeeeee: "<<this->fixNodeCount<<endl;
}

void Object::initKalmanFilter(){
	tFilter.statePre = Mat::zeros(6,1,CV_32FC1);

	//setIdentity(kFilter.transitionMatrix);
	tFilter.transitionMatrix = *(Mat_<float>(6,6) <<1,0,0,1,0,0,  0,1,0,0,1,0,  0,0,1,0,0,1,  0,0,0,1,0,0,  0,0,0,0,1,0,  0,0,0,0,0,1);
	setIdentity(tFilter.measurementMatrix);

	setIdentity(tFilter.processNoiseCov, Scalar::all(0.1));
	setIdentity(tFilter.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(tFilter.errorCovPost, Scalar::all(0.1));

	//init the quaternion kalmanfilter
	qFilter.statePre = Mat::zeros(3,1,CV_32FC1);
	setIdentity(qFilter.transitionMatrix);
	setIdentity(qFilter.measurementMatrix);

	setIdentity(qFilter.processNoiseCov, Scalar::all(0.1));
	setIdentity(qFilter.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(qFilter.errorCovPost, Scalar::all(0.1));
}

void Object::updateKalmanFilter(){
	for(int i=0;i<10;i++){
		Mat prediction = tFilter.predict();
		curT = tFilter.correct(curT);
		//cout<<"loop: "<<i<<"  "<<estimate<<endl;
	}

	for(int i=0;i<3;i++){
		Mat prediction = qFilter.predict();
		curR = qFilter.correct(curR);
		//cout<<"loop: "<<i<<"  "<<estimate<<endl;
	}
}

void Object::getMiddelPoint(Point3f &mid){
	mid = Point3f(0,0,0);
	for(int i=0;i<this->fixedNodeList.size();i++){
		mid += fixedNodeList[i]->getPoint();
	}

	if(this->fixNodeCount > 0){
		mid.x = mid.x/this->fixedNodeList.size();
		mid.y = mid.y/this->fixedNodeList.size();
		mid.z = mid.z/this->fixedNodeList.size();
	}
}

void Object::saveToVTKFile(const char *name){
	string savePath = string(defaultDataPath);
	savePath.append(name);
	savePath.append(".vtk");
	ofstream fout(savePath.data(), ios::out);

	// write the head of the vtk
	fout<<"# vtk DataFile Version 1.0"<<endl;
	fout<<"VTK Data for Object "<<name<<endl;
	fout<<"ASCII"<<endl<<endl;

	// write the points data head
	fout<<"DATASET POLYDATA"<<endl;
	vector<Node*> fixedNodes;
	for(int i=0;i<this->nodeList.size();i++){
		if(this->nodeList[i]->isFixed){
			fixedNodes.push_back(this->nodeList[i]);
		}
	}
	int pointSize = fixedNodes.size();
	fout<<"POINTS "<<pointSize<<" float"<<endl;

	// write the points
	for(int i=0;i<pointSize;i++){
		fout<<fixedNodes[i]->x<<" ";
		fout<<fixedNodes[i]->y<<" ";
		fout<<fixedNodes[i]->z<<endl;
	}
	fout<<endl;

	// write the edges
	int edgeSize = pointSize*(pointSize-1)/2;
	fout<<"LINES "<<edgeSize<<" "<<edgeSize*3<<endl;
	for(int i=0;i<pointSize;i++){
		for(int j=i+1;j<pointSize;j++){
			fout<<2<<" "<<i<<" "<<j<<endl;
		}
	}

	fout.close();
}

void Object::loadFromVTKFile(const char *name){
	string savePath = string(defaultDataPath);
	savePath.append(name);
	savePath.append(".vtk");
	ifstream fin(savePath.data());

	char temp[2048];
	//# vtk DataFile Version 1.0
	fin.getline(temp, 256);
	//Title
	fin.getline(temp, 256);
	//ACII
	fin.getline(temp, 256);
	//space 
	fin.getline(temp, 256);
	//DATASET POLYDATA
	fin.getline(temp, 256);

	char pointHead[256];
	fin.getline(pointHead, 256);
	string pointSizeS;
	for(int i=7;pointHead[i]!=' ';i++){
		pointSizeS.append(1,pointHead[i]);
	}
	stringstream ss(pointSizeS);
	int pointSize;
	ss>>pointSize;

	this->nodeList.clear();
	for(int i=0;i<pointSize;i++){
		char cPoint[256];
		fin.getline(cPoint, 256);
		string point(cPoint);

		int firstSpace = point.find(' ');
		stringstream xSS(point.substr(0, firstSpace));
		float x;
		xSS>>x;
		point.erase(point.begin(), point.begin()+firstSpace+1);

		int secondSpace = point.find(' ');
		stringstream ySS(point.substr(0, secondSpace));
		float y;
		ySS>>y;
		point.erase(point.begin(), point.begin()+secondSpace+1);

		//int end = point.find('\n');
		stringstream zSS(point);
		float z;
		zSS>>z;
		
		Node *temp = new Node(Point3f(x,y,z));
		this->nodeList.push_back(temp);
	}
}
