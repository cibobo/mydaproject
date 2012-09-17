#include <highgui.h>

#include "BildData.hpp"
#include "ImageProcess.hpp"

class MyFeatureDetector{
public:
	MyFeatureDetector();
	~MyFeatureDetector(){};

	void usingSTAR();
	void usingSURF();

	void setDetectedData(BildData *data);
	int brightnessControllSTAR();

	void planeFeaturesCombination();
	void createPMDFeatures();


	double energie;
	Mat detectedMat;
	BildData *detectedData;
	vector<KeyPoint> keypoints;
	vector<vector<Point2f>> planeFeatureGroups;
	vector<PMDPoint> PMDFeatures;

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

	//Parameter of plane features combination
	float planeCombiEps;
	int planeCombiMinPts;
};