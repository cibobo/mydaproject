#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Graph.hpp"
#include <fstream>

using namespace std;
using namespace cv;

class Object : public Graph{

public:
	Object();
	Object(const char* name);
	~Object();

	void update(vector<Point3f> points, Mat R, Mat T);
	void initKalmanFilter();
	void updateKalmanFilter();
	void getMiddelPoint(Point3f &mid);

	void saveToVTKFile(const char *name);
	void loadFromVTKFile(const char *name);

	Mat sumR;
	Mat sumT;
	Mat curR;
	Mat curT;

	const char *defaultDataPath;

	// KalmanFilter for Translationsmatrix
	KalmanFilter tFilter;
	// KalmanFilter for the Qaternion
	KalmanFilter qFilter;
};

#endif