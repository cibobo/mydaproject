#include "stdafx.h"
#include "Evaluation.hpp"

Evaluation::Evaluation(){
	this->defaultRootPath = "./evaluation";
	this->createPathWithTime();
}

Evaluation::Evaluation(const char *path){
	this->defaultRootPath = "./evaluation";
	this->createPath(path);
}

void Evaluation::createPath(const char *subPath){
	string path = string(this->defaultRootPath);
	path.append("/");
	path.append(subPath);
	CreateDirectory(path.data(), NULL);
	this->defaultSavePath = path;
}

void Evaluation::createPathWithTime(){
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

void Evaluation::createCVSFile(const char *fileName){
	string path = string(this->defaultSavePath);
	path.append("/");
	path.append(fileName);
	path.append(".cvs");
	this->dataFile = new ofstream(path.data(), ios::app);
}

void Evaluation::saveCVBild(const char *fileName, Mat data){
	string path = string(this->defaultSavePath);
	path.append("/");
	path.append(fileName);
	path.append(".png");

	cv::imwrite(path, data);
}

void Evaluation::saveCVSData(vector<float> data){
	for(int i=0;i<data.size();i++){
		*(this->dataFile)<<data[i]<<" ";
	}
	*(this->dataFile)<<endl;
}