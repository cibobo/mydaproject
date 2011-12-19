#include "stdafx.h"
#include "Graph.hpp"


/*
 * Defination for Edge
 */
Edge::Edge(Node *firstNode, Node *secondNode){
	this->orgNode = firstNode;
	this->dstNode = secondNode;
	this->vec = *secondNode - *firstNode;
}

Edge::Edge(Node *firstNode, Node *secondNode, float cost){
	this->orgNode = firstNode;
	this->dstNode = secondNode;
	this->cost = cost;
	this->vec = *secondNode - *firstNode;
}

/*
 * Defination for Node
 */
Node::Node(){
	this->timmer = 1;
}

Node::Node(int timmer){
	this->timmer = timmer;
}

Node::Node(Point2f point){
	this->x = point.x;
	this->y = point.y;
	this->timmer = 1;
}

Node::~Node(){
	//this->adjNodeList.clear();
}

void Node::setPosition(cv::Point2f pos){
	this->x = pos.x;
	this->y = pos.y;
}



/*
 * Defination for Graph
 */
Graph::Graph(){
	this->indexCount = 0;
	this->lifeTime = 0;
}

Graph::Graph(vector<Point2f> points){
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

void Graph::addNode(Point2f &node){
	Node *newNode = new Node(node);
	this->indexCount++;
	newNode->index = this->indexCount;
	this->nodeList.push_back(newNode);
}

void Graph::addNodes(vector<Point2f> points){
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


void Graph::createMaxGraph(vector<vector<Point2f>> pointSets){
	int maxSize = 0;
	int maxSizeIndex = 0;
	for(int i=0;i<pointSets.size();i++){
		if(pointSets[i].size()>maxSize){
			maxSize = pointSets[i].size();
			maxSizeIndex = i;
		}
	}
	if(maxSize > 0){
		this->addNodes(pointSets[i]);
	}
}

void Graph::createCompleteGraph(vector<Point2f> points){
	this->addNodes(points);
	for(int i=0;i<this->nodeList.size();i++){
		for(int j=i+1;j<this->nodeList.size();j++){
		this->addUndirectedEdge(nodeList[i], nodeList[j]);
		}
	}
}


bool Graph::updateGraph(vector<Point2f> points){
	// increase the lifetime
	this->lifeTime ++;
	// if there is no input points
	if(points.size()<=0){
		return false;
	} else {
		// if the graph contains no points
		if(this->nodeList.size()<=0){
			this->addNodes(points);
			return false;
		} else {
			vector<Point2f> oldPoints;
			for(int i=0;i<this->nodeList.size();i++){
				if(nodeList[i]->timmer < 0){
					nodeList.erase(nodeList.begin()+i);
					i--;
				} else {
					nodeList[i]->timmer--;
					oldPoints.push_back(Point2f(*nodeList[i]));
				}
			}
			//SVD
			vector<int> oldIndex, newIndex;
			featureAssociate(oldPoints, points, 18, oldIndex, newIndex);
			
			// set a new array to mark the new point, which will be add into the node List
			bool *isNewAdd = new bool[points.size()];
			for(int i=0;i<points.size();i++){
				isNewAdd[i] = true;
			}

			for(int i=0;i<oldIndex.size();i++){
				// reset the timmer for the coorespondence points
				this->nodeList[oldIndex[i]]->timmer += 2;
				this->nodeList[oldIndex[i]]->x = points[newIndex[i]].x;
				this->nodeList[oldIndex[i]]->y = points[newIndex[i]].y;	
				// if the point can find a coorespondence with the old point, set the value in the Mark-Array to false
				isNewAdd[newIndex[i]] = false;
			}

			for(int i=0;i<points.size();i++){
				if(isNewAdd[i]){
					this->addNode(points[i]);
				}
			}
			delete []isNewAdd;
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

vector<Point2f> Graph::getPoints(){
	vector<Point2f> points;
	for(int i=0;i<this->nodeList.size();i++){
		points.push_back(Point2f(*nodeList[i]));
	}
	return points;
}