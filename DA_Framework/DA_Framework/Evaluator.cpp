#include "stdafx.h"
#include "Evaluator.hpp"

Evaluator::Evaluator(){
	this->defaultRootPath = "./evaluation";
	//this->createPathWithTime();
	this->createPathWithDate();
}

Evaluator::Evaluator(const char *path){
	this->defaultRootPath = "./evaluation";
	this->createPath(path);
}

Evaluator::Evaluator(const char *path, const char *fileName){
	this->defaultRootPath = "./evaluation";
	this->createPath(path);
	this->createCSVFile(fileName);
}


void Evaluator::createPath(const char *subPath){
	CreateDirectory(subPath, NULL);
	string path = string(this->defaultRootPath);
	path.append("/");
	path.append(subPath);
	CreateDirectory(path.data(), NULL);
	this->defaultSavePath = path;
}

void Evaluator::createPathWithDate(){
	time_t curTime;
	time(&curTime);
	string subPath = ctime(&curTime);
	subPath = subPath.substr(0,10);
	createPath(subPath.data());
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

void Evaluator::writeCSVTitle(const char *title){
	ofstream *f  = (this->dataFiles.begin())->second;
	*(f)<<"#Description#"<<endl;
	*(f)<<"#"<<title<<"#"<<endl;
}

void Evaluator::writeCSVTitle(const char* fileName, const char* title){
	ofstream *f = this->dataFiles[fileName];
	*(f)<<"#Description#"<<endl;
	*(f)<<"#"<<title<<"#"<<endl;
}

void Evaluator::beginCSVLine(){
	this->evaLineData.clear();
}

void Evaluator::pushCSVData(float value){
	this->evaLineData.push_back(value);
}

void Evaluator::endCSVLine(){
	this->saveCSVData(this->evaLineData);
}
