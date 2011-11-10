#include "stdafx.h"
#include "MarkerDetection.hpp"

using namespace cv;

void StarDetection(unsigned char *pixels){

	//CvMat *img = cvCreateMat(204,204,CV_8UC1);
	//cvSetData(img, pixels, sizeof(unsigned char)*204);

	Size size = Size(204, 204);
	Mat img = Mat(size, CV_8UC3, pixels);
}