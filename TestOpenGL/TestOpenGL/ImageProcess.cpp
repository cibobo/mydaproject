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

float getEuclideanDis(Point3f p1, Point3f p2){
	return (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) + (p1.z-p2.z)*(p1.z-p2.z);
}


/****************************************************
 *
 * Gaussian Filter
 *
 * Using 1 dimentional Gaussian Filter to filter the depth daten of the 3D Daten
 *
 ***************************************************/
void filterDepthDate(float threeDData[], double sigma){
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

/****************************************************************************************
 *
 * Kalman Fileter
 *
 ***************************************************************************************/
// (x,y,z,Vx,Vy,Vz)
KalmanFilter kFilter(6,3,0);

// (q0, q1, q2, q3)
KalmanFilter qKFilter(3, 3, 0);

void initKalmanFilter(){
	//kFilter.statePre.at<float>(0) = 0;
	//kFilter.statePre.at<float>(1) = 0;
	//kFilter.statePre.at<float>(2) = 0;
	//kFilter.statePre.at<float>(3) = 0;
	//kFilter.statePre.at<float>(4) = 0;
	//kFilter.statePre.at<float>(5) = 0;
	kFilter.statePre = Mat::zeros(6,1,CV_32FC1);

	//setIdentity(kFilter.transitionMatrix);
	kFilter.transitionMatrix = *(Mat_<float>(6,6) <<1,0,0,1,0,0,  0,1,0,0,1,0,  0,0,1,0,0,1,  0,0,0,1,0,0,  0,0,0,0,1,0,  0,0,0,0,0,1);
	setIdentity(kFilter.measurementMatrix);


	setIdentity(kFilter.processNoiseCov, Scalar::all(0.1));
	setIdentity(kFilter.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(kFilter.errorCovPost, Scalar::all(0.1));
}


// init the kalman filter for Quanternion
void initQKFilter(){
	qKFilter.statePre = Mat::zeros(3,1,CV_32FC1);
	setIdentity(qKFilter.transitionMatrix);
	setIdentity(qKFilter.measurementMatrix);

	setIdentity(qKFilter.processNoiseCov, Scalar::all(0.1));
	setIdentity(qKFilter.measurementNoiseCov, Scalar::all(1e-1));
	setIdentity(qKFilter.errorCovPost, Scalar::all(0.1));
}

void updateKalmanFilter(Mat measure, Mat &estimate){
	for(int i=0;i<10;i++){
		Mat prediction = kFilter.predict();
		estimate = kFilter.correct(measure);
		//cout<<"loop: "<<i<<"  "<<estimate<<endl;
	}
}

void updateQKalmanFilter(Mat measure, Mat &estimate){
	for(int i=0;i<3;i++){
		Mat prediction = qKFilter.predict();
		estimate = qKFilter.correct(measure);
		//cout<<"loop: "<<i<<"  "<<estimate<<endl;
	}
}

/*********************************
 * 
 * The algorithm for the Brightness and RESPONSETHRESHOLD
 *
 ********************************/
//#define BRIGHT_TEST

bool brightnessControll(int vectorSize, float &contrast, int &detecParam, unsigned char *data){
	//int MINFEATURECOUNT = 22;
	//int MAXFEATURECOUNT = 45;

	int MINFEATURECOUNT = 42;
	int MAXFEATURECOUNT = 65;

	float MINSTANDARDENERGY = 300000.0;
	float MAXSTANDARDENERGY = 450000.0;

	float MINCONTRAST = 2;
	float MAXCONTRAST = 18;

	float MINRESPONSETHRESHOLD = 40;
	float MAXRESPONSETHRESHOLD = 130;

	double energie = 0;
	//if lesser than 7 features have been found
	if(vectorSize < MINFEATURECOUNT){

#ifdef BRIGHT_TEST
		cout<<"Case 1111111111111111111111111111111"<<endl;
#endif
		//calculate the Energy of the frame
		energie = 0;
		for(int k = 0;k<204*204;k++){
			energie += data[k];
		}

#ifdef BRIGHT_TEST
		cout<<"The energy of the data is: "<<energie<<endl;
#endif
		//if(energie < 50000) break;

		//compare the energy with the standard energy
		if(energie < MINSTANDARDENERGY){
			//if the frame is too dark
			//float eFactor = energie/MINSTANDARDENERGY;
			contrast -= 0.5;

#ifdef BRIGHT_TEST
			//cout<<"Too Dark!! The energy factor is: "<<eFactor<<endl;
			cout<<"Too Dark! The current contrast is "<<contrast<<endl;
#endif
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

#ifdef BRIGHT_TEST
			cout<<"Too Bright! The current contrast is "<<contrast<<endl;
#endif

			if(contrast>MAXCONTRAST) {
				cout<<"Algo fehld, break!"<<endl;
				contrast = MAXCONTRAST;
				//break;
				return false;
			}
		} else {
			//the energy is acceptable, but still can not find enough features. So change the parameter of STAR Detector
			detecParam -= 5;

#ifdef BRIGHT_TEST
			cout<<"Brightness is OK!! The current detecParam is "<<detecParam<<endl;
#endif

			if(detecParam < MINRESPONSETHRESHOLD){
				//break;
				return false;
			}
		}						
	} else if(vectorSize > MAXFEATURECOUNT){

#ifdef BRIGHT_TEST
		cout<<"Case 222222222222222222222222222"<<endl;
#endif

		if(detecParam > MAXRESPONSETHRESHOLD){
			//if the detecParam is too big
			//break;
			return false;
		} else {
			//else increase the detecParam
			detecParam += 4;
		}

#ifdef BRIGHT_TEST
		cout<<"Too Many Features!! The current detecParam is "<<detecParam<<endl;
#endif

	} else {

#ifdef BRIGHT_TEST
		cout<<"Case 333333333333333333333333333"<<endl;
#endif
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
				float zDis = fabs(points[i].z - points[j].z);
				//if they are too close
				if((xDis*xDis + yDis*yDis + zDis*zDis)<eps*eps){
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
				float zDis = fabs(points[i].z - points[j].z);
				//if they are too close
				//if(xDis<eps && yDis<eps){
				if((xDis*xDis + yDis*yDis + zDis*zDis)<eps*eps){
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



void calibration2D(vector<vector<KeyPoint>> &groupFeatures, vector<KeyPoint> features, float eps){
	// to save the set Nr. for each feature
	vector<int> pointer;
	// the beginning set Nr. is -1 
	for(int i=0;i<features.size();i++){
		pointer.push_back(-1);
	}
	for(int i=0;i<features.size();i++){
		vector<KeyPoint> temp;
		int index;
		// if the set Nr. for the current feature is not be set
		if(pointer.at(i) == -1){
			// create a new set, and add the current feature to it
			temp.push_back(features[i]);
			// get the set Nr.
			index = groupFeatures.size();
			for(int j=0;j<features.size();j++){
				//calculate the distance between two features
				float xDis = fabs(features[i].pt.x - features[j].pt.x);
				float yDis = fabs(features[i].pt.y - features[j].pt.y);
				//if they are too close
				if((xDis*xDis + yDis*yDis)<eps*eps){
					// if the feature j is not include in this set
					if(pointer.at(j) != index){
						// add the feature j into the set of close features for feature i 
						temp.push_back(features[j]);
						// set the set Nr. for feature j
						pointer.at(j) = index;
					}
				}
			}
			// add the new set to the groupFeatures
			groupFeatures.push_back(temp);
		// if the set Nr. for the current feature has been already set 
		} else {
			// get the set Nr.
			index = pointer.at(i);
			// get the set
			temp = groupFeatures.at(index);
			for(int j=0;j<features.size();j++){
				//calculate the distance between two features
				float xDis = fabs(features[i].pt.x - features[j].pt.x);
				float yDis = fabs(features[i].pt.y - features[j].pt.y);
				//if they are too close
				//if(xDis<eps && yDis<eps){
				if((xDis*xDis + yDis*yDis)<eps*eps){
					// if the feature j is not include in this set
					if(pointer.at(j) != index){
						temp.push_back(features[j]);
						pointer.at(j) = index;
					}
				}
			}
			// reset the set of groupFeatures 
			groupFeatures.at(index) = temp;
		}
	}
}
void calibration3(vector<vector<Point3f>> &result, vector<vector<Point2f>> &resultIndex, vector<Point3f> points, vector<Point2f> indexs, float eps){
	vector<int> pointer;
	// the beginning set Nr. is -1 
	for(int i=0;i<points.size();i++){
		pointer.push_back(-1);
	}
	// the loop for all points
	for(int i=0;i<points.size();i++){
		vector<Point3f> temp;
		vector<Point2f> tempIndex;
		int index;
		// if the set Nr. for the current feature is not be set
		if(pointer.at(i) == -1){
			// create a new set, and add the current feature to it
			temp.push_back(points[i]);
			tempIndex.push_back(indexs[i]);
			// get the set Nr.
			index = result.size();
			for(int j=0;j<points.size();j++){
				//calculate the distance between two features
				float xDis = fabs(points[i].x - points[j].x);
				float yDis = fabs(points[i].y - points[j].y);
				float zDis = fabs(points[i].z - points[j].z);
				//if they are too close
				if((xDis*xDis + yDis*yDis + zDis*zDis)<eps*eps){
					// if the feature j is not include in this set
					if(pointer.at(j) != index){
						// add the feature j into the set of close features for feature i 
						temp.push_back(points[j]);
						tempIndex.push_back(indexs[j]);
						// set the set Nr. for feature j
						pointer.at(j) = index;
					}
				}
			}
			// add the new set to the groupFeatures
			result.push_back(temp);
			resultIndex.push_back(tempIndex);
		// if the set Nr. for the current feature has been already set 
		} else {
			// get the set Nr.
			index = pointer.at(i);
			// get the set
			temp = result.at(index);
			tempIndex = resultIndex.at(index);
			for(int j=0;j<points.size();j++){
				//calculate the distance between two features
				float xDis = fabs(points[i].x - points[j].x);
				float yDis = fabs(points[i].y - points[j].y);
				float zDis = fabs(points[i].z - points[j].z);
				//if they are too close
				//if(xDis<eps && yDis<eps){
				if((xDis*xDis + yDis*yDis + zDis*zDis)<eps*eps){
					// if the feature j is not include in this set
					if(pointer.at(j) != index){
						temp.push_back(points[j]);
						tempIndex.push_back(indexs[j]);
						pointer.at(j) = index;
					}
				}
			}
			// reset the set of groupFeatures 
			result.at(index) = temp;
			resultIndex.at(index) = tempIndex;
		}
	}
}



void calibrationPMDPoint(vector<vector<PMDPoint>> &result, vector<PMDPoint> points, float eps){
	vector<int> pointer;
	// the beginning set Nr. is -1 
	for(int i=0;i<points.size();i++){
		pointer.push_back(-1);
	}
	// the loop for all points
	for(int i=0;i<points.size();i++){
		vector<PMDPoint> temp;
		int index;
		// if the set Nr. for the current feature is not be set
		if(pointer.at(i) == -1){
			// create a new set, and add the current feature to it
			temp.push_back(points[i]);
			// get the set Nr.
			index = result.size();
			pointer[i] = index;
			for(int j=0;j<points.size();j++){
				//calculate the distance between two features
				//float xDis = fabs(points[i].coord.x - points[j].coord.x);
				//float yDis = fabs(points[i].coord.y - points[j].coord.y);
				//float zDis = fabs(points[i].coord.z - points[j].coord.z);
				////if they are too close
				//if((xDis*xDis + yDis*yDis + zDis*zDis)<eps*eps){

				float dis = getEuclideanDis(points[i].coord, points[j].coord);
				if(dis < eps*eps){				
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
				//float xDis = fabs(points[i].coord.x - points[j].coord.x);
				//float yDis = fabs(points[i].coord.y - points[j].coord.y);
				//float zDis = fabs(points[i].coord.z - points[j].coord.z);
				////if they are too close
				////if(xDis<eps && yDis<eps){
				//if((xDis*xDis + yDis*yDis + zDis*zDis)<eps*eps){
				float dis = getEuclideanDis(points[i].coord, points[j].coord);
				if(dis < eps*eps){		
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



/**************************************************
 *
 * The Calibrations methode from the paper: 
 *   Motion Segmentation in Long Image Sequences
 *
 *************************************************/
void calibrationWithDistance(vector<PMDPoint> &oldResult, vector<PMDPoint> &newResult, float eps){
	int size = oldResult.size();
	// to save, how many point are faraway form the point i
	vector<int> factor;
	for(int i=0;i<size;i++){
		factor.push_back(0);
	}
	for(int i=0;i<size;i++){
		for(int j=i+1;j<size;j++){
			double oldDis = sqrt((oldResult[i].coord.x-oldResult[j].coord.x) + (oldResult[i].coord.y-oldResult[j].coord.y) + (oldResult[i].coord.z-oldResult[j].coord.z));
			double newDis = sqrt((newResult[i].coord.x-newResult[j].coord.x) + (newResult[i].coord.y-newResult[j].coord.y) + (newResult[i].coord.z-newResult[j].coord.z));

			if(fabs(newDis-oldDis)>eps){
				factor[i]++;
				factor[j]++;
			}
		}
	}

	//TODO: create a graph for features
	for(int i=0;i<oldResult.size();i++){
		// if the point i is faraway from the others
		if(factor[i] > size/2){
			oldResult.erase(oldResult.begin() + i);
			newResult.erase(newResult.begin() + i);
		}
	}
}

/**************************************************
 *
 * The Cluster methode with DBSCAN
 *   Refference: http://de.wikipedia.org/wiki/DBSCAN
 *   similar with my calibration's methode
 *
 *************************************************/

void getRegionQuery(set<int> &N, vector<Point3f> D, Point3f P, float eps){
	for(int i=0;i<D.size();i++){
		if(D[i]!= P && getEuclideanDis(P,D[i])<eps*eps){
			N.insert(i);
		}
	}
}

//#define USINGKNN

int getRegionQuery(Point3f p, set<int> &indices, float eps, int minPts, flann::Index *flann_index){
	vector<float> point;
	point.push_back(p.x);
	point.push_back(p.y);
	point.push_back(p.z);

	vector<float> dists;

#ifdef USINGKNN
	
	flann_index->knnSearch(point, indices, dists, minPts+1, flann::SearchParams(64));

	for(int i=0;i<dists.size();i++){
		if(dists[i]>eps || dists[i]==0){
			indices.erase(indices.begin()+i);
			dists.erase(dists.begin()+i);
			i--;
		}
	}

	return dists.size();
#else 
	vector<int> tempIndices;
	int maxSize = 32;
	flann_index->radiusSearch(point, tempIndices, dists, eps, maxSize, flann::SearchParams(64));

	indices.clear();
	for(int i=0;i<tempIndices.size();i++){
		if(tempIndices[i]>0){
			indices.insert(tempIndices[i]);
		} else {
			break;
		}
	}
	return indices.size();
#endif
}




void DBSCAN(vector<vector<Point3f>> &C, vector<Point3f> D, float eps, int minPts){
	int size = D.size();
	if(size>0){		
		Mat m_D = Mat(D);
		m_D = m_D.reshape(1);
		cv::flann::Index flann_index(m_D, cv::flann::KDTreeIndexParams(2));
		flann::SearchParams flann_par(64);
		int flann_maxSize = 32;

		// mark all points as not visited
		vector<int> isVisited;
		// marl all points as not yet member of any cluster
		vector<int> isInCluster;
		for(int i=0;i<size;i++){
			isVisited.push_back(0);
			isInCluster.push_back(0);
		}
		
		for(int i=0;i<size;i++){
			// if the Point is visited, continue
			if(isVisited[i]==1) continue;
			isVisited[i] = 1;
			// save the index of pints 
			set<int> N;
			getRegionQuery(D[i],N,eps,minPts,&flann_index);
			//getRegionQuery(N, D, D[i], eps);
			if(N.size() >= minPts){
				// begin the function of expandCluster
				// add P to cluster C
				vector<Point3f> tempC;
				tempC.push_back(D[i]);

				isInCluster[i] = 1;
				// for each point P'
				set<int>::iterator it;
				for(it=N.begin();it!=N.end();it++){
					// the index of P'
					int indexPP = *it;
					// if P' is not visited
					if(isVisited[indexPP] == 0){
						// mark P' as visited
						isVisited[indexPP] = 1;
						// get the neigbors of P'
						set<int> NN;
						getRegionQuery(D[indexPP],NN,eps,minPts,&flann_index);
						//getRegionQuery(NN, D, D[indexPP], eps);
						if(NN.size() >= minPts){
							// N joined with N'
							int oldSize = N.size();
							N.insert(NN.begin(), NN.end());
							int newSize = N.size();
							// if there are new point into N added, reset the iterator to the beginning of the set
							if(newSize > oldSize){
								it = N.begin();
							}
						}
					}
					// if P' is not yet member of any cluster
					if(isInCluster[indexPP] == 0){
						// add P' to cluster C
						tempC.push_back(D[indexPP]);
						isInCluster[indexPP] = 1;
					}
				}
				// next cluster
				C.push_back(tempC);
			}
		}
	} else {
		cout<<"The number of input point of DBSCAN is zero!"<<endl;
	}
}

void DBSCANPMDPoint(vector<vector<PMDPoint>> &C, vector<PMDPoint> D, float eps, int minPts){
	int size = D.size();
	if(size > 0){
		vector<Point3f> D3D;
		vector<Point2f> D2D;
		decPMDPointVector(D, D3D, D2D);

		Mat m_D = Mat(D3D);
		m_D = m_D.reshape(1);
		flann::Index flann_index(m_D, cv::flann::KDTreeIndexParams(2));
		flann::SearchParams flann_par(64);
		int flann_maxSize = 32;

		// mark all points as not visited
		vector<int> isVisited;
		// marl all points as not yet member of any cluster
		vector<int> isInCluster;
		for(int i=0;i<size;i++){
			isVisited.push_back(0);
			isInCluster.push_back(0);
		}
		
		for(int i=0;i<size;i++){
			// if the Point is visited, continue
			if(isVisited[i]==1) continue;
			isVisited[i] = 1;
			// save the index of pints 
			set<int> N;
			getRegionQuery(D3D[i],N,eps,minPts,&flann_index);
			if(N.size() >= minPts){
				// begin the function of expandCluster
				// add P to cluster C
				vector<PMDPoint> tempC;
				tempC.push_back(D[i]);

				isInCluster[i] = 1;
				// for each point P'
				set<int>::iterator it;
				for(it=N.begin();it!=N.end();it++){
					// the index of P'
					int indexPP = *it;
					// if P' is not visited
					if(isVisited[indexPP] == 0){
						// mark P' as visited
						isVisited[indexPP] = 1;
						// get the neigbors of P'
						set<int> NN;
						getRegionQuery(D3D[indexPP], NN, eps, minPts, &flann_index);
						if(NN.size() >= minPts){
							// N joined with N'
							int oldSize = N.size();
							N.insert(NN.begin(), NN.end());
							int newSize = N.size();
							// if there are new point into N added, reset the iterator to the beginning of the set
							if(newSize > oldSize){
								it = N.begin();
							}
						}
					}
					// if P' is not yet member of any cluster
					if(isInCluster[indexPP] == 0){
						// add P' to cluster C
						tempC.push_back(D[indexPP]);
						isInCluster[indexPP] = 1;
					}
				}
				// next cluster
				C.push_back(tempC);
			}
		}
	} else {
		cout<<"The number of input point of DBSCAN is zero!"<<endl;
	}
}

void DBSCANPMDPoint(vector<vector<PMDPoint>> &C, vector<PMDPoint> D, Point3f center, float eps, int minPts){
	int size = D.size();

	vector<Point3f> D3D;
	vector<Point2f> D2D;
	decPMDPointVector(D, D3D, D2D);

	Mat m_D = Mat(D3D);
	m_D = m_D.reshape(1);
	flann::Index flann_index(m_D, cv::flann::KDTreeIndexParams(2));
	flann::SearchParams flann_par(64);
	int flann_maxSize = 32;

	// mark all points as not visited
	vector<int> isVisited;
	// marl all points as not yet member of any cluster
	vector<int> isInCluster;
	for(int i=0;i<size;i++){
		isVisited.push_back(0);
		isInCluster.push_back(0);
	}
	
	for(int i=0;i<size;i++){
		// if the Point is visited, continue
		if(isVisited[i]==1) continue;
		isVisited[i] = 1;
		// save the index of pints 
		set<int> N;
		
		//getRegionQuery(N, D3D, D3D[i], eps);
		int sizeN; 
		if(i==0){
			// In first loop, using the center as the beginning point
			//sizeN = getRegionQuery(center,N,eps,minPts,&flann_index);
			// Using the old version of getRegionQuery
			getRegionQuery(N,D3D,center,eps);
			sizeN = N.size();
		} else {
			//sizeN = getRegionQuery(D3D[i],N,eps,minPts,&flann_index);
			getRegionQuery(N,D3D,D3D[i],eps);
			sizeN = N.size();
		}
		if(sizeN >= minPts){
			// begin the function of expandCluster
			// add P to cluster C
			vector<PMDPoint> tempC;
			tempC.push_back(D[i]);

			isInCluster[i] = 1;
			// for each point P'
			set<int>::iterator it;
			for(it=N.begin();it!=N.end();it++){
				// the index of P'
				int indexPP = *it;
				// if P' is not visited
				if(isVisited[indexPP] == 0){
					// mark P' as visited
					isVisited[indexPP] = 1;
					// get the neigbors of P'
					set<int> NN;
					//getRegionQuery(NN, D3D, D3D[indexPP], eps);
					//vector<float> point2;
					//point2.push_back(D3D[indexPP].x);
					//point2.push_back(D3D[indexPP].y);
					//point2.push_back(D3D[indexPP].z);
					//flann_index.radiusSearch(point2, NN, dists, eps, flann_maxSize, flann_par);
					//if(NN.size() >= minPts){
					//int sizeNN = getRegionQuery(D3D[indexPP], NN, eps, minPts, &flann_index);
					getRegionQuery(NN,D3D,D3D[indexPP],eps);
					int sizeNN = NN.size();
					if(sizeNN >= minPts){
						// N joined with N'
						int oldSize = N.size();
						N.insert(NN.begin(), NN.end());
						int newSize = N.size();
						// if there are new point into N added, reset the iterator to the beginning of the set
						if(newSize > oldSize){
							it = N.begin();
						}
					}
				}
				// if P' is not yet member of any cluster
				if(isInCluster[indexPP] == 0){
					// add P' to cluster C
					tempC.push_back(D[indexPP]);
					isInCluster[indexPP] = 1;
				}
			}
			// next cluster
			C.push_back(tempC);
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

void findMaxIndexesSet(vector<vector<Point2f>> indexesSets, vector<Point2f> &maxSet){
	int maxSize = 0;
	int maxSizeIndex = 0;
	for(int i=0;i<indexesSets.size();i++){
		if(indexesSets[i].size()>maxSize){
			maxSize = indexesSets[i].size();
			maxSizeIndex = i;
		}
	}
	if(maxSize > 0){
		maxSet = indexesSets[maxSizeIndex];
	}
}

void findMaxPMDPointSet(vector<vector<PMDPoint>> indexesSets, vector<PMDPoint> &maxSet){
	int maxSize = 0;
	int maxSizeIndex = 0;
	for(int i=0;i<indexesSets.size();i++){
		if(indexesSets[i].size()>maxSize){
			maxSize = indexesSets[i].size();
			maxSizeIndex = i;
		}
	}
	if(maxSize > 0){
		maxSet = indexesSets[maxSizeIndex];
	}
}

template<class T>
void findMaxSet(vector<vector<T>> sets, vector<T> &maxSet){
	int maxSize = 0;
	int maxSizeIndex = 0;
	for(int i=0;i<sets.size();i++){
		if(sets[i].size()>maxSize){
			maxSize = sets[i].size();
			maxSizeIndex = i;
		}
	}
	if(maxSize > 0){
		maxSet = sets[maxSizeIndex];
	}
}

/*********************************************************
 *
 * The algorithm to tacking the markers in different frames
 *
 ********************************************************/
void featureAssociate2(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, vector<int> &findIndexOld, vector<int> &findIndexNew){
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
bool featureAssociate(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, 
					   vector<Point3f> &findFeatureOld, vector<Point3f> &findFeatureNew,
					   vector<int> &findIndexOld, vector<int> &findIndexNew,
					   float &avrDis, float &disPE){
	// set the number of the old features as the row size
	int rowSize = oldFeature.size();
	// set the number of the new features as the column size
	int colSize = newFeature.size();
	

	Mat tempG = Mat(rowSize, colSize, CV_32FC1);
	for(int i=0;i<rowSize;i++){
		for(int j=0;j<colSize;j++){
			float rSquare = (oldFeature[i].x-newFeature[j].x)*(oldFeature[i].x-newFeature[j].x) +
							//(oldFeature[i].z-newFeature[j].z)*(oldFeature[i].z-newFeature[j].z) +
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
	cout<<"===================== THE SVD ======================="<<endl;
	//cout<<G<<endl<<endl;

	// Using the Singular Value Decomposition, using the full_uv molde to get the full-size square orthogonal matrices T and U
	SVD svd = SVD(G, SVD::FULL_UV);
	//cout<<svd.u<<endl<<endl;
	//cout<<svd.vt<<endl<<endl;
	// calculate the new Matrix P
	Mat P = svd.u * E * svd.vt;

	
	//cout<<P<<endl<<endl;

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

	findIndexOld.clear();
	findIndexNew.clear();

	disPE = 0;
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
		if(j==m && maxValue>0.71){
			disPE += fabs(1-maxValue);
			cout<<i<<" : "<<maxValue<<endl;
			// push the result into the vectors
			if(rowSize <= colSize){
				findFeatureOld.push_back(oldFeature[i]);
				findFeatureNew.push_back(newFeature[colIndex]);

				findIndexOld.push_back(i);
				findIndexNew.push_back(colIndex);
			} else {
				findFeatureOld.push_back(oldFeature[colIndex]);
				findFeatureNew.push_back(newFeature[i]);

				findIndexOld.push_back(colIndex);
				findIndexNew.push_back(i);
			}
		}
	}	

	disPE = disPE/findIndexOld.size();
	cout<<"The average distance from P to E is: "<<disPE<<endl;
	// calculate the average displacement
	float disSum = 0;
	float disMax = -1;
	for(int i=0;i<findFeatureOld.size();i++){
		float dis = sqrt((findFeatureOld[i].x - findFeatureNew[i].x)*(findFeatureOld[i].x - findFeatureNew[i].x)
					 + (findFeatureOld[i].y - findFeatureNew[i].y)*(findFeatureOld[i].y - findFeatureNew[i].y));
					 //+ (findFeatureOld[i].z - findFeatureNew[i].z)*(findFeatureOld[i].z - findFeatureNew[i].z));

		disSum += dis;
		if(dis>disMax)
			disMax = dis;
	}
	cout<<"The maximal displacement is: "<<disMax<<endl;
	avrDis = disSum/findFeatureOld.size();
	cout<<"The average displacement is: "<<avrDis<<endl;

	avrDis = disMax;
	
	delete []rowMax;
	if(_isnan(disPE) || findFeatureOld.size()<3){
		// return false if the matrix P is NaN, or the number of associate points smaller than 3
		return false;
	} else {
		return true;
	}
}

bool featureAssociatePMD(vector<PMDPoint> oldFeature, vector<PMDPoint> newFeature, float sigma, 
					   vector<PMDPoint> &findFeatureOld, vector<PMDPoint> &findFeatureNew,
					   float &avrDis, float &disPE, float &sumP){
	// set the number of the old features as the row size
	int rowSize = oldFeature.size();
	// set the number of the new features as the column size
	int colSize = newFeature.size();
	

	Mat tempG = Mat(rowSize, colSize, CV_32FC1);
	for(int i=0;i<rowSize;i++){
		for(int j=0;j<colSize;j++){
			float rSquare = (oldFeature[i].coord.x-newFeature[j].coord.x)*(oldFeature[i].coord.x-newFeature[j].coord.x) +
							//(oldFeature[i].z-newFeature[j].z)*(oldFeature[i].z-newFeature[j].z) +
						    (oldFeature[i].coord.y-newFeature[j].coord.y)*(oldFeature[i].coord.y-newFeature[j].coord.y);	
			//float rSquare = getEuclideanDis(oldFeature[i].coord, newFeature[j].coord);
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
	cout<<"===================== THE SVD ======================="<<endl;
	//cout<<G<<endl<<endl;

	// Using the Singular Value Decomposition, using the full_uv molde to get the full-size square orthogonal matrices T and U
	SVD svd = SVD(G, SVD::FULL_UV);
	//cout<<svd.u<<endl<<endl;
	//cout<<svd.vt<<endl<<endl;
	// calculate the new Matrix P
	Mat P = svd.u * E * svd.vt;

	
	//cout<<P<<endl<<endl;

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


	disPE = 0;
	sumP = 0;
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
		if(j==m && maxValue>0.71){
			disPE += fabs(1-maxValue);
			sumP += maxValue;
			cout<<i<<" : "<<maxValue<<endl;
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

	//sumP = sumP/findFeatureOld.size();
	cout<<"The sum of nonzero elements of P is: "<<sumP<<endl;
	disPE = disPE/findFeatureOld.size();
	cout<<"The average distance from P to E is: "<<disPE<<endl;
	// calculate the average displacement
	float disSum = 0;
	float disMax = -1;
	for(int i=0;i<findFeatureOld.size();i++){
		float dis = sqrt((findFeatureOld[i].coord.x - findFeatureNew[i].coord.x)*(findFeatureOld[i].coord.x - findFeatureNew[i].coord.x)
					 + (findFeatureOld[i].coord.y - findFeatureNew[i].coord.y)*(findFeatureOld[i].coord.y - findFeatureNew[i].coord.y));
					 //+ (findFeatureOld[i].z - findFeatureNew[i].z)*(findFeatureOld[i].z - findFeatureNew[i].z));

		//float dis = sqrt(getEuclideanDis(findFeatureOld[i].coord, findFeatureNew[i].coord));
		disSum += dis;
		if(dis>disMax)
			disMax = dis;
	}
	cout<<"The maximal displacement is: "<<disMax<<endl;
	avrDis = disSum/findFeatureOld.size();
	cout<<"The average displacement is: "<<avrDis<<endl;

	avrDis = disMax;
	
	delete []rowMax;
	if(_isnan(disPE) || findFeatureOld.size()<3){
		// return false if the matrix P is NaN, or the number of associate points smaller than 3
		return false;
	} else {
		//if(sumP > 0.95){
			return true;
		//} else {
		//	return false;
		//}
	}
}


void featureAssociateMat(Mat oldFeature, Mat newFeature, float sigma, vector<int> &findIndexOld, vector<int> &findIndexNew, float &dist){
	// set the number of the old features as the row size
	int rowSize = oldFeature.rows;
	// set the number of the new features as the column size
	int colSize = newFeature.rows;
	//for(int i=0;i<rowSize;i++){
	//	cout<<oldFeature.row(i)<<"   "<<newFeature.row(i)<<endl;
	//}
	//cout<<endl;
	

	Mat tempG = Mat(rowSize, colSize, CV_32FC1);
	for(int i=0;i<rowSize;i++){
		for(int j=0;j<colSize;j++){
			float rSquare = (oldFeature.at<float>(i,0)-newFeature.at<float>(j,0))*(oldFeature.at<float>(i,0)-newFeature.at<float>(j,0)) +
						    (oldFeature.at<float>(i,1)-newFeature.at<float>(j,1))*(oldFeature.at<float>(i,1)-newFeature.at<float>(j,1));

			//float rSquare = getEuclideanDis(oldFeature.at<Point3f>(i,0), newFeature.at<Point3f>(j,0));
			float value = exp(-rSquare/(2*sigma*sigma));
			tempG.at<float>(i,j) = value;
		}
	}
	//cout<<"G: "<<tempG<<endl;

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
	//cout<<"u:"<<svd.u<<endl<<endl;
	//cout<<"E: "<<E<<endl<<endl;
	//cout<<"vt: "<<svd.vt<<endl;
	//cout<<"Sigma: "<<svd.w<<endl;
	// calculate the new Matrix P
	Mat P = svd.u * E * svd.vt;
	//cout<<"P: "<<P<<endl;

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
	
	findIndexOld.clear();
	findIndexNew.clear();
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
		if(j==m && maxValue>0.71){
			cout<<i<<" : "<<maxValue<<endl;
			// push the result into the vectors
			if(rowSize <= colSize){
				findIndexOld.push_back(i);
				findIndexNew.push_back(colIndex);
			} else {
				findIndexOld.push_back(colIndex);
				findIndexNew.push_back(i);
			}
		}
	}
	delete []rowMax;

	dist = 0;
	for(int i=0;i<findIndexOld.size();i++){
		Mat oldRow = oldFeature.row(findIndexOld[i]);
		Mat newRow = newFeature.row(findIndexNew[i]);
		float qrDist = oldRow.dot(newRow);
		dist += sqrt(qrDist);
	}
}

/****************************************************************************************
 *
 * Use SVD to find the Rotation and Translate matrix
 * Arun et al. 1987
 *
 ***************************************************************************************/
void SVDFindRAndT(vector<Point3f> oldFeatures, vector<Point3f> newFeatures, Mat &R, Mat &T){
	// get the number of the points. The size of the oldFeatures must be the same as the size of the newFeatures
	int N = oldFeatures.size();

	// create two matrices with 3 chanles to save the points
	// D = RM + T * V
	Mat M = Mat(oldFeatures, true);
	Mat D = Mat(newFeatures, true);

	//cout<<"M is: "<<M<<endl;
	//cout<<"D is: "<<D<<endl;
	//cout<<"The rows of M is: "<<M.rows<<endl;
	//cout<<"The chanle of M is: "<<M.channels()<<endl;
	
	// calculate the average value of the points
	Scalar avrM = mean(M);
	Scalar avrD = mean(D);


	Mat mc = M-avrM;
	Mat dc = D-avrD;

	//cout<<"Mc is: "<<mc<<endl;
	//cout<<"Dc is: "<<dc<<endl;
	//cout<<"The colomuns of MC is: "<<mc.cols<<endl;
	
	// change the channel of the Matrix from 3 to 1
	// the result must be reset for mc and dc. The sample programm in webseit is wrong
	mc = mc.reshape(1);
	dc = dc.reshape(1);

	//cout<<"reshaped Mc is: "<<mc<<endl;
	//cout<<"The reshaped colomuns of MC is: "<<mc.cols<<endl;

	// calculate H
	Mat H = Mat::zeros(3,3,CV_32FC1);
	//cout<<"H= "<<H<<endl;

	for(int i=0;i<mc.rows;i++){
		Mat mci = mc(Range(i,i+1),Range(0,3));
		Mat dci = dc(Range(i,i+1),Range(0,3));
		H = H + mci.t() * dci;
	}

	// sigular value decomposition 
	SVD svd(H);

	R = svd.vt.t() * svd.u.t();
	double detR = determinant(R);
	//cout<<"|R|= "<<detR<<endl;

	// if the determinant of R is bigger than 1 oder smaller than -1, modified as the special case 
	if(abs(detR + 1.0) < 0.0001){
		float optPara = determinant(svd.vt*svd.u);
		Mat temp = Mat::eye(3,3,CV_32FC1);
		temp.at<float>(2,2) = optPara;
		R = svd.u * temp * svd.vt;
	}

	//cout<<"R = "<<R<<endl<<endl;
	//cout<<R.cols<<" , "<<R.rows<<" | "<<R.channels()<<" , "<<R.type()<<endl;

	Mat tempM = Mat(avrM).rowRange(0,3);
	Mat tempD = Mat(avrD).rowRange(0,3);

	// The type of avrM and avrD is CV_64FC1, so here need to call convertTo to change the type of the matrix
	tempM.convertTo(tempM, CV_32FC1);
	tempD.convertTo(tempD, CV_32FC1);

	//cout<<tempM<<endl;
	//cout<<tempM.cols<<" , "<<tempM.rows<<" | "<<tempM.channels()<<" , "<<tempM.type()<<endl;



	T = tempD - R * tempM;
	//cout<<"R*M= "<<R*tempM<<endl<<endl;
	//cout<<"T= "<<T<<endl;
	//cout<<T.cols<<" , "<<T.rows<<" | "<<T.channels()<<endl;
}


/****************************************************************************************
 *
 * Using unit quaternions to calculate the absolute orientation
 * Bertgikd K.P.Horn 1987
 *
 ***************************************************************************************/
float UQFindRAndT(vector<Point3f> oldFeatures, vector<Point3f> newFeatures, Mat &R, Mat &T){
	// create two matrices with 3 chanles to save the points
	// D = RM + T * V
	Mat M = Mat(oldFeatures, true);
	Mat D = Mat(newFeatures, true);
	float angle = m_UQFindRAndT(M, D, R, T);
	return angle;
}


//#define QKALMANFILTERON
#define TKALMANFILTERON

float m_UQFindRAndT(Mat &M, Mat &D, Mat &R, Mat &T){

	//cout<<"M is: "<<M<<endl;
	//cout<<"D is: "<<D<<endl;
	//cout<<"The rows of M is: "<<M.rows<<endl;
	//cout<<"The chanle of M is: "<<M.channels()<<endl;
	
	// calculate the average value of the points
	Scalar avrM = mean(M);
	Scalar avrD = mean(D);

	Mat mc = M-avrM;
	Mat dc = D-avrD;

	//cout<<"Mc is: "<<mc<<endl;
	//cout<<"Dc is: "<<dc<<endl;
	//cout<<"The colomuns of MC is: "<<mc.cols<<endl;
	
	// change the channel of the Matrix from 3 to 1
	// the result must be reset for mc and dc. The sample programm in webseit is wrong
	mc = mc.reshape(1);
	dc = dc.reshape(1);

	//cout<<"reshaped Mc is: "<<mc<<endl;
	//cout<<"The reshaped colomuns of MC is: "<<mc.cols<<endl;

	// calculate S
	Mat S = Mat::zeros(3,3,CV_32FC1);
	//cout<<"S= "<<S<<endl;

	for(int i=0;i<mc.rows;i++){
		Mat mci = mc(Range(i,i+1),Range(0,3));
		Mat dci = dc(Range(i,i+1),Range(0,3));
		S = S + mci.t() * dci;
	}

	// N is a symmetric matrix
	Mat N = Mat(4,4,CV_32FC1);
	
	// diagonal elements
	N.at<float>(0,0) = S.at<float>(0,0) + S.at<float>(1,1) + S.at<float>(2,2);
	N.at<float>(1,1) = S.at<float>(0,0) - S.at<float>(1,1) - S.at<float>(2,2);
	N.at<float>(2,2) =-S.at<float>(0,0) + S.at<float>(1,1) - S.at<float>(2,2);
	N.at<float>(3,3) =-S.at<float>(0,0) - S.at<float>(1,1) + S.at<float>(2,2);

	// the rest element at first row
	N.at<float>(0,1) = N.at<float>(1,0) = S.at<float>(1,2) - S.at<float>(2,1);
	N.at<float>(0,2) = N.at<float>(2,0) = S.at<float>(2,0) - S.at<float>(0,2);
	N.at<float>(0,3) = N.at<float>(3,0) = S.at<float>(0,1) - S.at<float>(1,0);

	// the rest element at second row
	N.at<float>(1,2) = N.at<float>(2,1) = S.at<float>(0,1) + S.at<float>(1,0);
	N.at<float>(1,3) = N.at<float>(3,1) = S.at<float>(2,0) + S.at<float>(0,2);

	// the rest element at third row
	N.at<float>(2,3) = N.at<float>(3,2) = S.at<float>(1,2) + S.at<float>(2,1);

	//cout<<"The matrix N is: "<<N<<endl;

	// calculate the eigenvalue and eigenvector
	Mat E = Mat(4,1,CV_32FC1);
	Mat V = Mat(4,4,CV_32FC1);
	eigen(N,E,V);

	//cout<<"The eigenvalues are: "<<E<<endl;
	//cout<<"The eigenvectors are: "<<V<<endl;

	float maxE = 0;
	int maxIndex = -1;
	for(int i=0;i<4;i++){
		if(E.at<float>(i,0) > maxE){
			maxE = E.at<float>(i,0);
			maxIndex = i;
		}
	}

	// the unit quaternion
	Mat q, qTemp;
	// if a positive eigenvalue has been found
	if(maxIndex>=0){
		q = V(Range(maxIndex, maxIndex+1),Range(0,4));
		//cout<<"max eigenvalue is: "<<maxE<<endl;
		//cout<<"q is: "<<q<<endl;
		//cout<<"Rotated: "<<acos(q.at<float>(0,0))*2*180/3.14<<endl;
	} else {
		cout<<"There are no positive eigenvalue!"<<endl;
		return 0;
	}

	cout<<q<<endl;

	float PI = 3.1416;
	float angle = acos(q.at<float>(0,0))*2*180/PI;
	cout<<"The rotation is: "<<angle<<endl;
	//cout<<"The old qunternion is: "<<q<<endl;
	// consider, that the angle Theta/2 is just between 0~pi
#ifdef QKALMANFILTERON
	float sinValue = sqrt(1-q.at<float>(0,0)*q.at<float>(0,0));
	// calculate the rotated vector
	if(sinValue != 0){
		qTemp = q.colRange(1,4)/sinValue;
	} else {
		qTemp = Mat::zeros(1,3,CV_32FC1);
	}
	cout<<"The old rotated vector is: "<<qTemp<<endl;
	
	// using kalman filter to filter this rotated vector
	Mat qTran = Mat(3,1,CV_32FC1);
	updateQKalmanFilter(qTemp.t(), qTran);
	cout<<"The new rotated vector is: "<<qTran.t()<<endl;
	q.at<float>(0,1) = qTran.at<float>(0,0)*sinValue;
	q.at<float>(0,2) = qTran.at<float>(1,0)*sinValue;
	q.at<float>(0,3) = qTran.at<float>(2,0)*sinValue;
	cout<<"The new qunternion is: "<<q<<endl;
#endif

	//float scale = sqrt(q.at<float>(0,1)*q.at<float>(0,1) + q.at<float>(0,2)*q.at<float>(0,2) + q.at<float>(0,3)*q.at<float>(0,3));
	//float rX = q.at<float>(0,1)/scale;
	//float rY = q.at<float>(0,2)/scale;
	//float rZ = q.at<float>(0,3)/scale;
	//float angle = acos(q.at<float>(0,0))*2*180/3.14;
	//cout<<"The rotation is: "<<angle<<"  on vector:["<<rX<<" , "<<rY<<" , "<<rZ<<"]"<<endl;

	//Mat I = Mat::eye(4,4,CV_32FC1);
	//cout<<"Test eigenvector:"<<(N-I*maxE)*q.t()<<endl;

	// umform to a rotation matrix
	R.at<float>(0,0) = q.at<float>(0,0)*q.at<float>(0,0) + q.at<float>(0,1)*q.at<float>(0,1) - q.at<float>(0,2)*q.at<float>(0,2) - q.at<float>(0,3)*q.at<float>(0,3);
	R.at<float>(0,1) = 2*(q.at<float>(0,1)*q.at<float>(0,2) - q.at<float>(0,0)*q.at<float>(0,3));
	R.at<float>(0,2) = 2*(q.at<float>(0,1)*q.at<float>(0,3) + q.at<float>(0,0)*q.at<float>(0,2));

	R.at<float>(1,0) = 2*(q.at<float>(0,2)*q.at<float>(0,1) + q.at<float>(0,0)*q.at<float>(0,3));
	R.at<float>(1,1) = q.at<float>(0,0)*q.at<float>(0,0) - q.at<float>(0,1)*q.at<float>(0,1) + q.at<float>(0,2)*q.at<float>(0,2) - q.at<float>(0,3)*q.at<float>(0,3);
	R.at<float>(1,2) = 2*(q.at<float>(0,2)*q.at<float>(0,3) - q.at<float>(0,0)*q.at<float>(0,1));

	R.at<float>(2,0) = 2*(q.at<float>(0,3)*q.at<float>(0,1) - q.at<float>(0,0)*q.at<float>(0,2));
	R.at<float>(2,1) = 2*(q.at<float>(0,3)*q.at<float>(0,2) + q.at<float>(0,0)*q.at<float>(0,1));
	R.at<float>(2,2) = q.at<float>(0,0)*q.at<float>(0,0) - q.at<float>(0,1)*q.at<float>(0,1) - q.at<float>(0,2)*q.at<float>(0,2) + q.at<float>(0,3)*q.at<float>(0,3);
	
	//cout<<"R = "<<R<<endl<<endl;
	//cout<<R.cols<<" , "<<R.rows<<" | "<<R.channels()<<" , "<<R.type()<<endl;
	//cout<<"Determinand of R is: "<<determinant(R)<<endl;
	
	Mat tempM = Mat(avrM).rowRange(0,3);
	Mat tempD = Mat(avrD).rowRange(0,3);

	// The type of avrM and avrD is CV_64FC1, so here need to call convertTo to change the type of the matrix
	tempM.convertTo(tempM, CV_32FC1);
	tempD.convertTo(tempD, CV_32FC1);

	//cout<<tempM<<endl;
	//cout<<tempM.cols<<" , "<<tempM.rows<<" | "<<tempM.channels()<<" , "<<tempM.type()<<endl;
	//cout<<"The Centeriol for M is: "<<tempM<<endl;
	//cout<<"The Centeriol for D is: "<<tempD<<endl;

	//kalman filter
#ifdef TKALMANFILTERON
	Mat kTempD = Mat(6,1,CV_32FC1);
	updateKalmanFilter(tempD, kTempD);

	cout<<"The Centeriol for D after Kalman Filter is: "<<kTempD<<endl;

	T = kTempD.rowRange(0,3) - R * tempM;
#else
	T = tempD - R * tempM;
#endif
	//cout<<"R*M= "<<R*tempM<<endl<<endl;
	//cout<<"T= "<<T<<endl;
	//cout<<T.cols<<" , "<<T.rows<<" | "<<T.channels()<<endl;
	return acos(q.at<float>(0,0))*2*180/3.14;
}

/***************************************************************
 *
 * Noise controll
 * Regard the linear acceleration and angular acceleration
 *
 **************************************************************/
bool isBigNoised(Mat T, float angular, int frameDiff, float eLinear, float eAngular){
	float avrAngular = angular/frameDiff;
	cout<<"The avrage angular is: "<<avrAngular<<endl;

	Mat avrT = T*(1.0/frameDiff);
	cout<<"The average translate is: "<<avrT<<endl;

	float avrSumT = (fabs(T.at<float>(0,0)) + fabs(T.at<float>(1,0)) + fabs(T.at<float>(2,0)))/frameDiff;
	cout<<"The average translate is: "<<avrSumT<<endl;

	if(avrAngular>eAngular){
		return true;
	} else {
		if(fabs(avrT.at<float>(0,0)) > eLinear || 
		   fabs(avrT.at<float>(1,0)) > eLinear || 
		   fabs(avrT.at<float>(2,0)) > eLinear){
		//if(avrSumT > eLinear){
			   return true;
		}
		return false;
	}
}

//TODO: Combination with the ObjectUpdata function
bool isBigNoised2( Graph *graph, vector<Point3f> points, Mat &R, Mat &T, float aRate, float e){
	float rate = getCorresRate(graph, points, R, T, e);
	if(rate == -1) {
		cout<<"The points or nodelist in graph is empty!"<<endl;
		return true;
	}
	if(rate > aRate){
		// if bigger than the avalible rate
		return false;
	} else {
		return true;
	}	
}

float getCorresRate(Graph *graph, vector<Point3f> points, Mat &R, Mat &T, float e){
	if(points.size()<=0){
		return -1;
	} else {
		if(graph->nodeList.size()<=0){
			return -1;
		} else {
			int coorPoints = 0;
			int size = graph->nodeList.size();
			for(int i=0;i<size;i++){
				Mat oldPoint = Mat(graph->nodeList[i]->getPoint());
				Mat newPoint = R*oldPoint + T;
				for(int j=0;j<points.size();j++){
					if(fabs(newPoint.at<float>(0,0) - points[j].x) < e &&
					   fabs(newPoint.at<float>(1,0) - points[j].y) < e &&
					   fabs(newPoint.at<float>(2,0) - points[j].z) < e){
						   // if find the coorespondence point
						   coorPoints ++;
							   break;
					} 
				}
			}
			float rate = float(coorPoints)/points.size();
			cout<<"The coorespondence rate: "<<rate<<" = "<<coorPoints<<"/"<<points.size()<<endl;
			return rate;
		}
	}
}

// To calculate the euler angle with rotationsmatrix.
// The result is saved into a vector with 3 elements
void calcEulerAngleFromR(Mat R, Vec3d &euler1, Vec3d &euler2){
	float pei = 3.1416;
	if(fabs(R.at<float>(2,0)) != 1){
		euler1[0] = -asin(R.at<float>(2,0));
		euler2[0] = pei - euler1[0];

		double cosTheta1 = cos(euler1[0]);
		double cosTheta2 = cos(euler2[0]);
		
		euler1[1] = atan2(R.at<float>(2,1)/cosTheta1, R.at<float>(2,2)/cosTheta1);
		euler2[1] = atan2(R.at<float>(2,1)/cosTheta2, R.at<float>(2,2)/cosTheta2);

		euler1[2] = atan2(R.at<float>(1,0)/cosTheta1, R.at<float>(0,0)/cosTheta1);
		euler2[2] = atan2(R.at<float>(1,0)/cosTheta2, R.at<float>(0,0)/cosTheta2);
	} else {
		euler1[2] = euler2[2] = 0;

		float R31 = -R.at<float>(2,0);
		
		euler1[0] = euler2[0] = R31*pei/2;
		euler1[1] = euler2[1]  = R31*euler1[2] + atan2(R31*R.at<float>(0,1), R31*R.at<float>(0,2));
		
		//if(R.at<float>(2,0) == -1){
		//	euler1[0] = euler2[0] = pei/2;
		//	euler1[1] = euler2[1]  = euler1[2] + atan2(R.at<float>(0,1), R.at<float>(0,2));
		//} else {
		//	euler1[0] = euler2[0] = -pei/2;
		//	euler1[1] = euler2[1]  = euler1[2] + atan2(-R.at<float>(0,1), -R.at<float>(0,2));
		//}
	}
}

/********************************************************************************
 *
 * ICP Algortihm
 *
 *******************************************************************************/
// find the nearest point of rsc from dst. The indices of the points in dst will be saved in vector pairs
float flann_knn(Mat& dst, Mat& rsc, vector<int>& pairs, vector<float>& dists = vector<float>()) {
	// find nearest neighbors using FLANN
	cv::Mat m_indices(rsc.rows, 1, CV_32S);
	cv::Mat m_dists(rsc.rows, 1, CV_32F);

	Mat dst_32f = dst.reshape(1); 
	//dst.convertTo(dst_32f,CV_32FC3);
	Mat rsc_32f = rsc.reshape(1); 
	//rsc.convertTo(rsc_32f,CV_32FC3);

	assert(dst_32f.type() == CV_32FC1);

	cv::flann::Index flann_index(dst_32f, cv::flann::KDTreeIndexParams(2));  // using 4 randomized kdtrees
    flann_index.knnSearch(rsc_32f, m_indices, m_dists, 1, cv::flann::SearchParams(64) ); // maximum number of leafs checked

    int* indices_ptr = m_indices.ptr<int>(0);
    //float* dists_ptr = m_dists.ptr<float>(0);
    for (int i=0;i<m_indices.rows;++i) {
   		pairs.push_back(indices_ptr[i]);
    }

	dists.resize(m_dists.rows);
	m_dists.copyTo(Mat(dists));

	return cv::sum(m_dists)[0];
}

void decPMDPointVector(vector<PMDPoint> pmdPoints, vector<Point3f> &points3D, vector<Point2f> &points2D){
	for(int i=0;i<pmdPoints.size();i++){
		points3D.push_back(pmdPoints[i].coord);
		points2D.push_back(pmdPoints[i].index);
	}
}

//#define ASSOCIATE

bool ICP(vector<PMDPoint> rsc, vector<PMDPoint> dst, Mat &R, Mat &T, vector<PMDPoint> &oldResult, vector<PMDPoint> &newResult){
	cout<<"============================ Beginning of the ICP ======================="<<endl;
	R = Mat::eye(3,3,CV_32FC1);
	T = Mat::zeros(3,1,CV_32FC1);
	
	oldResult.clear();
	newResult.clear();
	oldResult = rsc;

	vector<int> pairs;
	vector<float> dists;
	double minDist = numeric_limits<double>::max();
	float sigma = 0.002;

	// decomposition PMDPoint vector
	vector<Point3f> rsc3D, dst3D;
	vector<Point2f> rsc2D, dst2D;
	decPMDPointVector(rsc, rsc3D, rsc2D);
	decPMDPointVector(dst, dst3D, dst2D);

	Mat m_rsc, m_dst; 
	//if(dst.size() >= rsc.size()){
		m_rsc = Mat(rsc3D);
		m_dst = Mat(dst3D);
	//} else {
	//	m_rsc = Mat(dst3D);
	//	m_dst = Mat(rsc3D);
	//}

	int maxLoop = 30;
	int loopCount = 0;
	while(true){
		Mat tempR = Mat::eye(3,3,CV_32FC1);
		Mat tempT = Mat::zeros(3,1,CV_32FC1); 

#ifndef ASSOCIATE
		pairs.clear();
		dists.clear();

		double curDist = flann_knn(m_dst, m_rsc, pairs, dists);
	
		if(minDist <= curDist){
			// the best result is found
			break;
		}

		minDist = curDist;
		cout<<"The minimal distance of ICP is: "<<minDist<<endl;


		// set new destination points with the nearest points
		Mat tempDst(m_rsc.size(), m_rsc.type());
		for(int i=0;i<m_rsc.rows;i++){
			Point3f point = m_dst.at<Point3f>(pairs[i], 0);
			tempDst.at<Point3f>(i,0) = point;
			// copy to newResult
			//if(dst.size() >= rsc.size()){
				newResult.push_back(dst[pairs[i]]);
			//} else {
			//	newResult.push_back(rsc[pairs[i]]);
			//}
		}

		// calculate the Transformation
		float q = m_UQFindRAndT(m_rsc, tempDst, tempR, tempT);
#else
		vector<int> indicesRsc, indicesDst;
		float curDist;
		featureAssociateMat(m_rsc, m_dst, sigma, indicesRsc, indicesDst, curDist);

		cout<<"The number of useful feature is: "<<indicesRsc.size()<<endl;
		
		if(minDist <= curDist){
			// the best result is found
			break;
		}

		minDist = curDist;
		cout<<"The minimal distance of ICP is: "<<minDist<<endl;
		//sigma = curDist/indicesRsc.size();

		Mat tempRsc = Mat(indicesRsc.size(), 1, CV_32FC3);
		Mat tempDst = Mat(indicesDst.size(), 1, CV_32FC3);
		
		oldResult.clear();
		newResult.clear();
		for(int i=0;i<indicesRsc.size();i++){
			Point3f pointRsc = m_rsc.at<Point3f>(indicesRsc[i],0);
			Point3f pointDst = m_dst.at<Point3f>(indicesDst[i],0);

			tempRsc.at<Point3f>(i,0) = pointRsc;
			tempDst.at<Point3f>(i,0) = pointDst;

			oldResult.push_back(rsc[indicesRsc[i]]);
			newResult.push_back(dst[indicesDst[i]]);
		}
		
		float q = m_UQFindRAndT(tempRsc, tempDst, tempR, tempT);
#endif
		if(q==0){
			cout<<"The ICP faild, because of the invalid positive eigenvalue!"<<endl;
			return false;
		}
		R *= tempR;
		T += tempT;

		// update the resource points
		for(int i=0;i<m_rsc.rows;i++){
			Mat row = m_rsc.rowRange(i,i+1);
			row = row.reshape(1);
			row = row*R + T.t();
			m_rsc.row(i) = row;
			//m_rsc = m_rsc*R + T;
		}

		//if(curDist < 0.001 || loopCount > maxLoop){
		//	// the best result is found
		//	break;
		//}
		loopCount++;
	}
	cout << "converged" << endl;

	//if(dst.size() < rsc.size()){
	//	R = R.inv();
	//	T = -T;

	//	vector<PMDPoint> temp = oldResult;
	//	oldResult.clear();
	//	oldResult = newResult;
	//	newResult.clear();
	//	newResult = temp;
	//}
	
	cout<<"============================ End of the ICP ======================="<<endl;
	if(loopCount>maxLoop){
		return false;
	} else {
		return true;
	}
}

// Using bounding Box to get the ICP data
void createBoundingBox(vector<PMDPoint> &result, vector<PMDPoint> features, BildData *bildData){
	result.clear();
	int size = features.size();
	// calculate the 2D middel of all features
	Point2f middel = Point2f(0,0);
	for(int i= 0;i<size;i++){
		middel.x += features[i].index.x;
		middel.y += features[i].index.y;
	}
	middel.x = middel.x/size;
	middel.y = middel.y/size;

	//float maxDist = -1;
	//for(int i=0;i<size;i++){
	//	float dist = (features[i].coord.x - middel.x)*(features[i].coord.x - middel.x) +
	//				 (features[i].coord.y - middel.y)*(features[i].coord.y - middel.y);
	//	if(dist>maxDist){
	//		maxDist = dist;
	//	}
	//}

	//int boxSize = int(maxDist);
	int boxSize = 15;
	Point2i center = Point2i(int(middel.x), int(middel.y));
	for(int i=center.x-boxSize;i<center.x+boxSize;i++){
		for(int j=center.y-boxSize;j<center.y+boxSize;j++){
			Point3f point3D = Point3f(bildData->threeDData[(i*H_BILDSIZE+j)*3],
									  bildData->threeDData[(i*H_BILDSIZE+j)*3 +1],
									  bildData->threeDData[(i*H_BILDSIZE+j)*3 +2]);
			Point2f point2D = Point2f(i,j);
			result.push_back(PMDPoint(point3D, point2D));
		}
	}
}



void createBoundingPoints(vector<PMDPoint> &result, vector<PMDPoint> features, BildData *bildData, int boxSize){
	int size = features.size();
	for(int i=0;i<size;i++){
		Point2i curPoint = Point2i(int(features[i].index.x), int(features[i].index.y));
		for(int j=curPoint.x-boxSize;j<=curPoint.x+boxSize;j++){
			for(int k=curPoint.y-boxSize;k<=curPoint.y+boxSize;k++){
				Point3f point3D = Point3f(bildData->threeDData[(j*H_BILDSIZE+k)*3],
										  bildData->threeDData[(j*H_BILDSIZE+k)*3 +1],
										  bildData->threeDData[(j*H_BILDSIZE+k)*3 +2]);
				Point2f point2D = Point2f(j,k);
				result.push_back(PMDPoint(point3D, point2D));
			}
		}
	}
}
	