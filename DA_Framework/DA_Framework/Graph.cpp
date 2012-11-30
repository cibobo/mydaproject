#include "stdafx.h"
#include "Graph.hpp"

#include "stdafx.h"
#include "Graph.hpp"

/*
 * Defination for Node
 */
Node::Node(){
	this->timmer = 3;
	this->isFixed = false;
	this->color = -1;
}

Node::Node(int timmer){
	this->timmer = timmer;
	this->isFixed = false;
	this->color = -1;
}

Node::Node(Point3f point){
	this->x = point.x;
	this->y = point.y;
	this->z = point.z;
	this->timmer = 3;
	this->isFixed = false;
	this->color = -1;
}

Node::~Node(){
	//this->adjNodeList.clear();
}

void Node::addNeighbor(Node *node){
	float distance = this->distanceTo(node);
	this->neighbors.insert(pair<Node*, float>(node, distance));
}

bool Node::deleteNeighbor(Node *node){
	if(this->neighbors.size() <= 0){
		return false;
	}

	map<Node*, float>::iterator it = this->neighbors.find(node);
	if(it == this->neighbors.end()){
		return false;
	} else {
		this->neighbors.erase(it);
	}
	return true;
}

void Node::setPoint(cv::Point3f pos){
	this->x = pos.x;
	this->y = pos.y;
	this->z = pos.z;
}

Point3f Node::getPoint(){
	Point3f point = Point3f(this->x, this->y, this->z);
	return point;
}

float Node::distanceTo(Node *other){
	//return sqrt((this->x-other->x)*(this->x-other->x) + (this->y-other->y)*(this->y-other->y) + (this->z-other->z)*(this->z-other->z)); 
	return this->distanceTo(other->getPoint());
}

float Node::distanceTo(cv::Point3f point){
	return sqrt((this->x-point.x)*(this->x-point.x) + (this->y-point.y)*(this->y-point.y) + (this->z-point.z)*(this->z-point.z));
}

bool Node::hasNeighbor(Node *other){
	if(this->neighbors.find(other) != this->neighbors.end()){
		return true;
	} else {
		return false;
	}
}

void Node::findCorresNeighbors(Node *other, NodePairs &nodePairs, float e){
	Node *v = this;
	Node *p = other;
	nodePairs.insert(pair<Node*, Node*>(p,v));
	// begin the loop of vi first, in order to keep the vi unique in Pairs Map
	Neighbors::iterator l;
	for(l=v->neighbors.begin();l!=v->neighbors.end();l++){
		// Get the neighbor nodes of p
		Node *vi = l->first;
		float v_vi = l->second;
	
		// Loop for all neighbor node of p
		Neighbors::iterator j;
		for(j=p->neighbors.begin();j!=p->neighbors.end();j++){
			// Get the neighbor nodes of p
			Node *pi = j->first;
			float p_pi = j->second;
			
			float diff = fabs(p_pi - v_vi);
			// if the edge has the same lenth
			//if(diff < e){
			// with relative epsilon
			if(diff < e*p_pi){
				// add the node pair to the Map
				nodePairs.insert(pair<Node*, Node*>(pi, vi));
				// collect the temp error
				//tempError += diff;
				break;
			}
		}
	}
}


/*
 * Defination for Graph
 */
