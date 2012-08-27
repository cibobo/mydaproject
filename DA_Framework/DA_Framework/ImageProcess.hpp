#ifndef IMAGEPROCESS_HPP
#define IMAGEPROCESS_HPP

#include <cv.h>

#include "BildData.hpp"

class ImageProcess{
public:
   /**
	* Gaussian Filter
	* Using 1 dimentional Gaussian Filter to filter the depth daten of the 3D Daten
	**/
	void static filterDepthDate(float threeDData[], double sigma);

	/**
	 * Translate the float data array to Matrix with 1 Channel in OpenCV
	 **/
	void static transFloatToMat(float *src, Mat &dst, float balance, float contrast);
	/**
	 * Translate the float data array to Matrix with 3 Channels in OpenCV
	 **/
	void static transFloatToMat3(float *src, Mat &dst, float balance, float contrast);

	/**
	 * The Cluster methode with DBSCAN
	 *   Refference: http://de.wikipedia.org/wiki/DBSCAN
	 **/
	void static DBSCAN(vector<vector<Point2f>> &C, vector<Point2f> D, float eps, int minPts);
	// DBSCAN for the PMDPoints
	void static DBSCANPMDPoint(vector<vector<PMDPoint>> &C, vector<PMDPoint> D, float eps, int minPts);

	//Helping function to separate a vector of PMDPoint into two vectors for both 3D Point and 2D Point
	void static decPMDPointVector(vector<PMDPoint> pmdPoints, vector<Point3f> &points3D, vector<Point2f> &points2D);

	/**
	 * To find the associated features between two frames
	 *   See the paper of G.L.Scott and H.C.Lonuet-Higgins:
	 *   "An algorithm for associating the features of two images"
	 **/
	bool static featureAssociatePMD(vector<PMDPoint> oldFeature, vector<PMDPoint> newFeature, float sigma, 
					   vector<PMDPoint> &findFeatureOld, vector<PMDPoint> &findFeatureNew,
					   float &avrDis, float &disPE, float &sumP);

	/**
	 * Using unit quaternions to calculate the absolute orientation
	 * Bertgikd K.P.Horn 1987
	 *
	 * Using KalmanFilter for the center point
	 **/
	//Matrix version
	float static m_UQFindRAndT(Mat &M, Mat &D, Mat &R, Mat &T, KalmanFilter *pTFilter=NULL);
	//Points vector's version
	float static UQFindRAndT(vector<Point3f> oldFeatures, vector<Point3f> newFeatures, Mat &R, Mat &T, KalmanFilter *pTFilter=NULL);
};

#endif;