#include "stdafx.h"
#include "Object.hpp"

Object::Object(){
	sumR = Mat::eye(3,3,CV_32FC1);
	sumT = Mat::zeros(3,1,CV_32FC1);

	defaultDataPath = "VTKData/";
}

Object::Object(const char *name){
	sumR = Mat::eye(3,3,CV_32FC1);
	sumT = Mat::zeros(3,1,CV_32FC1);

	defaultDataPath = "VTKData/";
	loadFromVTKFile(name);
}

Object::~Object(){
}

void Object::update(vector<Point3f> points, Mat R, Mat T, float dThreshold, int tThreshold){
	curR = R;
	curT = T;

	updateGraph(points, curR, curT, dThreshold, tThreshold);
	
	////if lesser than 3 fixed node have been found, which means, that no plane can be identified. Than reset the rotationsmatrix
	//if(this->fixNodeCount>=3){
	//	sumR *= R;
	//	sumT += T;
	//}
	//cout<<"The number of fixed nodeeeeeeeeeeeeeeeeeeeeeeee: "<<this->fixNodeCount<<endl;
}



//void Object::getMiddelPoint(Point3f &mid){
//	mid = Point3f(0,0,0);
//	for(int i=0;i<this->fixedNodeList.size();i++){
//		mid += fixedNodeList[i]->getPoint();
//	}
//
//	if(this->fixNodeCount > 0){
//		mid.x = mid.x/this->fixedNodeList.size();
//		mid.y = mid.y/this->fixedNodeList.size();
//		mid.z = mid.z/this->fixedNodeList.size();
//	}
//}

void Object::clearUnfixedNodes(){
	//vector<Node*>::iterator it;
	//for(it=this->nodeList.begin();it!=this->nodeList.end();it++){
	//	if(!(*it)->isFixed){
	//		this->deleteNode(*it);
	//	}
	//}
	for(int i=0;i<this->nodeList.size();i++){
		if(!this->nodeList[i]->isFixed){
			this->deleteNode(i);
			// the index of the current node should be decreased
			i--;
		}
	}
}

void Object::joinSimilarNodes(float e){
	clearUnfixedNodes();
	for(int i=0;i<this->nodeList.size();i++){
		Node *curNode = nodeList[i];
		//map<Node*, float>::iterator edgeit;
		//for(edgeit=curNode->neighbors.begin();edgeit!=curNode->neighbors.end();edgeit++){
		//	// if two node are too close, union them
		//	if(edgeit->second<e){
		//		Node *dNode = edgeit->first;
		//		// reset the posion of the iterator
		//		edgeit--;
		//		// delete the node directly without union
		//		deleteNode(dNode);
		//	}
		//}
		for(int j=i+1;j<this->nodeList.size();j++){
			float dist = curNode->distanceTo(nodeList[j]);
			if(dist < e){
				unionNode(curNode, nodeList[j]);
				deleteNode(nodeList[j]);
				j--;
			}
		}
	}
}

void Object::transformate(Mat R, Mat T){
	for(int i=0;i<this->nodeList.size();i++){
		Mat oldPoint = Mat(this->nodeList[i]->getPoint());
		//cout<<"The oldPoint: "<<oldPoint<<endl;
		Mat newPoint = R*oldPoint + T;
		//cout<<"The newPoint: "<<newPoint<<endl;
		this->nodeList[i]->setPoint(Point3f(newPoint.at<float>(0,0),
											newPoint.at<float>(1,0),
											newPoint.at<float>(2,0)));
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
	//set<Node*> fixedNodes;
	//for(int i=0;i<this->nodeList.size();i++){
	//	if(this->nodeList[i]->isFixed){
	//		fixedNodes.insert(this->nodeList[i]);
	//	}
	//}
	// delete all unfixed nodes and join the similar nodes together
	this->joinSimilarNodes(0.045);
	int pointSize = this->nodeList.size();
	fout<<"POINTS "<<pointSize<<" float"<<endl;

	// write the points

	// the parameters of edges
	int edgeSize = 0;
	stringstream edges;
	// loop for all nodes
	//for(it=fixedNodes.begin();it!=fixedNodes.end();it++){
		//Node *curNode = *it;
		//// calculate the index of current node
		//int beginIndex = distance(fixedNodes.begin(), it);

	for(int i=0;i<pointSize;i++){
		Node *curNode = this->nodeList[i];
		int beginIndex = i;
		fout<<curNode->x<<" ";
		fout<<curNode->y<<" ";
		fout<<curNode->z<<endl;

		// write the edges
		map<Node*, float>::iterator mapit;
		// loop for all neighbors of current node
		for(mapit=curNode->neighbors.begin();mapit!=curNode->neighbors.end();mapit++){
			// try to find the current neighbor from the fixed node list
			//set<Node*>::iterator endit = fixedNodes.find(mapit->first);
			//// if found, write the edge into the stream
			//if(endit!=fixedNodes.end()){
			//	int endIndex = distance(fixedNodes.begin(),endit);
			int endIndex = findIndex(mapit->first);
			if(endIndex != -1){
				edges<<"2 ";
				edges<<beginIndex;
				edges<<" ";
				edges<<endIndex;
				edges<<"\n";
				edgeSize++;
			}
		}
	}
	fout<<endl;

	// write the number of edges
	fout<<"LINES "<<edgeSize<<" "<<edgeSize*3<<endl;
	fout<<edges.rdbuf();

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

	//this->nodeList.clear();
	vector<Point3f> nodes;
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
		
		//Node *temp = new Node(Point3f(x,y,z));
		//this->nodeList.push_back(temp);
		Point3f temp(x,y,z);
		nodes.push_back(temp);
	}
	//this->createCompleteGraph(nodes);
	this->addNodes(nodes);

	//space 
	fin.getline(temp, 256);

	char edgeHead[256];
	fin.getline(edgeHead, 256);
	int edgeSize;
	string edgeHeadStr(edgeHead);
	edgeHeadStr.erase(edgeHeadStr.begin(), edgeHeadStr.begin()+6);
	stringstream edgeHeadSS;
	edgeHeadSS<<edgeHeadStr;
	edgeHeadSS>>edgeSize;

	for(int i=0;i<edgeSize;i++){
		char cEdge[256];
		fin.getline(cEdge, 256);
		string edge(cEdge);

		// remove the first 2 charactors
		edge.erase(edge.begin(), edge.begin()+2);


		// read the index of the beginning node
		int firstSpace = edge.find(' ');
		stringstream beginSS(edge.substr(0,firstSpace));
		int beginIndex;
		beginSS>>beginIndex;
		edge.erase(edge.begin(), edge.begin()+firstSpace+1);

		// read the index of the end node
		stringstream endSS(edge);
		int endIndex;
		endSS>>endIndex;

		Node *beginNode = this->nodeList[beginIndex];
		Node *endNode = this->nodeList[endIndex];

		this->addEdge(beginNode, endNode);
	}
}




