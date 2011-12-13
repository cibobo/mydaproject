#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <cv.h>

using namespace std;
using namespace cv;

class Node;

class Edge{
public:
	Node *orgNode;
	Node *dstNode;
	float cost;

	Edge(Node *firstNode, Node *secondNode, float cost);
};

class Node : public Point3f{
public:
	vector<Edge> adjNodeList;
	int index;
	int timmer;

	Node();
	Node(int timmer);
	~Node();
};

class Graph{
public:
	vector<Node*> nodeList;
	int indexCount;

	Graph();
	~Graph();

	void addNode(Point3f &node);
	void addEdge(int orgIndex, int dstIndex);
	Node *findNode(int index);
};

#endif
