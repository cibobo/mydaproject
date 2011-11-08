#include "stdafx.h"
#include "MarkerDetection.hpp"

void StarDetection(unsigned char *pixels){
	//CvMat img = CvMat(204, 204, CV_8UC3, pixels, NULL);
	CvMat *img = cvCreateMat(204,204,CV_8UC1);
	cvSetData(img, pixels, sizeof(unsigned char)*204);

	
}