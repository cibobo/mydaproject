#ifndef EVALUATION_HPP
#define EVALUATION_HPP

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
	void createCSVFile(const char *fileName);
	
	void saveCVBild(const char *fileName, Mat data);
	void saveCSVData(vector<float> data);
	void saveCSVData(const char* fileName, vector<float> data);

	char *defaultRootPath;
	string defaultSavePath;
	map<const char*,ofstream*> dataFiles;
};

#endif