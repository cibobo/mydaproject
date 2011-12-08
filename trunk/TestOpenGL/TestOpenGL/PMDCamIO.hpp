#include "pmdsdk2.h"
#include "ImageProcess.hpp"
#include "MyDataIO.hpp"


#include <cv.h>
#include <highgui.h>
//#include "calib3d/calib3d.h"

#define SHORT_TIME 200
#define LONG_TIME 5000
#define AMPL_THRESHOLD 1300.0



BOOL createPMDCon();

void closePMDCon();

void checkError (PMDHandle hnd, int code);

void getPMDData(float *disData, float *intData, float *ampData);

void getPMDData(BildData *bildData);

//float* getPMDDataPointer();

void savePMDDataToFile(const char *path, float *data);

void setIsDataSaved(bool isSaved);

void calibration();