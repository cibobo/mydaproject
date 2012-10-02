#include "stdafx.h"
#include "PMDCamIO.hpp"


PMDCamIO::PMDCamIO(){
	longTime = 5000;

	frameCount = 0;
	isDataSaved = false;
	savedPath = new char[150];
}

PMDCamIO::~PMDCamIO(){
}

BOOL PMDCamIO::createPMDCon(){
	int res = pmdOpen (&hnd, ".\\plugin\\camcube0", "", ".\\plugin\\camcubeproc0", "");

	// Check if the call succeeded 
	if (res != PMD_OK){
		char err[256];
		/* The handle parameter in pmdGetLastError is 0
			because no connection was established */
		pmdGetLastError (0, err, 256);
		fprintf (stderr, "Could not connect: %s\n", err);

		return FALSE;
	}
	//create the directory of the new data
	if(isDataSaved){
		createDefaultPMDDataDirectory(this->savedPath);
	}
	return TRUE;
}

void PMDCamIO::closePMDCon(){
	pmdClose(hnd);
}

void PMDCamIO::checkError(int code){
	/* Only do something if the result code is not PMD_OK */
	if (code != PMD_OK)
	{
		char err[256];

		/* Get an error description from the PMDSDK2 */
		pmdGetLastError (hnd, err, 256);
		/* Display the error message */
		fprintf (stderr, "Error: %s\n", err);
		/* Close the connection */
		pmdClose (hnd);
		/* For our example programs, we just quit */
		exit (1);
	}
}

void PMDCamIO::getPMDData(BildData *bildData){
	int res;

	// array to save the data of long integration time
	float ampDataL[H_BILDSIZE*V_BILDSIZE];
	float disDataL[H_BILDSIZE*V_BILDSIZE];
	float intDataL[H_BILDSIZE*V_BILDSIZE];

	/* Take a picture with the short integration time */
	res = pmdSetIntegrationTime (hnd, 0, this->longTime);
	checkError (res);

	res = pmdUpdate (hnd);
	checkError (res);

	/* space of 204*204 float */
	res = pmdGetAmplitudes (hnd, bildData->ampData,
							  sizeof (float) * H_BILDSIZE*V_BILDSIZE);
	checkError (res);

	res = pmdGetDistances (hnd, bildData->disData,
							 sizeof (float) * H_BILDSIZE*V_BILDSIZE);
	checkError (res);

	res = pmdGetIntensities(hnd, bildData->intData,
							 sizeof (float) * H_BILDSIZE*V_BILDSIZE);
	checkError (res);

	res = pmdGet3DCoordinates(hnd, bildData->threeDData,
							 sizeof (float) * H_BILDSIZE*V_BILDSIZE * 3);
	checkError (res);

	if(isDataSaved){
		saveAllDataToFile(frameCount, bildData);
	}
	frameCount++;
}

void PMDCamIO::setIsDataSaved(bool isSaved){
	isDataSaved = isSaved;
}

