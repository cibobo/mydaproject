#ifndef MYFEATUREDETECTOR_HPP
#define MYFEATUREDETECTOR_HPP

#include <highgui.h>

#include "BildData.hpp"
#include "ImageProcess.hpp"

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
	int brightnessControllSTAR(unsigned char *tempData);


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