Graph::Graph(){
	this->indexCount = 0;
	this->lifeTime = 0;
	this->fixNodeCount = 0;
	this->R = Mat::eye(3,3,CV_32FC1);
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
	this->fixNodeCount = 0;
	this->R = Mat::eye(3,3,CV_32FC1);
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

bool Graph::deleteNode(Node *node){
	//map<Node*, float>::iterator it;
	//for(it = node->neighbors.begin();it!=node->neighbors.end();it++){
	//	// delete the edge, which direct to this node
	//	if(!deleteEdge(it->first, node)){
	//		return false;
	//	}
	//}
	//for(int i=0;i<this->nodeList.size();i++){
	//	if(nodeList[i] == node){
	//		nodeList.erase(nodeList.begin() + i);
	//		delete node;
	//		break;
	//	}
	//}
	//return true;
	int index = this->findIndex(node);
	if(index == -1){
		return false;
	} else {
		return this->deleteNode(index);
	}
}

bool Graph::deleteNode(int index){
	Node *node = nodeList[index];
	map<Node*, float>::iterator it;
	for(it = node->neighbors.begin();it!=node->neighbors.end();it++){
		// delete the edge, which direct to this node
		if(!deleteEdge(it->first, node)){
			return false;
		}
	}
	nodeList.erase(nodeList.begin() + index);
	delete node;
	return true;
}

void Graph::addEdge(Node *begin, Node *end){
	begin->addNeighbor(end);
}

void Graph::addUndirectedEdge(Node *first, Node *second){
	this->addEdge(first, second);
	this->addEdge(second, first);
}

bool Graph::deleteEdge(Node *begin, Node *end){
	return begin->deleteNeighbor(end);
}

//void Graph::addEdge(Node *begin, Node *end){
//	begin->edgeList.push_back(Edge(begin, end));
//}
//

//	
//bool Graph::deleteEdge(Node *begin, Node *end){
//	for(int i=0;i<begin->edgeList.size();i++){
//		if(begin->edgeList[i].dstNode == end){
//			begin->edgeList.erase(begin->edgeList.begin() + i);
//			return true;
//		}
//	}
//	// if the edge not found, return false
//	return false;
//}
//
bool Graph::deleteUndirectedEdge(Node *first, Node *second){
	return deleteEdge(first, second) && deleteEdge(second, first);
}

void Graph::unionNode(Node *rsc, Node *dst){
	map<Node*, float>::iterator it;
	// add all neighbors of dst as the neighbors of rsc
	for(it=dst->neighbors.begin();it!=dst->neighbors.end();it++){
		// without the node rsc itself
		if(it->first != rsc){
			this->addUndirectedEdge(rsc, it->first);
		}
	}
	//this->deleteNode(dst);
}

void Graph::unionGraph(Graph *graph){
	for(int i=0;i<graph->nodeList.size();i++){
		this->nodeList.push_back(graph->nodeList[i]);
	}
}

//bool Graph::deleteNode(Node *node){
//	for(int i=0;i<node->edgeList.size();i++){
//		// delete the edge, which direct to this node
//		if(!deleteEdge(node->edgeList[i].dstNode, node->edgeList[i].orgNode)){
//			return false;
//		}
//	}
//	for(int i=0;i<this->nodeList.size();i++){
//		if(nodeList[i] == node){
//			nodeList.erase(nodeList.begin() + i);
//			delete node;
//			break;
//		}
//	}
//	return true;
//}
//
//bool Graph::deleteNode(int index){
//	Node *node = nodeList[index];
//	for(int i=0;i<node->edgeList.size();i++){
//		// delete the edge, which direct to this node
//		if(!deleteEdge(node->edgeList[i].dstNode, node->edgeList[i].orgNode)){
//			return false;
//		}
//	}
//	nodeList.erase(nodeList.begin() + index);
//	delete node;
//	return true;
//}


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
	this->nodeList.clear();
	this->addNodes(points);
	for(int i=0;i<this->nodeList.size();i++){
		for(int j=i+1;j<this->nodeList.size();j++){
		this->addUndirectedEdge(nodeList[i], nodeList[j]);
		}
	}
}

