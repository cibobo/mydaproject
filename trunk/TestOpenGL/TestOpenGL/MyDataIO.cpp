#include "stdafx.h"
#include "MyDataIO.hpp"

using namespace std;

//the path of the saved Data
//const char *defaultSavePath = "data/translate/";

//default load path
const char *defaultRootPath = "data/";
string defaultLoadPath;
string defaultSavePath;

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

void createDefaultPMDDataDirectory(char *subPath){
	defaultSavePath.append(defaultRootPath);
	defaultSavePath.append(subPath);
	defaultSavePath.append("/");
	const char* ptrSavePath = defaultSavePath.data();
	//check the root path of the new data
	if(isDirectoryExist(ptrSavePath)){
		cout<<"The root directory: "<<defaultSavePath<<" has been already exist!"<<endl;
		exit(3);
	} else {
		CreateDirectory(ptrSavePath, NULL);
		
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

		string threeDPath;
		threeDPath.append(defaultSavePath);
		threeDPath.append("3D");
		CreateDirectory(threeDPath.data(), NULL);
	}
}


void saveAllDataToFile(int frameCount, BildData *bildData){
	string prefix;
	string suffix;
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
	prefix.append(defaultSavePath);
	
	suffix.append("/");
	suffix.append(fileName);
	suffix.append(".dat");

	//create the path of distance data
	string disPath(prefix);
	disPath.append("distance");
	disPath.append(suffix);
	
	saveData<float>(disPath.data(), bildData->disData, 204*204);
	cout<<"The "<<frameCount<<" frame successful saved in "<<disPath.data()<<endl;

	//create the path of intensity data
	string intPath(prefix);
	intPath.append("intensity");
	intPath.append(suffix);
	
	saveData<float>(intPath.data(), bildData->intData, 204*204);
	cout<<"The "<<frameCount<<" frame successful saved in "<<intPath.data()<<endl;

	//create the path of amplitude data
	string ampPath(prefix);
	ampPath.append("amplitude");
	ampPath.append(suffix);
	
	saveData<float>(ampPath.data(), bildData->ampData, 204*204);
	cout<<"The "<<frameCount<<" frame successful saved in "<<ampPath.data()<<endl;

	//create the path of 3 dimensional data
	string threeDPath(prefix);
	threeDPath.append("3D");
	threeDPath.append(suffix);
	
	saveData<float>(threeDPath.data(), bildData->threeDData, 204*204*3);
	cout<<"The "<<frameCount<<" frame successful saved in "<<threeDPath.data()<<endl;
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

void loadAllDataFromFile(int frameCount, BildData *bildData){
	//string path("data/2/distance/");
	string prefix;
	string suffix;
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
	prefix.append(defaultLoadPath);
	
	suffix.append("/");
	suffix.append(fileName);
	suffix.append(".dat");

	string disPath(prefix);
	disPath.append("distance");
	disPath.append(suffix);
	
	//cout<<"Path: "<<ch<<endl;
	if(!loadData<float>(disPath.data(), bildData->disData, 204*204)){
		cout<<"Distance Data "<<frameCount<<" load error!"<<endl;
		//exit(0);
	} else {
		//cout<<"Distance Data "<<frameCount<<" successful load!"<<endl;
	}

	string intPath(prefix);
	intPath.append("intensity");
	intPath.append(suffix);
	
	//cout<<"Path: "<<ch<<endl;
	if(!loadData<float>(intPath.data(), bildData->intData, 204*204)){
		cout<<"Intensity Data "<<frameCount<<" load error!"<<endl;
		//exit(0);
	} else {
		//cout<<"Intensity Data "<<frameCount<<" successful load!"<<endl;
	}

	string ampPath(prefix);
	ampPath.append("amplitude");
	ampPath.append(suffix);
	
	//cout<<"Path: "<<ch<<endl;
	if(!loadData<float>(ampPath.data(), bildData->ampData, 204*204)){
		cout<<"Amplitude Data "<<frameCount<<" load error!"<<endl;
		//exit(0);
	} else {
		//cout<<"Amplitude Data "<<frameCount<<" successful load!"<<endl;
	}

	//string threeDPath(prefix);
	//threeDPath.append("3D");
	//threeDPath.append(suffix);
	//
	////cout<<"Path: "<<ch<<endl;
	//if(!loadData<float>(threeDPath.data(), bildData->threeDData, 204*204*3)){
	//	cout<<"3D Data "<<frameCount<<" load error!"<<endl;
	//	//exit(0);
	//} else {
	//	//cout<<"3D Data "<<frameCount<<" successful load!"<<endl;
	//}
}

/**
 * Load data with the index of the data file with the data type
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
	//cout<<"Path: "<<ch<<endl;
	if(!loadData<float>(ch, data, 204*204)){
		cout<<"data "<<frameCount<<" load error!"<<endl;
		//exit(0);
	} else {
		//cout<<"data "<<frameCount<<" successful load!"<<endl;
	}
}

void setDefaultLoadPath(const char* subPath){
	//string loadPath;
	defaultLoadPath.append(defaultRootPath);
	defaultLoadPath.append(subPath);
	defaultLoadPath.append("/");
	//defaultLoadPath = (char*)loadPath.data();
	//cout<<"load path "<<defaultLoadPath<<endl;
}

unsigned char * loadNormalDataForAR(int frameCount){
	float *temp = new float[204*204];
	setDefaultLoadPath("5");
	loadNormalDataFromFile("intensity", frameCount, temp);
	int balance = 4900;
	int contrast = 35;
	unsigned char data[166464];
	for(int i=0;i<204*204;i++){
		float gray = (temp[i]-balance)/contrast;
		if(gray>100){
			gray = 255;
		/*} else if(gray <129){
			gray = 0;*/
		} else gray = 0;
	
		data[4*i] = data[4*i+1] = data[4*i+2] = int(gray);
		data[4*i+3] = 0;
	}
	delete []temp;
	return data;
}

