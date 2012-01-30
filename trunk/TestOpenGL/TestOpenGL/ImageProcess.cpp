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

Point2f point3To2(Point3f point){
	Point2f newPoint = Point2f(point.x, point.y);
	return newPoint;
}

/*********************************
 * 
 * The algorithm for the Brightness and RESPONSETHRESHOLD
 *
 ********************************/
bool brightnessControll(int vectorSize, float &contrast, int &detecParam, unsigned char *data){
	int MINFEATURECOUNT = 12;
	int MAXFEATURECOUNT = 31;

	float MINSTANDARDENERGY = 300000.0;
	float MAXSTANDARDENERGY = 450000.0;

	float MINCONTRAST = 2;
	float MAXCONTRAST = 18;

	float MINRESPONSETHRESHOLD = 40;
	float MAXRESPONSETHRESHOLD = 130;

	double energie = 0;
	//if lesser than 7 features have been found
	if(vectorSize < MINFEATURECOUNT){
		cout<<"Case 1111111111111111111111111111111"<<endl;

		//calculate the Energy of the frame
		energie = 0;
		for(int k = 0;k<204*204;k++){
			energie += data[k];
		}
		cout<<"The energy of the data is: "<<energie<<endl;
		//if(energie < 50000) break;

		//compare the energy with the standard energy
		if(energie < MINSTANDARDENERGY){
			//if the frame is too dark
			//float eFactor = energie/MINSTANDARDENERGY;
			contrast -= 0.5;
			//cout<<"Too Dark!! The energy factor is: "<<eFactor<<endl;
			cout<<"Too Dark! The current contrast is "<<contrast<<endl;
			if(contrast<MINCONTRAST) {
				cout<<"Algo fehld, break!"<<endl;
				contrast = MINCONTRAST;
				//break;
				return false;
			}
		} else if(energie > MAXSTANDARDENERGY){
			//if the frame is too bright
			//float eFactor = energie/MAXSTANDARDENERGY;
			//contrast *= eFactor;
			//cout<<"Too Bright!! The energy factor is: "<<eFactor<<endl;
			
			contrast += 0.5;
			cout<<"Too Bright! The current contrast is "<<contrast<<endl;
			if(contrast>MAXCONTRAST) {
				cout<<"Algo fehld, break!"<<endl;
				contrast = MAXCONTRAST;
				//break;
				return false;
			}
		} else {
			//the energy is acceptable, but still can not find enough features. So change the parameter of STAR Detector
			detecParam -= 5;
			cout<<"Brightness is OK!! The current detecParam is "<<detecParam<<endl;
			if(detecParam < MINRESPONSETHRESHOLD){
				//break;
				return false;
			}
		}						
	} else if(vectorSize > MAXFEATURECOUNT){
		cout<<"Case 222222222222222222222222222"<<endl;
		if(detecParam > MAXRESPONSETHRESHOLD){
			//if the detecParam is too big
			//break;
			return false;
		} else {
			//else increase the detecParam
			detecParam += 4;
		}
		cout<<"Too Many Features!! The current detecParam is "<<detecParam<<endl;
	} else {
		cout<<"Case 333333333333333333333333333"<<endl;
		//break;
		return false;
	}
	return true;
}

/**************************************************
 *
 * A new Algorithm to make sure, that every marker will be as a just one KeyPoint recognized
 *
 * 1. Spread the features to different set, which are close to each others. The parameter eps set the size of the area
 * 2. Calculate the middel point of each set and show them
 *
 *************************************************/
