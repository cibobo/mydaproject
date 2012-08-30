#include <list>
#include "Learning.hpp"


class FrameIterator{
public:
	typedef list<BildData*>::iterator BufferItr;

	FrameIterator();
	~FrameIterator(){};


	//Initation of the DataBuffer
	void initDataBuffer(BildData *bildData);
	//Save new BildData into the DataBuffer
	void updateDataBuffer(BildData *bildData);
	//Get the current BildData
	BildData* getCurrentBildData();
	//Get the historical BildData
	BildData* getHistoricalBildData();
	//Frame Controller: check the quality of the frame and throw the bad frame out
	void framesControl(Learning* pLearning);

	// The Buffer for BildDatas, which is saving the newest BildData at last place and the oldest at the first place.
	list<BildData*> bildDataBuffer;
	// The difference between the index of the current frame and historical frame
	int DATABUFFERLENGTH;
	// The maximal number of allowed jumping frames 
	int MAXJUMPEDFEATURES;
	// To save the number of jumped frames
	int jumpedFeatures;
	// The current index of frame
	int frameIndex;

	//Distance threshold zu check the identity of original points and the transformed points
	float qualityCheckThreshold;
	// Threshold for the acceptable frame
	float qualityCheckMinAcceptRate;

	// Temp parameters to save the information of best frame
	float bestValue;
	Mat bestR;
	Mat bestT;
	vector<PMDPoint> bestFeatures;
};