#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <cv.h>

using namespace std;
using namespace cv;

class Object{

public:
	Object();
	Object(std::vector<Point2f> points);
	~Object();

	void addNewMarker(Point2f point);
	void addNewMarkers(vector<Point2f> points);
	int getSize();

private:
	std::vector<Point2f> markers;
};

#endif