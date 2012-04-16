#ifndef MYFEATUREDETECTOR_HPP
#define MYFEATUREDETECTOR_HPP

#include <opencv/cv.h>
#include <opencv/highgui.h>

//#include "ImageProcess.hpp"

using namespace cv;
using namespace std;

class MyFeatureDetector{
public:
	MyFeatureDetector(){};
	MyFeatureDetector(int maxFeatureCount, int minFeatureCount, int maxLoopCount);
	~MyFeatureDetector(){};

	void usingSTAR();
	void usingSURF();

	void setDetectedData(float *data);
	void setDetectedMat(Mat data);
	int brightnessControllSTAR();
	void getFeaturePoints(vector<Point2f> &features, float eps);


	Mat detectedMat;
	Mat drawMat;
	float *detectedData;
	vector<KeyPoint> keypoints;

	int maxFeatureCount;
	int minFeatureCount;
	int maxLoopCount;
	int balance;
	float contrast;
	int detecParam;
};

#endif