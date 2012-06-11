#include <cv.h>
#include <highgui.h>
#include <list>
#include <process.h>

#include "ImageProcess.hpp"
#include "Object.hpp"
#include "PMDPoint.hpp"
#include "OpenGLDraw.hpp"
//#include "OpenGLWin.hpp"

using namespace cv;
using namespace std;

static Scalar scalarColorList[6] = {Scalar(0,0,255,0),
									Scalar(0,255,255,0),
									Scalar(255,255,0,0),
									Scalar(255,0,255,0),
									Scalar(255,255,255,0),
									Scalar(180,180,180,0)};

class StatisticDate{
public:
	StatisticDate();
	//StaticDate(int index);

	~StatisticDate(){};

	int appear(int timmer);
	float getAppearanceRate(int timmer);

	//int objectIndex;
	int appearances;
	int lifetime;
	int continueFrames;
	int maxContinueFrames;
	int timmerDiff;
};


class RecognitionResult {
public:
	typedef map<Node*, Node*> NodePairs;
	
	RecognitionResult();
	RecognitionResult(NodePairs nodePair, float weight,int objIndex);
	RecognitionResult(NodePairs nodePair, Node *center, float weight,int objIndex);
	~RecognitionResult(){};

	void mark();
	void markColorful();

	NodePairs nodePair;
	Node* center;
	float weight;
	int objIndex;
};


class Recognition {

public:
	typedef map<Node*, Node*> NodePairs;

	Recognition();
	~Recognition();

	void objectRecognition(vector<PMDPoint> inputPoints);
	void createStatisticMap();

	// update the result list and return the index of the best result
	void updateResultList(int index, NodePairs resultPair, Node *center);
	// second version
	void updateResultList(int index, NodePairs resultPair);
	void updateObjectPosition(int index, NodePairs resultPair);

	//void drawGraphWithOpenGL();

	int findBestResult();

	// to save the existed objects
	vector<Object*> objectList;
	// to save the input graph
	Graph *graph;
	Mat drawMat; 
	// Parameter for the evaluation of the result
	int maxListLength;
	int timmer;
	
	vector<map<int,StatisticDate>> Statistic;
	// to save the recognition's results
	vector<RecognitionResult> resultList;
};