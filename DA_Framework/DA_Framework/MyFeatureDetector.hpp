#include <highgui.h>

#include "BildData.hpp"
#include "ImageProcess.hpp"

class MyFeatureDetector{
public:
	MyFeatureDetector();
	~MyFeatureDetector(){};

	void usingSTAR();
	void usingSURF();

	void setDetectedData(float *data);
	int brightnessControllSTAR(unsigned char *tempData);


	Mat detectedMat;
	Mat testMat;
	float *detectedData;
	vector<KeyPoint> keypoints;

	//Detection's parameters
	// Minimal allowed Features
	int MINFEATURECOUNT;
	// Maximal allowed Features
	int MAXFEATURECOUNT;
	// Maximal allowed execute Loops
	int MAXDETECTLOOPS;
	// Beginning Brightness	
	int BEGINBRIGHTNESS;
	// Beginning Contrast
	float BEGINCONTRAST;

	//Parameters of CenSurE Detector
	// See: http://opencv.willowgarage.com/documentation/cpp/feature_detection.html
	int MAXSIZE;
	int responseThreshold;
	int LINETHRESHOLDPROJECTED;
	int LINETHRESHOLDBINARIZED;
	int SUPPRESSNONMAXSIZE;

	//Parameter of Brightness Control
	int balance;
	float contrast;
	float MINSTANDARDENERGY;
	float MAXSTANDARDENERGY;
	float MINCONTRAST;
	float MAXCONTRAST;
	float MINRESPONSETHRESHOLD;
	float MAXRESPONSETHRESHOLD;
};