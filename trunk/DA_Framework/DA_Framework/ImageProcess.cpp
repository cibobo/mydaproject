#include "stdafx.h"
#include "ImageProcess.hpp"

void ImageProcess::filterDepthDate(float threeDData[], double sigma){
	int arraySize = 204*204;
	float depthDaten[204*204];
	for(int i=0;i<arraySize;i++){
		depthDaten[i] = threeDData[i*3+2];
	}
	Mat src = Mat(1, arraySize, CV_32FC1, depthDaten);
	Mat dst = Mat(1, arraySize, CV_32FC1);
	GaussianBlur(src, dst, Size(0,0), sigma);

	// save the depth information back to the array
	for(int i=0;i<arraySize;i++){
		threeDData[i*3+2] = dst.at<float>(0,i);
	}
}

void ImageProcess::transFloatToMat(float *src, cv::Mat &dst, float balance, float contrast){
	dst = dst.reshape(1);
	for(int i=0;i<V_BILDSIZE;i++){
		for(int j=0;j<H_BILDSIZE;j++){
			int gray = (src[i*V_BILDSIZE+j]-balance)/contrast;
			if(gray>255){
				gray = 255;
			} else if(gray <0){
				gray = 0;
			}
			dst.at<char>(i,j*3) = dst.at<char>(i,j*3+1) = dst.at<char>(i,j*3+2) = gray;
		}
	}
	dst = dst.reshape(3);
}
