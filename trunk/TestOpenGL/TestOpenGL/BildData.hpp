#ifndef BILDDATA_HPP
#define BILDDATA_HPP

#include <cv.h>

using namespace std;
using namespace cv;

class BildData{
public:
	// Array and the Pointer of Distance Data
	float *disData;

	// Array and the Pointer of Intensity Data
	float *intData;

	// Array and the Pointer of Amplitude Data
	float *ampData;

	// Features
	vector<Point2f> features;

	int dataSize;


	BildData();
	~BildData();
};

#endif