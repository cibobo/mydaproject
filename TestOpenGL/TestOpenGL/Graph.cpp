#include "stdafx.h"
#include "Graph.hpp"


/*
 * Defination for Edge
 */
Edge::Edge(Node *firstNode, Node *secondNode){
	this->orgNode = firstNode;
	this->dstNode = secondNode;
}

Edge::Edge(Node *firstNode, Node *secondNode, float cost){
	this->orgNode = firstNode;
	this->dstNode = secondNode;
	this->cost = cost;
}

/*
 * Defination for Node
 */
Node::Node(){
	this->timmer = 3;
	this->isFixed = false;
}

Node::Node(int timmer){
	this->timmer = timmer;
	this->isFixed = false;
}

Node::Node(Point3f point){
	this->x = point.x;
	this->y = point.y;
	this->z = point.z;
	this->timmer = 3;
	this->isFixed = false;
}

Node::~Node(){
	//this->adjNodeList.clear();
}

void Node::setPosition(cv::Point3f pos){
	this->x = pos.x;
	this->y = pos.y;
	this->z = pos.z;
}

Point3f Node::getPoint(){
	Point3f point = Point3f(this->x, this->y, this->z);
	return point;
}


/*
 * Defination for Graph
 */
Graph::Graph(){
	this->indexCount = 0;
	this->lifeTime = 0;

	// test
	//this->nodeList.push_back(new Node(Point3f(30, 0, 2)));
	//this->nodeList.push_back(new Node(Point3f(-15, -22, 2)));
	//this->nodeList.push_back(new Node(Point3f(-15, 22, 2)));
}

Graph::Graph(vector<Point3f> points){
	for(int i=0;i<points.size();i++){
		this->addNode(points[i]);
	}
	this->lifeTime = 0;
}

Graph::~Graph(){
	for(int i=0;i<this->nodeList.size();i++){
		delete this->nodeList[i];
	}
	this->nodeList.clear();
}

//void Graph::addNode(Point3f &node){
//	Node *newNode = (Node*)(new Point3f(node));
//	this->indexCount++;
//	newNode->index = this->indexCount;
//	this->nodeList.push_back(newNode);
//}

void Graph::addNode(Point3f &node){
	Node *newNode = new Node(node);
	this->indexCount++;
	newNode->index = this->indexCount;
	this->nodeList.push_back(newNode);
}

void Graph::addNodes(vector<Point3f> points){
	for(int i=0;i<points.size();i++){
		this->addNode(points[i]);
	}
}

void Graph::addEdge(Node *begin, Node *end){
	begin->edgeList.push_back(Edge(begin, end));
}

void Graph::addUndirectedEdge(Node *first, Node *second){
	this->addEdge(first, second);
	this->addEdge(second, first);
}
	
bool Graph::deleteEdge(Node *begin, Node *end){
	for(int i=0;i<begin->edgeList.size();i++){
		if(begin->edgeList[i].dstNode == end){
			begin->edgeList.erase(begin->edgeList.begin() + i);
			return true;
		}
	}
	// if the edge not found, return false
	return false;
}

bool Graph::deleteUndirectedEdge(Node *first, Node *second){
	return deleteEdge(first, second) && deleteEdge(second, first);
}

bool Graph::deleteNode(Node *node){
	for(int i=0;i<node->edgeList.size();i++){
		// delete the edge, which direct to this node
		if(!deleteEdge(node->edgeList[i].dstNode, node->edgeList[i].orgNode)){
			return false;
		}
	}
	for(int i=0;i<this->nodeList.size();i++){
		if(nodeList[i] == node){
			nodeList.erase(nodeList.begin() + i);
			delete node;
			break;
		}
	}
	return true;
}

bool Graph::deleteNode(int index){
	Node *node = nodeList[index];
	for(int i=0;i<node->edgeList.size();i++){
		// delete the edge, which direct to this node
		if(!deleteEdge(node->edgeList[i].dstNode, node->edgeList[i].orgNode)){
			return false;
		}
	}
	nodeList.erase(nodeList.begin() + index);
	delete node;
	return true;
}


//void Graph::createMaxGraph(vector<vector<Point2f>> pointSets){
//	int maxSize = 0;
//	int maxSizeIndex = 0;
//	for(int i=0;i<pointSets.size();i++){
//		if(pointSets[i].size()>maxSize){
//			maxSize = pointSets[i].size();
//			maxSizeIndex = i;
//		}
//	}
//	if(maxSize > 0){
//		this->addNodes(pointSets[i]);
//	}
//}

void Graph::createCompleteGraph(vector<Point3f> points){
	this->addNodes(points);
	for(int i=0;i<this->nodeList.size();i++){
		for(int j=i+1;j<this->nodeList.size();j++){
		this->addUndirectedEdge(nodeList[i], nodeList[j]);
		}
	}
}


