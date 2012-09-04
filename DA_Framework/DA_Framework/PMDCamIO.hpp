#include "pmdsdk2.h"
#include "ImageProcess.hpp"
#include "MyDataIO.hpp"




class PMDCamIO{
public:
	PMDCamIO();
	~PMDCamIO();

	BOOL createPMDCon();
	void closePMDCon();

	void checkError(int code);
	void getPMDData(BildData *bildData);
	void setIsDataSaved(bool isSaved);

	//Handle fo the PMD Camera
	PMDHandle hnd;

	//Ingeration's time
	int shortTime;
	int longTime;

	//Index of the frame
	int frameCount;

	//Whether the inputs data should be saved
	BOOL isDataSaved;

	//the path of the saved Data
	char *savedPath;

};