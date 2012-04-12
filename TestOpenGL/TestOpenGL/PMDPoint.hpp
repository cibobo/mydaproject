#ifndef PMDPOINT_HPP
#define PMDPOINT_HPP

#include <cv.h>

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

#endif