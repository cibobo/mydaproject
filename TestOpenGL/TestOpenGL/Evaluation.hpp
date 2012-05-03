#include <stdlib.h>
#include <cv.h>
#include <highgui.h>

#include <fstream>
#include <iostream>


using namespace std;
using namespace cv;

class Evaluation {
public:
	Evaluation();
	Evaluation(const char *path);

	void createPath(const char *subPath);
	void createPathWithTime();
	void createCVSFile(const char *fileName);
	
	void saveCVBild(const char *fileName, Mat data);
	void saveCVSData(vector<float> data);

	char *defaultRootPath;
	string defaultSavePath;
	ofstream *dataFile;
};