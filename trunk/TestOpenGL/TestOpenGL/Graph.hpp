#ifndef GRAPH_HPP
#define GRAPH_HPP


#include <cv.h>
//#include "ImageProcess.hpp"
#include "Evaluation.hpp"


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
	//vector<Edge> edgeList;
	map<Node*, float> neighbors;
	int index;
	int timmer;
	bool isFixed;
	// to save the color of the node, in order to sepreate the points of different plane
	int color;

	Node();
	Node(int timmer);
	//Node(Point2f point);
	Node(Point3f point);
	~Node();

	void addNeighbor(Node *node);
	bool deleteNeighbor(Node *node);

	void setPosition(Point3f pos);
	Point3f getPoint();
	float distanceTo(Node *other);
	float distanceTo(Point3f point);
};

class Graph{
public:
	vector<Node*> nodeList;
	vector<Node*> fixedNodeList;
	int indexCount;
	int lifeTime;
	int fixNodeCount;
	// The rotationmatrix from beginning status to the current status
	Mat R;
	Mat T;

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
	bool deleteNode(int index);

	void unionNode(Node *rsc, Node *dst);

	//void createMaxGraph(vector<vector<Point2f>> pointSets);
	void createCompleteGraph(vector<Point3f> points);
	int getSize();
	vector<Point3f> getPoints();
	//bool updateGraph(vector<Point3f> points);
	bool updateGraph(vector<Point3f> points, Mat R, Mat T);

	void showWithOpenCV(const char *name);
	bool isEqual(Graph *other);
	bool isEqual(Graph *other, float e, float rate);

	Node *findNode(int index);

	Point3f getMiddelPoint();
};

#endif
