#include "stdafx.h"
#include "ImageProcess.hpp"

/*
 * TODO
 * Deal with the Image Processing. The functions should be realize with CUDA later.
 */

int cRow = 204;
int cCol = 204;

void transFloatToChar(float *src, unsigned char *dst, float balance, float contrast){
	for(int i=0;i<cRow*cCol;i++){
		float gray = (src[i]-balance)/contrast;
		if(gray>255){
			gray = 255;
		} else if(gray <0){
			gray = 0;
		}
		dst[i] = gray;
	}
}

void transIntensityToGrayValue(float *src, unsigned char *dst){
	//default parameter for Intensity data
	float balance = 4900;
	float contrast = 38;
	transFloatToChar(src, dst,balance, contrast);
}

void transAmplitudeToGrayValue(float *src, unsigned char *dst){
	//default parameter for Amplitude data
	float balance = 500;
	float contrast = 10;
	transFloatToChar(src, dst,balance, contrast);
}