void calibration(vector<vector<Point3f>> &result, vector<Point3f> points, float eps){
	vector<int> pointer;
	// the beginning set Nr. is -1 
	for(int i=0;i<points.size();i++){
		pointer.push_back(-1);
	}
	// the loop for all points
	for(int i=0;i<points.size();i++){
		vector<Point3f> temp;
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


void findMaxPointsSet(vector<vector<Point3f>> pointsSets, vector<Point3f> &maxSet){
	int maxSize = 0;
	int maxSizeIndex = 0;
	for(int i=0;i<pointsSets.size();i++){
		if(pointsSets[i].size()>maxSize){
			maxSize = pointsSets[i].size();
			maxSizeIndex = i;
		}
	}
	if(maxSize > 0){
		maxSet = pointsSets[maxSizeIndex];
	}
}

/*********************************************************
 *
 * The algorithm to tacking the markers in different frames
 *
 ********************************************************/
void featureAssociate(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, vector<int> &findIndexOld, vector<int> &findIndexNew){
	// set the number of the old features as the row size
	int rowSize = oldFeature.size();
	// set the number of the new features as the column size
	int colSize = newFeature.size();
	

	Mat tempG = Mat(rowSize, colSize, CV_32FC1);
	for(int i=0;i<rowSize;i++){
		for(int j=0;j<colSize;j++){
			float rSquare = (oldFeature[i].x-newFeature[j].x)*(oldFeature[i].x-newFeature[j].x) +
						    (oldFeature[i].y-newFeature[j].y)*(oldFeature[i].y-newFeature[j].y);	
			tempG.at<float>(i,j) = exp(-rSquare/(2*sigma*sigma));
		}
	}

	Mat G;
	Mat E;

	// compare the row size and column size, the row size should bigger than the column size
	if(rowSize <= colSize){
		G = Mat(tempG);
		E = Mat::eye(rowSize, colSize, CV_32FC1);
	} else {
		// if the column size is bigger, than set G as the transpose of temp
		G = Mat(tempG.t());
		E = Mat::eye(colSize, rowSize, CV_32FC1);
	}

	// Using the Singular Value Decomposition, using the full_uv molde to get the full-size square orthogonal matrices T and U
	SVD svd = SVD(G, SVD::FULL_UV);
	// calculate the new Matrix P
	Mat P = svd.u * E * svd.vt;

	int m = P.size().height;
	int n = P.size().width;
	int *rowMax = new int[m];
	// find the maximal element for each row, and save the column index into array rowMax
	for(int i=0;i<m;i++){
		float maxValue = P.at<float>(i,0);
		rowMax[i] = 0;
		for(int j=1;j<n;j++){
			if(P.at<float>(i,j)>maxValue){
				maxValue = P.at<float>(i,j);
				rowMax[i] = j;
			}
		}
	}
	
	vector<int> resultM, resultN;
	// loop for the element, which were found as the biggest one for each row in last step
	for(int i=0;i<m;i++){
		int colIndex = rowMax[i];
		float maxValue = P.at<float>(i, colIndex);
		int j;
		for(j=0;j<m;j++){
			// if the biggest element of row is not the biggest one for the column, break
			if(P.at<float>(j,colIndex) > maxValue){
				break;
			}
		}
		// if the loop of the column is complete, a frame coorespondence for two frames are found.
		if(j==m){
			// push the result into the vectors
			resultM.push_back(i);
			resultN.push_back(colIndex);
		}
	}

	findIndexOld.clear();
	findIndexNew.clear();
	if(rowSize <= colSize){
		findIndexOld = resultM;
		findIndexNew = resultN;
	} else {
		findIndexOld = resultN;
		findIndexNew = resultM;
	}
	delete []rowMax;
}

/*
 * The Input points are 3D points, but just the information for x and y is useful to deal with the SVD
 */
void featureAssociate2(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, vector<Point3f> &findFeatureOld, vector<Point3f> &findFeatureNew){
	// set the number of the old features as the row size
	int rowSize = oldFeature.size();
	// set the number of the new features as the column size
	int colSize = newFeature.size();
	

	Mat tempG = Mat(rowSize, colSize, CV_32FC1);
	for(int i=0;i<rowSize;i++){
		for(int j=0;j<colSize;j++){
			float rSquare = (oldFeature[i].x-newFeature[j].x)*(oldFeature[i].x-newFeature[j].x) +
						    (oldFeature[i].y-newFeature[j].y)*(oldFeature[i].y-newFeature[j].y);	
			tempG.at<float>(i,j) = exp(-rSquare/(2*sigma*sigma));
		}
	}

	Mat G;
	Mat E;

	// compare the row size and column size, the row size should bigger than the column size
	if(rowSize <= colSize){
		G = Mat(tempG);
		E = Mat::eye(rowSize, colSize, CV_32FC1);
	} else {
		// if the column size is bigger, than set G as the transpose of temp
		G = Mat(tempG.t());
		E = Mat::eye(colSize, rowSize, CV_32FC1);
	}

	// Using the Singular Value Decomposition, using the full_uv molde to get the full-size square orthogonal matrices T and U
	SVD svd = SVD(G, SVD::FULL_UV);
	// calculate the new Matrix P
	Mat P = svd.u * E * svd.vt;

	int m = P.size().height;
	int n = P.size().width;
	int *rowMax = new int[m];
	// find the maximal element for each row, and save the column index into array rowMax
	for(int i=0;i<m;i++){
		float maxValue = P.at<float>(i,0);
		rowMax[i] = 0;
		for(int j=1;j<n;j++){
			if(P.at<float>(i,j)>maxValue){
				maxValue = P.at<float>(i,j);
				rowMax[i] = j;
			}
		}
	}
	
	findFeatureOld.clear();
	findFeatureNew.clear();
	// loop for the element, which were found as the biggest one for each row in last step
	for(int i=0;i<m;i++){
		int colIndex = rowMax[i];
		float maxValue = P.at<float>(i, colIndex);
		int j;
		for(j=0;j<m;j++){
			// if the biggest element of row is not the biggest one for the column, break
			if(P.at<float>(j,colIndex) > maxValue){
				break;
			}
		}
		// if the loop of the column is complete, a frame coorespondence for two frames are found.
		if(j==m){
			// push the result into the vectors
			if(rowSize <= colSize){
				findFeatureOld.push_back(oldFeature[i]);
				findFeatureNew.push_back(newFeature[colIndex]);
			} else {
				findFeatureOld.push_back(oldFeature[colIndex]);
				findFeatureNew.push_back(newFeature[i]);
			}
		}
	}	
	delete []rowMax;
}

/****************************************************************************************
 *
 * Use SVD to find the Rotation and Translate matrix
 * Arun et al. 1987
 *
 ***************************************************************************************/
void findRAndT(vector<Point3f> oldFeatures, vector<Point3f> newFeatures, Mat &R, Mat &T){
	// get the number of the points. The size of the oldFeatures must be the same as the size of the newFeatures
	int N = oldFeatures.size();
	// create two matrices with 3 chanles to save the points
	// D = RM + T * V
	Mat M = Mat(oldFeatures);
	Mat D = Mat(newFeatures);
	

	// calculate the average value of the points
	Scalar avrM = mean(M);
	Scalar avrD = mean(D);

	Mat mc = M-avrM;
	Mat dc = D-avrD;
	
	// N * 3
	mc.reshape(1);
	dc.reshape(1);

	// calculate H
	Mat H = Mat(3,3,CV_32FC1);
	for(int i=0;i<mc.rows;i++){
		Mat mci = mc(Range(i,i+1),Range(0,3));
		Mat dci = dc(Range(i,i+1),Range(0,3));
		H = H + mci.t() * dci;
	}

	// sigular value decomposition 
	SVD svd(H);

	R = svd.vt.t() * svd.u.t();
	double detR = determinant(R);
	// if the determinant of R is bigger than 1 oder smaller than -1, modified as the special case 
	if(abs(detR + 1.0) < 0.0001){
		float optPara = determinant(svd.vt*svd.u);
		Mat temp = Mat::eye(3,3,CV_32FC1);
		temp.at<float>(2,2) = optPara;
		R = svd.u * temp * svd.vt;
	}

	Mat tempD = Mat(3,1,CV_32FC1, avrD);
	Mat tempM = Mat(3,1,CV_32FC1, avrM);
	T = tempD - R * tempM;
}


/****************************************************************************************
 *
 * The ICP Algorithm
 *
 ***************************************************************************************/
