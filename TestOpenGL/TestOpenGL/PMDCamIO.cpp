#include "stdafx.h"
#include "PMDCamIO.hpp"

/* Some arbitrary values for the integration times and the
   amplitude threshold. These may not be practical for all
   situations.
*/
using namespace std;

//the path of the saved Data
const char *distanceSavePath = "data/distance/";
const char *amplitudeSavePath = "data/amplitude/";
//the pointer of the saved Data Array
float *ptrPMDData;
// Result code for PMDSDK2 calls 
int res;
PMDHandle hnd;

int frameCount = 0;

bool isDataSaved = false;


BOOL createPMDCon(){
	res = pmdOpen (&hnd, "plugin\\camcube0", "", "plugin\\camcubeproc0", "");

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
	//createDefaultPMDDataDirectory();

	return TRUE;
}


void closePMDCon()
{
	pmdClose(hnd);
}

/* 
	We put error handling in an extra function to keep
   the interesting code short
*/
void checkError (PMDHandle hnd, int code)
{
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

/* Take two pictures and use the better pixels from both */
void hdrImage ()
{
  int res;

  
  float amplitude[2][204*204];
  float distance[2][204*204];
  float intensity[204*204];

  /* Take a picture with the short integration time */
  res = pmdSetIntegrationTime (hnd, 0, SHORT_TIME);
  checkError (hnd, res);

  res = pmdUpdate (hnd);
  checkError (hnd, res);

  /* space of 204*204 float */
  res = pmdGetAmplitudes (hnd, amplitude[0],
                          sizeof (float) * 204 * 204);
  checkError (hnd, res);

  res = pmdGetDistances (hnd, distance[0],
                         sizeof (float) * 204 * 204);
  checkError (hnd, res);

  /* Take a picture with the long integration time */
  res = pmdSetIntegrationTime (hnd, 0, LONG_TIME);
  checkError (hnd, res);

  res = pmdUpdate (hnd);
  checkError (hnd, res);

  res = pmdGetAmplitudes (hnd, amplitude[1],
                          sizeof (float) * 204 * 204);
  checkError (hnd, res);

  res = pmdGetDistances (hnd, distance[1],
                              sizeof (float) * 204 * 204);
  checkError (hnd, res);


  res = pmdGetIntensities (hnd, intensity, sizeof (float) * 204 * 204);

  checkError (hnd, res);

  /* Check every pixel: If the amplitude is too low, use
     the long measurement, otherwise keep the short one */
  for (unsigned i = 0; i < 204 * 204; ++i)
    {
      if (amplitude[0][i] < AMPL_THRESHOLD)
        {
          amplitude[0][i] = amplitude[1][i];
          distance[0][i] = distance[1][i];
		  //float temp = distance[0][i] + distance[1][i];
		  //distance[0][i] = temp/2;
        }
    }

  /* amplitude[0] and distance[0] now contain the merged
     data */
  //saveNormalDataToFile("distance", frameCount, distance[0]);
  //saveNormalDataToFile("amplitude", frameCount, amplitude[0]);
  //saveNormalDataToFile("intensity", frameCount, intensity);
  frameCount++;
  ptrPMDData = distance[0];
  printf ("Selected distance from the middle: %f m\n",
          distance[0][204 * 102 + 102]);
}


void getPMDData(float *disData, float *intData, float *ampData){
	int res;

	// array to save the data of long integration time
	float ampDataL[204*204];
	float disDataL[204*204];
	float intDataL[204*204];

	/* Take a picture with the short integration time */
	res = pmdSetIntegrationTime (hnd, 0, LONG_TIME);
	checkError (hnd, res);

	res = pmdUpdate (hnd);
	checkError (hnd, res);

	/* space of 204*204 float */
	res = pmdGetAmplitudes (hnd, ampData,
							  sizeof (float) * 204 * 204);
	checkError (hnd, res);

	res = pmdGetDistances (hnd, disData,
							 sizeof (float) * 204 * 204);
	checkError (hnd, res);

	res = pmdGetIntensities(hnd, intData,
							 sizeof (float) * 204 * 204);
	checkError (hnd, res);

	
	/* Take a picture with the long integration time */
	//res = pmdSetIntegrationTime (hnd, 0, LONG_TIME);
	//checkError (hnd, res);

	//res = pmdUpdate (hnd);
	//checkError (hnd, res);

	//res = pmdGetAmplitudes (hnd, ampDataL,
	//						  sizeof (float) * 204 * 204);
	//checkError (hnd, res);

	//res = pmdGetDistances (hnd, disDataL,
	//							  sizeof (float) * 204 * 204);
	//checkError (hnd, res);

	//res = pmdGetIntensities (hnd, intDataL, 
	//							  sizeof (float) * 204 * 204);
	//checkError (hnd, res);

	/* 
	 * Check every pixel: If the amplitude is too low, use
	 * the long measurement, otherwise keep the short one 
	 */
	//for (unsigned i = 0; i < 204 * 204; ++i)
	//{
	//  if (ampData[i] < AMPL_THRESHOLD)
	//	{
	//	  ampData[i] = ampDataL[i];
	//	  disData[i] = disDataL[i];
	//	  intData[i] = intDataL[i];
	//	  //float temp = distance[0][i] + distance[1][i];
	//	  //distance[0][i] = temp/2;
	//	}
	//}

	if(isDataSaved){
		saveNormalDataToFile("distance", frameCount, disData);
		saveNormalDataToFile("amplitude", frameCount, ampData);
		saveNormalDataToFile("intensity", frameCount, intData);
	}
	frameCount++;
}

void setIsDataSaved(bool isSaved){
	isDataSaved = isSaved;
}
	
float* getPMDDataPointer(){
	return ptrPMDData;
}

