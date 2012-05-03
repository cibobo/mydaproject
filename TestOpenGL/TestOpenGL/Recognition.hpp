#include <cv.h>
#include <highgui.h>

#include "ImageProcess.hpp"
#include "Object.hpp"
#include "PMDPoint.hpp"

using namespace cv;

class Recognition {

public:
	Recognition();
	~Recognition();

	void objectRecognition(vector<PMDPoint> inputPoints);

	// to save the existed objects
	vector<Object*> objectList;
	// to save the input graph
	Graph *graph;
	Mat drawMat; 
};