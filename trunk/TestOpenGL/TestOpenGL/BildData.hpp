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

	// test complete features
	vector<Point3f> comFeatures;

	int width;
	int length;


	BildData();
	~BildData();
	void disCorrection();
	//BildData* operator =(BildData* oldData);
};

#endif