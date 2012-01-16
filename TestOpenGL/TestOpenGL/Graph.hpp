#ifndef GRAPH_HPP
#define GRAPH_HPP


#include <cv.h>
#include "ImageProcess.hpp"

using namespace std;
using namespace cv;

class Node;

class Edge{
public:
	Node *orgNode;
	Node *dstNode;
	float cost;
	// save the edge with the direction
	//Point3f vec;
	//Point2f vec;

	Edge(Node *firstNode, Node *secondNode);
	Edge(Node *firstNode, Node *secondNode, float cost);
};

class Node : public Point3f{
//class Node : public Point2f{
public:
	vector<Edge> edgeList;
	int index;
	int timmer;

	Node();
	Node(int timmer);
	//Node(Point2f point);
	Node(Point3f point);
	~Node();

	//void setPosition(Point2f pos);
	void setPosition(Point3f pos);
};

class Graph{
public:
	vector<Node*> nodeList;
	int indexCount;
	int lifeTime;

	Graph();
	Graph(vector<Point3f> points);
	~Graph();

	void addNode(Point3f &node);
	void addNodes(vector<Point3f> points);
	void addEdge(Node * begin, Node* end);
	void addUndirectedEdge(Node *first, Node *second);

	bool deleteEdge(Node *begin, Node *end);
	bool deleteUndirectedEdge(Node *first, Node *second);
	bool deleteNode(Node *node);

	//void createMaxGraph(vector<vector<Point2f>> pointSets);
	void createCompleteGraph(vector<Point3f> points);
	int getSize();
	vector<Point3f> getPoints();
	bool updateGraph(vector<Point3f> points);

	Node *findNode(int index);
};

#endif
