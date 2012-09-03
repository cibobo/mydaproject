#ifndef GRAPH_HPP
#define GRAPH_HPP


#include <cv.h>

using namespace std;
using namespace cv;

/**
 * Definition of the Nodes
 **/
class Node : public Point3f{
public:
	//Type Definition
	typedef map<Node*, Node*> NodePairs;
	typedef map<Node*, float> Neighbors;

	//To save all neighbors, which have edge connecting to the current node
	Neighbors neighbors;

	int index;
	//The life time of the node
	int timmer;
	//Whether this node is as fixed node recognized
	bool isFixed;
	//To save the color of the node, in order to sepreate the points of different plane
	int color;

	Node();
	Node(int timmer);
	Node(Point3f point);
	~Node();

	void setPoint(Point3f pos);
	Point3f getPoint();

	float distanceTo(Node *other);
	float distanceTo(Point3f point);

	void addNeighbor(Node *node);
	bool deleteNeighbor(Node *node);
	bool hasNeighbor(Node *other);

	/*
	 * Check all neighbors of both 'this' node and 'other' node 
	 * @param: other target node
	 * @param: nodePairs save similar neighbors pairs from both this and other
	 * @param: e the proportion of the comparision of the distance 
	 */
	void findCorresNeighbors(Node *other, NodePairs &nodePairs, float e);
};


/**
 * Definition of Graph
 **/
class Graph{
public:
	/*
	 * Type Definition
	 */
	//NodePairs for the Graph Isomorphismus
	typedef map<Node*, Node*> NodePairs;
	//Each element of Neighbors contains the point of the neighbor node and the edge length between them
	typedef map<Node*, float> Neighbors;

	/*
	 * Data Parameters
	 */
	//Vector of Nodes to save all nodes in graph
	vector<Node*> nodeList;
	//vector<Node*> fixedNodeList;
	int indexCount;
	int lifeTime;
	int fixNodeCount;

	// The rotationmatrix from beginning status to the current status
	Mat R;
	Mat T;

	//Constructors
	Graph();
	Graph(vector<Point3f> points);
	~Graph();

	/*
	 * Standard Functions for graph
	 */
	void addNode(Point3f &node);
	void addNodes(vector<Point3f> points);

	void addEdge(Node * begin, Node* end);
	void addUndirectedEdge(Node *first, Node *second);

	bool deleteEdge(Node *begin, Node *end);
	bool deleteUndirectedEdge(Node *first, Node *second);

	bool deleteNode(Node *node);
	bool deleteNode(int index);

	void unionNode(Node *rsc, Node *dst);
	void unionGraph(Graph *graph);

	//void createMaxGraph(vector<vector<Point2f>> pointSets);
	//Create a complete graph with input points
	void createCompleteGraph(vector<Point3f> points);

	int getSize();
	vector<Point3f> getPoints();

	//Update current graph with the new points and the rotation and traslation matrix
	bool updateGraph(vector<Point3f> points, Mat R, Mat T, float dThreshold=0.004, int tThreashold=25);
	
	/*
	 * Functions for the Graph Isomorphismus
	 * @param: other the target graph of comparision
	 * @param: nodePairs saving the pointer of associate Nodes, where the first place for the node in Graph 'other'
	 * @param: disProp the distance threshold for the identical nodes
	 * @param: countProp the proportion of input points, which should be recognized as correspondent neighbors, if two graphs are recognized as the same 
	 * @param: e the distance threshold to check the number of neighbors of the selected nodePairs
	 */
	bool isEqual(Graph *other, NodePairs &nodePairs, float disProp=0.05, float countProp=0.5, float e=0.008);
	// Helping functionf for the Graph Isomorphismus
	//Select the nodes from nodePairs, which has at least 2 neighbors. Save the result into triangle 
	bool findTriangle(NodePairs nodePairs, NodePairs &triangle, float e);

	Node *findNode(int index);
	//find the index of the node in nodelist. If it doesn't exist, return -1, otherweise return the index
	int findIndex(Node *node);

	Point3f getMiddelPoint();
	//set the color with the index in color list
	void setColor(int colorIndex);
	//set the color for all node to null
	void clearColor();


};

#endif
