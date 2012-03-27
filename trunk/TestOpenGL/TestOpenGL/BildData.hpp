#ifndef BILDDATA_HPP
#define BILDDATA_HPP

#include <cv.h>

using namespace std;
using namespace cv;

class BildData{
public:
	// Pointer of Distance Data
	float *disData;

	// Pointer of Intensity Data
	float *intData;

	// Pointer of Amplitude Data
	float *ampData;

	// Pointer of 3D Data
	float *threeDData;

	// Features
	vector<Point3f> features;

	// 2D Features for the OpenCV Windows
	vector<Point2f> features2D;

	// test complete features
	vector<Point3f> comFeatures;

	// to save the with DistanceFilter filtered data
	vector<Point3f> filted3DData;
	vector<Point2f> filted2DData;
	vector<int> filtedPointIndex;
	Mat clusterLabel;

	Mat featuresLabel;

	vector<vector<Point3f>> culsterGroupe;

	int width;
	int length;


	BildData();
	~BildData();
	void disCorrection();
	//BildData* operator =(BildData* oldData);
};

#endif