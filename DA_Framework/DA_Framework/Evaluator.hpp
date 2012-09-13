#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <stdlib.h>
#include <cv.h>
#include <highgui.h>

#include <fstream>
#include <iostream>


using namespace std;
using namespace cv;

class Evaluator {
public:
	Evaluator();
	Evaluator(const char *path);
	// Create a CSV file in constructor
	Evaluator(const char *path, const char *fileName);

	// Create the saving path of the evaluation's data
	void createPath(const char *subPath);
	// Create the saving path with the current time
	void createPathWithTime();
	// Create a csv File with the user defined name
	void createCSVFile(const char *fileName);
	// Save the OpenCV bild with the user defined file name
	void saveCVBild(const char *fileName, Mat data);
	// Save the data into the default csv File
	void saveCSVData(vector<float> data);
	// Save the data into the user defined csv File
	void saveCSVData(const char* fileName, vector<float> data);

	// Begin to saving the data in a new line of the CSV file
	void beginCSVLine();
	// Save data into the current line of CSV file
	void pushCSVData(float value);
	// End the line
	void endCSVLine();

	// Vector to save a line of data, which will be saved into the CSV file
	vector<float> evaLineData;

	char *defaultRootPath;
	string defaultSavePath;
	map<const char*,ofstream*> dataFiles;
};

#endif