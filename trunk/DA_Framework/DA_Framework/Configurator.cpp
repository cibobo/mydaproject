#include "stdafx.h"
#include "Configurator.hpp"

using namespace std;

TinyParser::TinyParser(void){};
TinyParser::~TinyParser(void){};

// Parsing configuration files for dates
void TinyParser::parseLineDate( std::string line, int* date )
{
	int index = 0;
	int old_pos = 0;
	int pos = 0;
	while ( (pos != -1) && (index < 2) )
	{
		pos = (int)line.find(".", pos+1);
		if (pos != -1) 
		{
			istringstream temp(line.substr( old_pos, pos-old_pos ));						
			temp >> date[index];
			index++;
			pos++;
			old_pos = pos;
		}
	}
	pos = (int)line.rfind(".");
	istringstream temp(line.substr( pos+1, line.size() - pos ));						
	temp >> date[index];
}

// Parsing configuration files for image dimensions 
void TinyParser::parseLineDimensions( std::string line, int* dim )
{
	int index = 0;
	int old_pos = 0;
	int pos = 0;
	while ( (pos != -1) && (index < 1) )
	{
		pos = (int)line.find(" ", pos+1);
		if (pos != -1) 
		{
			istringstream temp(line.substr( old_pos, pos-old_pos ));						
			temp >> dim[index];
			index++;
			pos++;
			old_pos = pos;
		}
	}
	pos = (int)line.rfind(" ");
	istringstream temp(line.substr( pos+1, line.size() - pos ));						
	temp >> dim[index];
}

// Parsing configuration files for ip adress 
void TinyParser::parseLineIP( std::string line, int* ip )
{
	int index = 0;
	int old_pos = 0;
	int pos = 0;
	while ( (pos != -1) && (index < 3) )
	{
		pos = (int)line.find(".", pos+1);
		if (pos != -1) 
		{
			istringstream temp(line.substr( old_pos, pos-old_pos ));						
			temp >> ip[index];
			index++;
			pos++;
			old_pos = pos;
		}
	}
	pos = (int)line.rfind(".");
	istringstream temp(line.substr( pos+1, line.size() - pos ));						
	temp >> ip[index];
}

// Parsing configuration files for integer value 
void TinyParser::parseLineInteger( std::string line, int* val )
{
	istringstream temp(line);						
	temp >> val[0];
}
// Parsing configuration files for float value 
void TinyParser::parseLineFloat( std::string line, float* val )
{
	istringstream temp(line);						
	temp >> val[0];
}
// Parsing configuration files for double value 
void TinyParser::parseLineDouble( std::string line, double* val )
{
	istringstream temp(line);						
	temp >> val[0];
}


//MEMORY LEAK
// Parsing configuration files for integer value 
int TinyParser::parseLineInteger( std::string line )
{
	istringstream temp(line);						
	int i;
	temp >> i;
	return i;
}
// Parsing configuration files for float value 
float TinyParser::parseLineFloat( std::string line )
{
	istringstream temp(line);						
	float f;
	temp >> f;
	return f;
	//temp >> val[0];
}
// Parsing configuration files for double value 
double TinyParser::parseLineDouble( std::string line)
{
	istringstream temp(line);						
	double d;
	temp >> d;
	return d;
}


Configurator::Configurator(MainThread *p){
	strConfigureFile = "./conf/Configuration.txt";
	strParserConfiguration = "iiiifiiiiifffffffibfiffbbfiffiibbbsbfffbbsbsibbbffibbs";

	this->pMainThread = p;
}

Configurator::~Configurator(){
}

