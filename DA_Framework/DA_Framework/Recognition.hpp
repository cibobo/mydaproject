#include <cv.h>
#include <highgui.h>
#include <list>
#include <process.h>

#include "ImageProcess.hpp"
#include "Object.hpp"
#include "BildData.hpp"
#include "OpenGLDraw.hpp"
//#include "OpenGLWin.hpp"

using namespace cv;
using namespace std;

// Predefined Colorlist
static Scalar scalarColorList[6] = {Scalar(0,0,255,0),
									Scalar(0,255,255,0),
									Scalar(255,255,0,0),
									Scalar(255,0,255,0),
									Scalar(255,255,255,0),
									Scalar(180,180,180,0)};



class RecognitionResult {
public:
	typedef map<Node*, Node*> NodePairs;
	
	RecognitionResult();
	RecognitionResult(NodePairs nodePair, float weight,int objIndex);
	RecognitionResult(NodePairs nodePair, Node *center, float weight,int objIndex);
	~RecognitionResult(){};

	// Mark the node in Model as a special color
	void mark();
	void markColorful();

	NodePairs nodePair;
	Node* center;
	float weight;
	int modelIndex;

	// Parameter for the Association to the Segementation
	//Current Points
	vector<PMDPoint> nodes;
	//Life Time
	int lifeTime;
	//Threshold to evaluate the Association of the current result and the segementation
	float lastSumP;
	//Whether the points is updated 
	bool isFind;
};


class Recognition {

public:
	typedef map<Node*, Node*> NodePairs;

	Recognition();
	~Recognition();

	void loadModels();
	void loadModels(vector<string> names);
	void loadModels(string names);

	// Main functionf for the Recognition
	void objectRecognition(vector<PMDPoint> inputPoints);

	// Update the result list and return the index of the best result
	void updateResultList(int index, NodePairs resultPair, Node *center);
	// Second version of Update
	void updateResultList(int index, NodePairs resultPair);
	// Third version of Update
	void updateResultList(int objIndex, int modelIndex, NodePairs resultPair);
	// Update the position of the objects
	void updateObjectPosition(int index, NodePairs resultPair);

	/**
	 * Update the Reuslt List for more than one Objects
	 *
	 * Every element of the MutiResultList should be tracking with the current markers. 
	 **/
	void updateMultiResultList();

	//void drawGraphWithOpenGL();

	int findBestResult();
	int findBestResult(int objIndex);

	// to save the existed objects
	vector<Object*> modelList;
	// to save the input graph
	vector<Graph*> graphList;

	// Segmentations Result
	vector<vector<PMDPoint>> segResult;
	// Segmentations Parameters for DBSCAN
	float spatialCombiEps;
	int spatialCombiMinPts;

	// Parameter for the evaluation of the result
	int maxListLength;
	int timmer;
	
	// To save the recognition's results for one Object
	vector<RecognitionResult> resultList;
	// To save the recognitions' results for more than one objects
	vector<RecognitionResult*> multiResultList;
	map<int, map<int,RecognitionResult>> resultMap;

	vector<int> statisticResult;

	// Parameter for the Graph Isomorphismus
	float distanceProportion;
	float nodesCountProportion;
	float distanceThreshold;

	// The boolean parameters for the input's model of the recognition
	BOOL isTest;
	BOOL isLoad;
	BOOL isCreate;

	//Help Parameter for evaluation
	BOOL isInCurrentFound;

	int evaIsoRecogCount;
	int evaNodeCount;
};