//bool Graph::updateGraph(vector<Point3f> points){
//	// increase the lifetime
//	this->lifeTime ++;
//	// if there is no input points
//	if(points.size()<=0){
//		return false;
//	} else {
//		// if the graph contains no points
//		if(this->nodeList.size()<=0){
//			//this->addNodes(points);
//			this->createCompleteGraph(points);
//			return false;
//		} else {
//			vector<Point3f> oldPoints;
//			for(int i=0;i<this->nodeList.size();i++){
//				if(nodeList[i]->timmer < 0){
//					//nodeList.erase(nodeList.begin()+i);
//					this->deleteNode(nodeList[i]);
//					i--;
//				} else {
//					nodeList[i]->timmer--;
//					oldPoints.push_back(Point3f(*nodeList[i]));
//				}
//			}
//			//SVD
//			vector<int> oldIndex, newIndex;
//			featureAssociate(oldPoints, points, 18, oldIndex, newIndex);
//			
//			// set a new array to mark the new point, which will be add into the node List
//			bool *isNewAdd = new bool[points.size()];
//			for(int i=0;i<points.size();i++){
//				isNewAdd[i] = true;
//			}
//
//			for(int i=0;i<oldIndex.size();i++){
//				// reset the timmer for the coorespondence points
//				this->nodeList[oldIndex[i]]->timmer += 2;
//				this->nodeList[oldIndex[i]]->setPosition(points[newIndex[i]]);
//				// if the point can find a coorespondence with the old point, set the value in the Mark-Array to false
//				isNewAdd[newIndex[i]] = false;
//			}
//			
//			// mark the beginning position for the new added nodes
//			int newNodeBeginIndex = nodeList.size();
//			for(int i=0;i<points.size();i++){
//				if(isNewAdd[i]){
//					// add new nodes
//					this->addNode(points[i]);
//					Node *curNode = nodeList[nodeList.size()-1];
//					// connect the new added node with the old node in graph, which has coorespondence with last frame
//					for(int j=0;j<oldIndex.size();j++){
//						this->addUndirectedEdge(curNode, nodeList[oldIndex[j]]);
//					}
//					// connect the new node with the earlier new added node in this time
//					// form the beginning position of the new nodes to the end of the nodelist
//					for(int j=newNodeBeginIndex;j<nodeList.size();j++){
//						this->addUndirectedEdge(curNode, nodeList[j]);
//					}
//				}
//			}
//			delete []isNewAdd;
//		}
//	}
//	return true;
//}

bool Graph::updateGraph(vector<Point3f> points, Mat R, Mat T){
	// if there is no input points
	if(points.size()<=0){
		return false;
	} else {
		// if the graph contains no points
		if(this->nodeList.size()<=0){
			//this->addNodes(points);
			this->createCompleteGraph(points);
			return false;
		} else {
			//update the position of the old points
			for(int i=0;i<this->nodeList.size();i++){
				Mat oldPoint = Mat(this->nodeList[i]->getPoint());
				//cout<<"The oldPoint: "<<oldPoint<<endl;
				Mat newPoint = R*oldPoint + T;
				//cout<<"The newPoint: "<<newPoint<<endl;
				this->nodeList[i]->setPosition(Point3f(newPoint.at<float>(0,0),
													   newPoint.at<float>(1,0),
													   //2));
													   newPoint.at<float>(2,0)));
			}

			vector<Point3f> tempPoints = points;
			float e = 0.2;
			int timeThreshold = 10;
			for(int i=0;i<this->nodeList.size();i++){
				Node *currentNode = this->nodeList[i];
				int j;
				bool isFound = false;
				//find a coorespondence relationship to the new point			
				for(j=0;j<tempPoints.size();j++){
					//if they are very close
					if((fabs(currentNode->x - tempPoints[j].x) < e) && 
					   (fabs(currentNode->y - tempPoints[j].y) < e) && 
					   (fabs(currentNode->z - tempPoints[j].z) < e)){
						   //increase the life time of the node in graph
						   currentNode->timmer +=2;
						   //if the life time of the node is bigger than the threshold
						   if(currentNode->timmer >= timeThreshold){
							   currentNode->isFixed = true;
						   }
						   //delete the point from tempPoints
						   tempPoints.erase(tempPoints.begin()+j);
						   isFound = true;
						   break;
					}
				}
				//if there is no close poind found for the i-th node  
				if(!isFound){
					//if the node is not a fixed node and the life time is smaller than 0
					if((!currentNode->isFixed) && (--currentNode->timmer < 0)){
						this->deleteNode(i);
					}
					//if(--currentNode->timmer < 0){
					//	if(currentNode->isFixed){
					//		currentNode->isFixed = false;
					//		currentNode->timmer = timeThreshold;
					//	} else {
					//		this->deleteNode(i);
					//	}
					//}
				}
			}

			//add the rest new node into the nodelist
			this->addNodes(tempPoints);				
		}
	}
	return true;
}

Node *Graph::findNode(int index){
	for(int i=0;i<this->nodeList.size();i++){
		if(nodeList[i]->index=index){
			return nodeList[i];
		}
	}
	return NULL;
}

int Graph::getSize(){
	return this->nodeList.size();
}

vector<Point3f> Graph::getPoints(){
	vector<Point3f> points;
	for(int i=0;i<this->nodeList.size();i++){
		points.push_back(Point3f(*nodeList[i]));
	}
	return points;
}