class Parameters{
public:
	//Constructor
	Parameters();

	//Detection's parameters
	// Minimal allowed Features
	int MINFEATURECOUNT;
	// Maximal allowed Features
	int MAXFEATURECOUNT;
	// Maximal allowed execute Loops
	int MAXDETECTLOOPS;
	// Beginning Brightness	
	int balance;
	// Beginning Contrast
	float contrast;

	//Parameters of CenSurE Detector
	// See: http://opencv.willowgarage.com/documentation/cpp/feature_detection.html
	int MAXSIZE;
	int responseThreshold;
	int LINETHRESHOLDPROJECTED;
	int LINETHRESHOLDBINARIZED;
	int SUPPRESSNONMAXSIZE;
};