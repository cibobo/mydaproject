#include "pmdsdk2.h"
#include "MyDataIO.hpp"

#define SHORT_TIME 200
#define LONG_TIME 4000
#define AMPL_THRESHOLD 1300.0



BOOL createPMDCon();

void closePMDCon();

void checkError (PMDHandle hnd, int code);

void hdrImage();

void getPMDData(float *disData, float *intData, float *ampData);

float* getPMDDataPointer();

void savePMDDataToFile(const char *path, float *data);