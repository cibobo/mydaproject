#include "stdafx.h"
#include "Graph.hpp"


/*
 * Defination for Edge
 */
Edge::Edge(Node *firstNode, Node *secondNode, float cost){
	this->orgNode = firstNode;
	this->dstNode = secondNode;
	this->cost = cost;
}

/*
 * Defination for Node
 */
Node::Node(){}
Node::Node(int timmer){
	this->timmer = timmer;
}

Node::~Node(){
	this->adjNodeList.clear();
}

/*
 * Defination for Graph
 */
Graph::Graph(){
	this->indexCount = 0;
}

Graph::~Graph(){
	for(int i=0;i<this->nodeList.size();i++){
		delete this->nodeList[i];
	}
	this->nodeList.clear();
}

void Graph::addNode(Point3f &node){
	Node *newNode = (Node*)(new Point3f(node));
	this->indexCount++;
	newNode->index = this->indexCount;
	this->nodeList.push_back(newNode);
	
}

Node *Graph::findNode(int index){
	for(int i=0;i<this->nodeList.size();i++){
		if(nodeList[i]->index=index){
			return nodeList[i];
		}
	}
	return NULL;
}