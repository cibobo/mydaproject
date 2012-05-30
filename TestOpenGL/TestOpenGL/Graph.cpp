#include "stdafx.h"
#include "Graph.hpp"


/*
 * Defination for Edge
 */
Edge::Edge(Node *firstNode, Node *secondNode){
	this->orgNode = firstNode;
	this->dstNode = secondNode;
	this->cost = firstNode->distanceTo(secondNode);
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

void Node::setPosition(cv::Point3f pos){
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

//bool Graph::updateGraph(vector<Point3f> points, Mat R, Mat T){
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
//			//update the position of the old points
//			for(int i=0;i<this->nodeList.size();i++){
//				Mat oldPoint = Mat(this->nodeList[i]->getPoint());
//				//cout<<"The oldPoint: "<<oldPoint<<endl;
//				Mat newPoint = R*oldPoint + T;
//				//cout<<"The newPoint: "<<newPoint<<endl;
//				this->nodeList[i]->setPosition(Point3f(newPoint.at<float>(0,0),
//													   newPoint.at<float>(1,0),
//													   //2));
//													   newPoint.at<float>(2,0)));
//			}
//
//			vector<Point3f> tempPoints = points;
//			float e = 0.004;
//			int timeThreshold = 35;
//			for(int i=0;i<this->nodeList.size();i++){
//				Node *currentNode = this->nodeList[i];
//				int j;
//				bool isFound = false;
//				//find a coorespondence relationship to the new point			
//				for(j=0;j<tempPoints.size();j++){
//					//if they are very close
//					if((fabs(currentNode->x - tempPoints[j].x) < e) && 
//					   (fabs(currentNode->y - tempPoints[j].y) < e) && 
//					   (fabs(currentNode->z - tempPoints[j].z) < e)){ //&& !currentNode->isFixed){
//					//if(currentNode->distanceTo(tempPoints[j]) < e){
//						   //increase the life time of the node in graph
//						   currentNode->timmer +=2;
//						   //if the life time of the node is bigger than the threshold
//						   if(currentNode->timmer >= timeThreshold){
//							   currentNode->isFixed = true;
//							   this->fixedNodeList.push_back(currentNode);
//							   this->fixNodeCount++;
//						   }
//						   //delete the point from tempPoints
//						   tempPoints.erase(tempPoints.begin()+j);
//						   isFound = true;
//						   break;
//					}
//				}
//				//if there is no close poind found for the i-th node  
//				if(!isFound){
//					//if the node is not a fixed node and the life time is smaller than 0
//					if((!currentNode->isFixed) && (--currentNode->timmer < 0)){
//						this->deleteNode(i);
//					}
//					//if(--currentNode->timmer < 0){
//					//	if(currentNode->isFixed){
//					//		currentNode->isFixed = false;
//					//		currentNode->timmer = timeThreshold;
//					//	} else {
//					//		this->deleteNode(i);
//					//	}
//					//}
//				} 	
//			}
//
//			//add the rest new node into the nodelist
//			this->addNodes(tempPoints);	
//
//
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
			// create temp Graph to save the new detected points and their edges
			Graph *tempGraph = new Graph();
			tempGraph->createCompleteGraph(points);

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

			float e = 0.004;
			int timeThreshold = 25;
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
							   this->fixedNodeList.push_back(currentNode);
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

void Graph::showWithOpenCV(const char *name){
	//string windowName = "Object:";
	//windowName.append(name);
	//namedWindow(windowName.data(), CV_WINDOW_AUTOSIZE);

	//Mat drawMat = Mat::zeros(H_BILDSIZE, V_BILDSIZE, CV_8UC3);

	//for(int i=0;i<this->nodeList.size();;i++){
	//	circle(drawMat, nodeList[i], 2, Scalar(0,0,255,0), -1);
	//}
}

bool Graph::isEqual(Graph *other){
	// With abstruct epsilon
	//float e = 0.002;
	// With relative epsilon
	float e = 0.017;
	float rate = 0.95;
	float error = 0;
	NodePairs nodePairs;
	Node *center;
	return this->isEqualAdvance(other, e, rate, nodePairs, center, error);
}

bool Graph::isEqual1(Graph *other, float e, float rate){
	if(this->nodeList.size()<3){
		cout<<"The current graph has not enough points"<<endl;
		return false;
	}
	if(other->nodeList.size()<3){
		cout<<"The compared graph has not enough points"<<endl;
		return false;
	}

	// reset the timmer for each node, inoder to show the correspondenz nodes
	for(int i=0;i<other->nodeList.size();i++){
		other->nodeList[i]->timmer = 0;
	}

	
	// save the coorespondenz node pairs, where the first place save the node from other, and second for this
	map<Node*, Node*> pairs;

	int minNodeSize = this->nodeList.size()<other->nodeList.size()?this->nodeList.size():other->nodeList.size();
	int minParisCount = (minNodeSize * rate)>=3?(minNodeSize * rate):3;

	// Loop for all node in Other Graph
	for(int i=0;i<other->nodeList.size();i++){
		// choose one point from Other graph
		Node *p = other->nodeList[i];
		// Loop for all nodes in this Graph
		for(int k=0;k<this->nodeList.size();k++){
			Node *v = this->nodeList[k];
			
			// Insert the pair (p,v)
			pairs.insert(pair<Node*, Node*>(p,v));
			// Loop for all neighbor node of p
			//for(int j=0;j<p->edgeList.size();j++){
				// Get the neighbor nodes of p
				//Node *pi = p->edgeList[j].dstNode;
				//float p_pi = p->edgeList[j].cost;
			map<Node*, float>::iterator j;
			for(j=p->neighbors.begin();j!=p->neighbors.end();j++){
				// Get the neighbor nodes of p
				Node *pi = j->first;
				float p_pi = j->second;

				//for(int l=0;l<v->edgeList.size();l++){
				//	// Get the neighbor nodes of v
				//	Node *vi = v->edgeList[l].dstNode;
				//	float v_vi = v->edgeList[l].cost;
				map<Node*, float>::iterator l;
				for(l=v->neighbors.begin();l!=v->neighbors.end();l++){
					// Get the neighbor nodes of p
					Node *vi = l->first;
					float v_vi = l->second;

					// if the edge has the same lenth
					//if(fabs(p_pi - v_vi)< e){
					// with relative epsilon
					if(fabs(p_pi - v_vi)< e*p_pi){
						// add the node pair to the Map
						pairs.insert(pair<Node*, Node*>(pi, vi));
						break;
					}
				}
				// if more than 4 pairs have been found, return true;
				if(pairs.size()>minParisCount){
					// Mark correspondenz points
					map<Node*, Node*>::iterator it;
					for(it=pairs.begin();it!=pairs.end();it++){
						Node *node = (*it).first;
						node->timmer = -1;
					}
					return true;
				}
			}
			// if there is no coorespondenz pair between p and v, than clear the Map
			pairs.clear();
		}
	}
	cout<<"All nodes have been reached but no coorespondenz found!"<<endl;
	return false;
}

bool Graph::isEqualAdvance(Graph *other, float e, float rate, NodePairs &nodePairs, Node *&center, float &error){
	if(this->nodeList.size()<3){
		cout<<"The current graph has not enough points"<<endl;
		return false;
	}
	if(other->nodeList.size()<3){
		cout<<"The compared graph has not enough points"<<endl;
		return false;
	}

	// reset the timmer for each node, inoder to show the correspondenz nodes
	for(int i=0;i<other->nodeList.size();i++){
		other->nodeList[i]->timmer = 0;
	}

	
	// save the best coorespondenz node pairs, where the first place save the node from other, and second for this
	NodePairs maxPairs;
	//Node *centerNode;

	int minNodeSize = this->nodeList.size()<other->nodeList.size()?this->nodeList.size():other->nodeList.size();
	int minPairsCount = (minNodeSize * rate)>=4?(minNodeSize * rate):4;

	cout<<"The minimal allowed number of pointspaars is: "<<minPairsCount<<endl;

	// Loop for all node in Other Graph
	for(int i=0;i<other->nodeList.size();i++){
		// choose one point from Other graph
		Node *p = other->nodeList[i];
		// Loop for all nodes in this Graph
		for(int k=0;k<this->nodeList.size();k++){
			Node *v = this->nodeList[k];
			
			// save the coorespondenz neighbors of p(i) and v(k)
			NodePairs pair_k;
			pair_k.insert(pair<Node*, Node*>(p, v));

			// create temp error
			float tempError = 0;

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
						pair_k.insert(pair<Node*, Node*>(pi, vi));
						// collect the temp error
						tempError += diff;
						break;
					}
				}
			}
			// if find a better correspondenz
			int sizePairK = pair_k.size();
			if(sizePairK>maxPairs.size()){
				maxPairs.clear();
				maxPairs = pair_k;
				center = p;
				// save the error for the selected point
				error = tempError;
				cout<<"The resault: "<<sizePairK<<"   "<<error/sizePairK<<endl;
			}
		}
	}

	//if(maxPairs.size()>minNodeSize){
	//	cout<<"Superise!"<<endl;
	//}

	// check, whether the number of coorespondenz pairs is enough
	if(maxPairs.size()>=minPairsCount){
		// Mark correspondenz points
		//NodePairs::iterator it = maxPairs.begin();
		//for(;it!=maxPairs.end();it++){
		//	it->first->timmer = -1;
		//}
		//centerNode->timmer = -2;

		nodePairs = maxPairs;
		return true;
	} else {
		cout<<"All nodes have been reached but no coorespondenz found!"<<endl;
		return false;
	}
}



	

	