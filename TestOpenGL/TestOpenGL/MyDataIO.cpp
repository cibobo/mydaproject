#include "stdafx.h"
#include "MyDataIO.hpp"

using namespace std;

//the path of the saved Data
const char *defaultSavePath = "data/4/";

//default load path
const char *defaultLoadPath = "data/4/";

bool isDirectoryExist(const char *directory){
	int returnvalue;
	returnvalue = GetFileAttributes(directory);
	if(returnvalue == (-1)){
		return false;
	} else {
		return true;
	}
}

bool createDirectory(const char *directory){
	if(isDirectoryExist(directory)){
		cout<<"The directory: "<<directory<<" has been already exist!"<<endl;
		return false;
	} else {
		CreateDirectory(directory, NULL);
		return true;
	}
}

void createDefaultPMDDataDirectory(){
	//check the root path of the new data
	if(isDirectoryExist(defaultSavePath)){
		cout<<"The root directory: "<<defaultSavePath<<" has been already exist!"<<endl;
		exit(3);
	} else {
		CreateDirectory(defaultSavePath, NULL);
		
		string disPath;
		disPath.append(defaultSavePath);
		disPath.append("distance");
		CreateDirectory(disPath.data(), NULL);

		string ampPath;
		ampPath.append(defaultSavePath);
		ampPath.append("amplitude");
		CreateDirectory(ampPath.data(), NULL);

		string intPath;
		intPath.append(defaultSavePath);
		intPath.append("intensity");
		CreateDirectory(intPath.data(), NULL);
	}
}


/*
 * save the data to the File with the type name: distance, amplitude, intentsity,
 * the index of frame and the pointer of the containing array
 */
void saveNormalDataToFile(const char *type, int frameCount, float *data){
	string fullPath;
	string fileName;
	if(frameCount<10){
		fileName.append(3,'0');
		fileName.append(1,char(frameCount+48));
	} else if(frameCount>=10 && frameCount<100){
		fileName.append(2,'0');
		fileName.append(1,char((frameCount/10)+48));
		fileName.append(1,char((frameCount%10)+48));
	} else if(frameCount>=100 && frameCount<1000){
		fileName.append(1,'0');
		fileName.append(1,char((frameCount/100)+48));
		fileName.append(1,char(((frameCount%100)/10)+48));
		fileName.append(1,char((frameCount%10)+48));
	} else {
		cout<<"the file name is bigger than 1000"<<endl;
		exit(0);
	}

	fullPath.append(defaultSavePath);
	fullPath.append(type);
	fullPath.append("/");
	fullPath.append(fileName);
	fullPath.append(".dat");
	saveData<float>(fullPath.data(), data, 204*204);
	cout<<"The "<<frameCount<<" frame successful saved in "<<fullPath.data()<<endl;
}

/**
 * Load data with the index of the data file
 */
void loadNormalDataFromFile(const char *type,int frameCount, float *data){
	//string path("data/2/distance/");
	string fullPath;
	string fileName;
	if(frameCount<10){
		fileName.append(3,'0');
		fileName.append(1,char(frameCount+48));
	} else if(frameCount>=10 && frameCount<100){
		fileName.append(2,'0');
		fileName.append(1,char((frameCount/10)+48));
		fileName.append(1,char((frameCount%10)+48));
	} else if(frameCount>=100 && frameCount<1000){
		fileName.append(1,'0');
		fileName.append(1,char((frameCount/100)+48));
		fileName.append(1,char(((frameCount%100)/10)+48));
		fileName.append(1,char((frameCount%10)+48));
	} else {
		cout<<"the file name is bigger than 1000"<<endl;
		exit(0);
	}

	fullPath.append(defaultLoadPath);
	fullPath.append(type);
	fullPath.append("/");
	fullPath.append(fileName);
	fullPath.append(".dat");

	const char *ch = fullPath.data();
	cout<<"Path: "<<ch<<endl;
	if(!loadData<float>(ch, data, 204*204)){
		cout<<"data "<<frameCount<<" load error!"<<endl;
		//exit(0);
	} else {
		cout<<"data "<<frameCount<<" successful load!"<<endl;
	}
}

