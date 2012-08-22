#include "BildData.hpp"
#include "ImageProcess.hpp"

class Learning{
public:
	Learning();
	~Learning(){};

	//Get the trainings data of detected features
	void setTrainingFeatures(vector<PMDPoint> features);
	//Get the current bild data
	void setCurrentBildData(BildData *curBildData);
	//Get the historical bild data
	void setHistoricalBildData(BildData *hisBildData);

	//Helping function to find the maximal set
	void findMaxPMDPointSet(vector<vector<PMDPoint>> features, vector<PMDPoint> &maxSet);
	//Segementation of the necessary features from the training data
	void findObjectFeatures();
	//To find the association between the current bild and historical bild
	void findAssociations();

	//Vector to save the training data
	vector<PMDPoint> trainingFeatures;
	//Current bild data
	BildData *curBildData;
	//Historical bild data
	BildData *hisBildData;
	//Current associated result
	vector<PMDPoint> curAssPoints;
	//Historical associated result
	vector<PMDPoint> hisAssPoints;

	//Parameter for the spatial DBSCAN, to segement the features for the learning
	float spatialCombiEps;
	int spatialCombiMinPts;

	//Parameter for the Association
	//Association Variance
	float associateVariance;
	//Association Rate
	float associateRate;
};

