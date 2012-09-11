#include "stdafx.h"
#include "Evaluator.hpp"

Evaluator::Evaluator(){
	this->defaultRootPath = "./evaluation";
	this->createPathWithTime();
}

Evaluator::Evaluator(const char *path){
	this->defaultRootPath = "./evaluation";
	this->createPath(path);
}

void Evaluator::createPath(const char *subPath){
	string path = string(this->defaultRootPath);
	path.append("/");
	path.append(subPath);
	CreateDirectory(path.data(), NULL);
	this->defaultSavePath = path;
}

void Evaluator::createPathWithTime(){
	time_t curTime;
	//curTime = time(NULL);
	//stringstream ss;
	//ss<<curTime;
	//string subPath = ss.str();
	time(&curTime);
	string subPath = ctime(&curTime);
	// exchange the : to -
	subPath[13] = '-';
	subPath[16] = '-';
	// remove the last character of the string, which may be set as \0.
	subPath.erase(24,1);
	createPath(subPath.data());
}

void Evaluator::createCSVFile(const char *fileName){
	string path = string(this->defaultSavePath);
	path.append("/");
	path.append(fileName);
	path.append(".csv");
	ofstream *f = new ofstream(path.data(), ios::app);
	this->dataFiles.insert(pair<const char*, ofstream*>(fileName, f));
}

void Evaluator::saveCVBild(const char *fileName, Mat data){
	string path = string(this->defaultSavePath);
	path.append("/");
	path.append(fileName);
	path.append(".png");

	cv::imwrite(path, data);
}

void Evaluator::saveCSVData(vector<float> data){
	// default for the first file
	//ofstream *f = (*(this->dataFiles.begin())).second;
	//for(int i=0;i<data.size();i++){
	//	*(f)<<data[i]<<" ";
	//}
	//*(f)<<endl;
	const char *fileName = (*(this->dataFiles.begin())).first;
	this->saveCSVData(fileName, data);
}

void Evaluator::saveCSVData(const char* fileName, vector<float> data){
	ofstream *f = this->dataFiles[fileName];
	for(int i=0;i<data.size();i++){
		*(f)<<data[i]<<" ";
	}
	*(f)<<endl;
}