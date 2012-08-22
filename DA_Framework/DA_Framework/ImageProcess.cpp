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
	for(int i=0;i<V_BILDSIZE;i++){
		for(int j=0;j<H_BILDSIZE;j++){
			int gray = (src[i*V_BILDSIZE+j]-balance)/contrast;
			if(gray>255){
				gray = 255;
			} else if(gray <0){
				gray = 0;
			}
			dst.at<unsigned char>(i,j) = gray;
		}
	}
}

void ImageProcess::transFloatToMat3(float *src, cv::Mat &dst, float balance, float contrast){
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

/*
 * Get the square euclidean distance
 */
float getSquareEuclideanDis(Point3f p1, Point3f p2){
	return (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) + (p1.z-p2.z)*(p1.z-p2.z);
}

/*
 * Helping Methode of DBSCAN: get all points, which are in the circle around P with radius eps
 */
void getRegionQuery(set<int> &N, vector<Point3f> D, Point3f P, float eps){
	for(int i=0;i<D.size();i++){
		if(D[i]!= P && getSquareEuclideanDis(P,D[i])<eps*eps){
			N.insert(i);
		}
	}
}

/*
 * Helping Methode of DBSCAN: get all points, which are in the circle around P with radius eps 
 * (using flann from OpenCV)
 */
int getRegionQuery(Point2f p, set<int> &indices, float eps, int minPts, flann::Index *flann_index){
	vector<float> point;
	point.push_back(p.x);
	point.push_back(p.y);
	//point.push_back(p.z);

	vector<float> dists;
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
}


void ImageProcess::DBSCAN(vector<vector<Point2f>> &C, vector<Point2f> D, float eps, int minPts){
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
				vector<Point2f> tempC;
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

void ImageProcess::DBSCANPMDPoint(vector<vector<PMDPoint>> &C, vector<PMDPoint> D, float eps, int minPts){
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
			//getRegionQuery(D3D[i],N,eps,minPts,&flann_index);
			getRegionQuery(N,D3D,D3D[i],eps);
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
						//getRegionQuery(D3D[indexPP], NN, eps, minPts, &flann_index);
						getRegionQuery(NN,D3D,D3D[indexPP],eps);
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

void ImageProcess::decPMDPointVector(vector<PMDPoint> pmdPoints, vector<Point3f> &points3D, vector<Point2f> &points2D){
	for(int i=0;i<pmdPoints.size();i++){
		points3D.push_back(pmdPoints[i].coord);
		points2D.push_back(pmdPoints[i].index);
	}
}

bool ImageProcess::featureAssociatePMD(vector<PMDPoint> oldFeature, vector<PMDPoint> newFeature, float sigma, 
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
		if(j==m && maxValue>0.76){
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