int Configurator::readConfigurationFile(){
	// Prepare parser.
	string line;
	int index = 0;

	int i;
	float f;
	double d;
	bool b;
	string s;

	// Read configuration from text file
	string comment("#");

	std::ifstream file;  
	file.open( strConfigureFile.c_str() );

	// Line by line parsing 
	TinyParser tp;
	while (file && (index < (int)strParserConfiguration.length()))  
	{
		string line;  
		try{
			getline(file, line);
		} catch (ios_base::failure e){
			return 0;
		}

		// Check whether line is a comment line or not.
		// Comment lines have a "#" at position 0.
		if ( line.find( comment ) != 0 )
		{	
			// STEP 1
			// Parse file corresponding to parser configuration.
			switch (strParserConfiguration.at(index))
			{
			case 'i':
				tp.parseLineInteger( line, &i );
				break;
			case 'f':
				tp.parseLineFloat( line, &f );
				break;
			case 'd':
				tp.parseLineDouble( line, &d );
				break;
			case 'b':
				tp.parseLineInteger( line, &i );
				b = (bool) i;
				break;
			case 's':
				s = line;
				break;
			default:
				// ERROR!!!
				break;
			}

			vector<string> namesVec;

			// STEP 2
			// Map parsed line to corresponding attribute.
			switch(index){
				//Feture Detector
			case 0: this->pMainThread->pDetector->MAXFEATURECOUNT = i;
					break;
			case 1: this->pMainThread->pDetector->MINFEATURECOUNT = i;
					break;
			case 2: this->pMainThread->pDetector->MAXDETECTLOOPS = i;
					break;
			case 3: this->pMainThread->pDetector->BEGINBRIGHTNESS = i;
					break;
			case 4: this->pMainThread->pDetector->BEGINCONTRAST = f;
					break;
				//CenSureE
			case 5: this->pMainThread->pDetector->MAXSIZE = i;
					break;
			case 6: this->pMainThread->pDetector->responseThreshold = i;
					break;
			case 7: this->pMainThread->pDetector->LINETHRESHOLDPROJECTED = i;
					break;
			case 8: this->pMainThread->pDetector->LINETHRESHOLDBINARIZED = i;
					break;
			case 9: this->pMainThread->pDetector->SUPPRESSNONMAXSIZE = i;
					break;
				//Brightness Controll
			case 10: this->pMainThread->pDetector->MINSTANDARDENERGY = f;
					break;
			case 11: this->pMainThread->pDetector->MAXSTANDARDENERGY = f;
					break;
			case 12: this->pMainThread->pDetector->MINCONTRAST = f;
					break;
			case 13: this->pMainThread->pDetector->MAXCONTRAST = f;
					break;
			case 14: this->pMainThread->pDetector->MINRESPONSETHRESHOLD = f;
					break;
			case 15: this->pMainThread->pDetector->MAXRESPONSETHRESHOLD = f;
					break;
				//Plane Features Conbiantion
			case 16: this->pMainThread->pDetector->planeCombiEps = f;
					break;
			case 17: this->pMainThread->pDetector->planeCombiMinPts = i;
					break;
				//Learning
			case 18: this->pMainThread->isLearning = b;
					 //this->pMainThread->isRecognise = (!b);
					break;
				//Spatial Features Segmentation
			case 19: this->pMainThread->pLearning->spatialCombiEps = f;
					break;
			case 20: this->pMainThread->pLearning->spatialCombiMinPts = i;
					break;
				//Features Association
			case 21: this->pMainThread->pLearning->associateVariance = f;
					break;
			case 22: this->pMainThread->pLearning->associateRate = f;
					break;
				//Kalman Filter
			case 23: this->pMainThread->pLearning->isTKFilter = b;
					break;
			case 24: this->pMainThread->pLearning->isQKFilter = b;
					break;
				//Graph Update
			case 25: this->pMainThread->pLearning->updateDThreshold = f;
					break;
			case 26: this->pMainThread->pLearning->updateTThreshold = i;
					break;				
				//Frame Control
			case 27: this->pMainThread->pIterator->qualityCheckThreshold = f;
					break;
			case 28: this->pMainThread->pIterator->qualityCheckMinAcceptRate = f;
					break;
			case 29: this->pMainThread->pIterator->DATABUFFERLENGTH = i;
					break;
			case 30: this->pMainThread->pIterator->MAXJUMPEDFEATURES = i;
					break;
				//Recognition
			case 31: this->pMainThread->isRecognise = b;
					 //this->pMainThread->isLearning = (!b);
					break;
				 //Recognition's Model
			case 32: this->pMainThread->pRecognition->isTest = b;
					break;
			case 33: this->pMainThread->pRecognition->isLoad = b;
					break;
			case 34: this->modelNames = s;
					break;
			case 35: this->pMainThread->pRecognition->isCreate = b;
					 //this->pMainThread->isLearning = b;
					 //this->pMainThread->isRecognise = (!b);
					break;
				//Graph Isomorphismus
			case 36: this->pMainThread->pRecognition->distanceProportion = f;
					break;
			case 37: this->pMainThread->pRecognition->nodesCountProportion = f;
					break;
			case 38: this->pMainThread->pRecognition->distanceThreshold = f;
					break;
				//Input Mode
			case 39: this->pMainThread->isOnline = b;
					break;
			case 40: this->pMainThread->pPMDCamIO->isDataSaved = b;
					break;
			case 41: strcpy(this->pMainThread->pPMDCamIO->savedPath, s.data());
					break;
			case 42: this->pMainThread->isOffline = b;
					break;
			case 43: strcpy(this->pMainThread->INPUTPATH, s.data()); //here should use the strcpy, but not direct using the pointer equal
					break;
			case 44: this->pMainThread->FRAMERATE = i;
					break;
				//Visualisation
			case 45: this->pMainThread->isObservingWindowVisible = b;
					break;
			case 46: this->pMainThread->isResultWindowVisible = b;
					break;
			case 47: this->pMainThread->isOpenCVWindowVisible = b;
					break;
				//Distance Filter
			case 48: this->pMainThread->pDFilter->eps = f;
					break;
			case 49: this->pMainThread->pDFilter->diffRate = f;
					break;
			case 50: this->pMainThread->pDFilter->creatingFrames = i;
					break;
				//Tracking result
			case 51: this->pMainThread->isResultSaved = b;
					break;
			case 52: this->pMainThread->resultSavePath = "RecognitionResult/";
					break;
			case 53: //strcpy(this->pMainThread->resultSavePath, s.data());
					break;
			default:
					break;
			}
			index++;
		}
	}
	return 1;
}