#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Graph.hpp"

using namespace std;
using namespace cv;

class Object : public Graph{

public:
	Object();
	~Object();

	void update(vector<Point3f> points, Mat R, Mat T);
	void initKalmanFilter();
	void updateKalmanFilter();

	Mat sumR;
	Mat sumT;
	Mat curR;
	Mat curT;

	// KalmanFilter for Translationsmatrix
	KalmanFilter tFilter;
	// KalmanFilter for the Qaternion
	KalmanFilter qFilter;
};

#endif