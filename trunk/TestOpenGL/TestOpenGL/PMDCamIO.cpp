#include "stdafx.h"
#include "PMDCamIO.hpp"

/* Some arbitrary values for the integration times and the
   amplitude threshold. These may not be practical for all
   situations.
*/

// The handle is used to identify a connection 
PMDHandle hnd;

// Result code for PMDSDK2 calls 
int res;

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

  printf ("A distance from the middle (short): %f m (a = %f)\n",
          distance[0][204 * 102 + 102], amplitude[0][204 * 102 + 102]);

  printf ("A distance from the middle (long): %f m (a = %f)\n",
          distance[1][204 * 102 + 102], amplitude[1][204 * 102 + 102]);

  /* Check every pixel: If the amplitude is too low, use
     the long measurement, otherwise keep the short one */
  //for (unsigned i = 0; i < 204 * 204; ++i)
  //  {
  //    if (amplitude[0][i] < AMPL_THRESHOLD)
  //      {
  //        amplitude[0][i] = amplitude[1][i];
  //        distance[0][i] = distance[1][i];
  //      }
  //  }

  /* amplitude[0] and distance[0] now contain the merged
     data */
  char a[]="data/data";
  saveData<float>(a, distance[0], 204*204);

  printf ("Selected distance from the middle: %f m\n",
          distance[0][204 * 102 + 102]);
}