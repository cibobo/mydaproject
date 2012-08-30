#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Graph.hpp"
#include <fstream>

class Object : public Graph{

public:
	Object();
	Object(const char* name);
	~Object();

	//Update the Object strucure graph with Rotaion and Translation matrix
	void update(vector<Point3f> points, Mat R, Mat T, float dThreshold, int tThreshold);

	//void getMiddelPoint(Point3f &mid);
	void clearUnfixedNodes();
	void joinSimilarNodes(float e);

	void transformate(Mat R, Mat T);

	void saveToVTKFile(const char *name);
	void loadFromVTKFile(const char *name);

	Mat sumR;
	Mat sumT;
	Mat curR;
	Mat curT;

	const char *defaultDataPath;
};

#endif