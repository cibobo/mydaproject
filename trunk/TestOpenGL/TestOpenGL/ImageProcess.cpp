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

/*
 * translate the Float data to a array with the size of 3 times
 */
void transFloatTo3Char(float *src, unsigned char *dst, float balance, float contrast){
	for(int i=0;i<cRow*cCol;i++){
		float gray = (src[i]-balance)/contrast;
		if(gray>255){
			gray = 255;
		} else if(gray <0){
			gray = 0;
		}
		dst[3*i] = dst[3*i+1] = dst[3*i+2] = gray;
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

void calibration(vector<Point2f> points, vector<Object> &objects){
	//float eps = 20;
	//objects.clear();


	//for(int i=0;i<points.size();i++){
		
}

/**************************************************
 *
 * A new Algorithm to make sure, that every marker will be as a just one KeyPoint recognized
 *
 * 1. Spread the features to different set, which are close to each others. The parameter eps set the size of the area
 * 2. Calculate the middel point of each set and show them
 *
 *************************************************/
void calibration(vector<vector<Point2f>> &result, vector<Point2f> points, float eps){
	vector<int> pointer;
	// the beginning set Nr. is -1 
	for(int i=0;i<points.size();i++){
		pointer.push_back(-1);
	}
	// the loop for all points
	for(int i=0;i<points.size();i++){
		vector<Point2f> temp;
		int index;
		// if the set Nr. for the current feature is not be set
		if(pointer.at(i) == -1){
			// create a new set, and add the current feature to it
			temp.push_back(points[i]);
			// get the set Nr.
			index = result.size();
			for(int j=0;j<points.size();j++){
				//calculate the distance between two features
				float xDis = fabs(points[i].x - points[j].x);
				float yDis = fabs(points[i].y - points[j].y);
				//if they are too close
				if((xDis*xDis + yDis*yDis)<eps*eps){
					// if the feature j is not include in this set
					if(pointer.at(j) != index){
						// add the feature j into the set of close features for feature i 
						temp.push_back(points[j]);
						// set the set Nr. for feature j
						pointer.at(j) = index;
					}
				}
			}
			// add the new set to the groupFeatures
			result.push_back(temp);
		// if the set Nr. for the current feature has been already set 
		} else {
			// get the set Nr.
			index = pointer.at(i);
			// get the set
			temp = result.at(index);
			for(int j=0;j<points.size();j++){
				//calculate the distance between two features
				float xDis = fabs(points[i].x - points[j].x);
				float yDis = fabs(points[i].y - points[j].y);
				//if they are too close
				//if(xDis<eps && yDis<eps){
				if((xDis*xDis + yDis*yDis)<eps*eps){
					// if the feature j is not include in this set
					if(pointer.at(j) != index){
						temp.push_back(points[j]);
						pointer.at(j) = index;
					}
				}
			}
			// reset the set of groupFeatures 
			result.at(index) = temp;
		}
	}
}


/*********************************************************
 *
 * The algorithm to tacking the markers in different frames
 *
 ********************************************************/
void featureAssociate(vector<Point2f> oldFeature, vector<Point2f> newFeature, float sigma){
	//int rowSize = newFeature.size();
	//int colSize = oldFeature.size();

	//Mat G = Mat(rowSize, colSize, CV_32FC1);
	//for(int i=0;i<rowSize;i++){
	//	for(int j=0;j<colSize;j++){
	//		float r = sqrt(oldFeature[i]

	//	}
	//}
	//if(oldSize > newSize){
	//	matSize = Size(oldSize, newSize);
	//} else {
	//	matSize = Size(newSize, oldSize);
	//}
}