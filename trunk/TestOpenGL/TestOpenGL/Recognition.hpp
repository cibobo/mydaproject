#include <cv.h>
#include <highgui.h>

#include "ImageProcess.hpp"
#include "Object.hpp"
#include "PMDPoint.hpp"

using namespace cv;

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


class Recognition {

public:
	Recognition();
	~Recognition();

	void objectRecognition(vector<PMDPoint> inputPoints);
	void createStatisticMap();

	// to save the existed objects
	vector<Object*> objectList;
	// to save the input graph
	Graph *graph;
	Mat drawMat; 
	int timmer;
	
	vector<map<int,StatisticDate>> Statistic;
};