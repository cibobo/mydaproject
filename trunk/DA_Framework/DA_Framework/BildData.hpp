#ifndef BILDDATA_HPP
#define BILDDATA_HPP

#include <cv.h>

using namespace std;
using namespace cv;


/************************************
 *
 * To save all information from PMD Camera for a point
 *
 ***********************************/
class PMDPoint{
public:
	// The 3D coordinaten of the point
	Point3f coord;
	// The index of the point in OpenCV windows
	Point2f index;

	PMDPoint(){};
	PMDPoint(Point3f coord, Point2f index);
	~PMDPoint(){};
};


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

	vector<PMDPoint> features;

	// test complete features
	vector<PMDPoint> comFeatures;

	int width;
	int length;

	BildData();
	~BildData();
};

#endif