bool Graph::updateGraph(vector<Point3f> points, Mat R, Mat T, float dThreshold, int tThreshold){
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
			// create temp Graph to save the new detected points and their edges
			Graph *tempGraph = new Graph();
			tempGraph->createCompleteGraph(points);

			//update the position of the old points
			for(int i=0;i<this->nodeList.size();i++){
				Mat oldPoint = Mat(this->nodeList[i]->getPoint());
				//cout<<"The oldPoint: "<<oldPoint<<endl;
				Mat newPoint = R*oldPoint + T;
				//cout<<"The newPoint: "<<newPoint<<endl;
				this->nodeList[i]->setPoint(Point3f(newPoint.at<float>(0,0),
													newPoint.at<float>(1,0),
													//2));
													newPoint.at<float>(2,0)));
			}

			float e = dThreshold;
			int timeThreshold = tThreshold;
			for(int i=0;i<this->nodeList.size();i++){
				Node *currentNode = this->nodeList[i];
				int j;
				bool isFound = false;
				//find a coorespondence relationship to the new point			
				for(j=0;j<tempGraph->nodeList.size();j++){
					//if they are very close
					//if((fabs(currentNode->x - tempGraph->nodeList[j]->x) < e) && 
					//   (fabs(currentNode->y - tempGraph->nodeList[j]->y) < e) && 
					//   (fabs(currentNode->z - tempGraph->nodeList[j]->z) < e)){ //&& !currentNode->isFixed){
					if(currentNode->distanceTo(tempGraph->nodeList[j]) < e){
						   //increase the life time of the node in graph
						   currentNode->timmer +=2;
						   //if the life time of the node is bigger than the threshold
						   if(currentNode->timmer >= timeThreshold){
							   currentNode->isFixed = true;
							   //this->fixedNodeList.push_back(currentNode);
							   this->fixNodeCount++;
						   }
						   // union the correspondenz points
						   this->unionNode(currentNode, tempGraph->nodeList[j]);
						   // delete the point from tempGraph
						   tempGraph->deleteNode(j);
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
				} 	
			}

			//add the rest new node into the nodelist
			//this->addNodes(tempGraph->getPoints());	
			this->unionGraph(tempGraph);
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

int Graph::findIndex(Node *node){
	for(int i=0;i<this->nodeList.size();i++){
		if(this->nodeList[i] == node){
			return i;
		}
	}
	return -1;
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

Point3f Graph::getMiddelPoint(){
	Mat nodes = Mat(this->getPoints(), true);
	Scalar midPointS = mean(nodes);
	Mat midPointM = Mat(midPointS).rowRange(0,3);
	midPointM.convertTo(midPointM, CV_32FC1);
	Point3f midPoint = Point3f(midPointM.at<float>(0,0), midPointM.at<float>(1,0), midPointM.at<float>(2,0));
	return midPoint;
}

void Graph::setColor(int colorIndex){
	for(int i=0;i<this->nodeList.size();i++){
		this->nodeList[i]->color = colorIndex;
	}
}

void Graph::clearColor(){
	for(int i=0;i<this->nodeList.size();i++){
		this->nodeList[i]->color = 0;
	}
}

bool Graph::isEqual(Graph *other, NodePairs &nodePairs, float disProp, float countProp, float e){
	if(this->nodeList.size()<3){
		cout<<"The current graph has not enough points"<<endl;
		return false;
	}
	if(other->nodeList.size()<3){
		cout<<"The compared graph has not enough points"<<endl;
		return false;
	}

	NodePairs localNodePair;

	// Begin the loop of V first, to keep the 1:1 coorespondenz in result
	// Loop for all nodes in this Graph
	for(int k=0;k<this->nodeList.size();k++){
		Node *v = this->nodeList[k];

		bool isFound = false;
		// create the minimal acceptable number of numbers for point p
		int minNodeSize = (this->nodeList.size()*countProp)>=3?(this->nodeList.size()*countProp):3;

		int bestNeighborCount = 0;
		Node *bestP;

		// Loop for all node in Other Graph
		for(int i=0;i<other->nodeList.size();i++){
			// choose one point from Other graph
			Node *p = other->nodeList[i];
			
			// save the coorespondenz neighbors of p(i) and v(k)
			NodePairs pair_i;
			v->findCorresNeighbors(p, pair_i, disProp);

			if(pair_i.size() > bestNeighborCount){
				bestP = p;
				bestNeighborCount = pair_i.size();
			}
		}
		// if the v and p has enough coorespondenz neighbors
		if(bestNeighborCount >= minNodeSize){
			localNodePair.insert(pair<Node*, Node*>(bestP, v));
		}

	}
	// check the number of coorespondenz point pair
	nodePairs.clear();

	if(findTriangle(localNodePair, nodePairs, e)){

		return true;
	}
	cout<<"All nodes have been reached but no coorespondenz found!"<<endl;
	return false;
}

bool Graph::findTriangle(NodePairs nodePairs, NodePairs &triangle, float e){
	if(nodePairs.size() < 3){
		return false;
	}
	//check the neighbors in nodePairs
	NodePairs::iterator i = nodePairs.begin();
	for(;i!=nodePairs.end();i++){
		int neighborCount = 0;
		float error = 0;
		NodePairs::iterator j = nodePairs.begin();
		for(;j!=nodePairs.end();j++){
			// if i and j are neighbor
			if(i->first->hasNeighbor(j->first)){
				neighborCount++;
				// calculate the error with neighbors
				float pDist = i->first->neighbors[j->first];
				float vDist = i->second->neighbors[j->second];
				error += fabs(pDist-vDist);
			}
		}
		
		// if i has more than 2 neighbors 
		if(neighborCount >= 2){
			// if the average error is acceptable
			float aveError = error/neighborCount;
			if(aveError <= e){
				triangle.insert(pair<Node*, Node*>(i->first,i->second));
			}
		}
	}

	//if the number of left node is bigger than 3
	if(triangle.size() >= 3){
		return true;
	} else {
		return false;
	}
}


	

	