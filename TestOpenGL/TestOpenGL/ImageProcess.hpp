#ifndef IMAGEPROCESS_HPP
#define IMAGEPROCESS_HPP

#include <cv.h>
#include "flann/flann.hpp"
//#include "Object.hpp"
#include "Graph.hpp"
//#include "PMDPoint.hpp"
#include "BildData.hpp"

using namespace cv;
using namespace std;

void transFloatToChar(float *src, unsigned char *dst, float balance, float contrast);

void transFloatTo3Char(float *src, unsigned char *dst, float balance, float contrast);

void transIntensityToGrayValue(float *src, unsigned char *dst);

void transAmplitudeToGrayValue(float *src, unsigned char *dst);

Point2f point3To2(Point3f point);

void filterDepthDate(float threeDData[], double sigma);

void initKalmanFilter();

void initQKFilter();

bool brightnessControll(int vectorSize, float &contrast, int &detecParam, unsigned char *data);

void calibration(vector<vector<Point3f>> &result, vector<Point3f> points, float eps);

void calibration2D(vector<vector<KeyPoint>> &groupFeatures, vector<KeyPoint> features, float eps);

void calibration3(vector<vector<Point3f>> &result, vector<vector<Point2f>> &resultIndex, vector<Point3f> points, vector<Point2f> indexs, float eps);

void calibrationPMDPoint(vector<vector<PMDPoint>> &result, vector<PMDPoint> points, float eps);

void calibrationWithDistance(vector<PMDPoint> &oldResult, vector<PMDPoint> &newResult, float eps);

// get the square of the euclidean distance
float getEuclideanDis(Point3f p1, Point3f p2);

void DBSCAN(vector<vector<Point3f>> &C, vector<Point3f> D, float eps, int minPts);

void DBSCANPMDPoint(vector<vector<PMDPoint>> &C, vector<PMDPoint> D, float eps, int minPts);

void DBSCANPMDPoint(vector<vector<PMDPoint>> &C, vector<PMDPoint> D, Point3f center, float eps, int minPts);

void findMaxPointsSet(vector<vector<Point3f>> pointsSets, vector<Point3f> &maxSet);

void findMaxIndexesSet(vector<vector<Point2f>> indexesSets, vector<Point2f> &maxSet);

void findMaxPMDPointSet(vector<vector<PMDPoint>> indexesSets, vector<PMDPoint> &maxSet);

template<class T>
void findMaxSet(vector<vector<T>> sets, vector<T> &maxSet);

void featureAssociate2(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, vector<int> &findIndexOld, vector<int> &findIndexNew);

bool featureAssociate(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, 
					   vector<Point3f> &findFeatureOld, vector<Point3f> &findFeatureNew,
					   vector<int> &findIndexOld, vector<int> &findIndexNew,
					   float &avrDis, float &disPE);
/**
 * @param avrDis the average distance of all correspondence points
 * @param disPE  the average difference between the matrix P and identity matrix E
 * @param sumP   the sum of the digonal elements of matrix P
 */
bool featureAssociatePMD(vector<PMDPoint> oldFeature, vector<PMDPoint> newFeature, float sigma, 
					   vector<PMDPoint> &findFeatureOld, vector<PMDPoint> &findFeatureNew,
					   float &avrDis, float &disPE, float &sumP);

void SVDFindRAndT(vector<Point3f> oldFeatures, vector<Point3f> newFeatures, Mat &R, Mat &T);

float UQFindRAndT(vector<Point3f> oldFeatures, vector<Point3f> newFeatures, Mat &R, Mat &T);

float m_UQFindRAndT(Mat &M, Mat &D, Mat &R, Mat &T);

bool isBigNoised(Mat T, float angle, int frameDiff, float eLinear, float eAngular);

bool isBigNoised2(Graph *graph, vector<Point3f> points, Mat &R, Mat &T, float aRate, float e);

float getCorresRate(Graph *graph, vector<Point3f> points, Mat &R, Mat &T, float e);

void calcEulerAngleFromR(Mat R, Vec3d &euler1, Vec3d &euler2);

void decPMDPointVector(vector<PMDPoint> pmdPoints, vector<Point3f> &points3D, vector<Point2f> &points2D);

bool ICP(vector<PMDPoint> src, vector<PMDPoint> dst, Mat &R, Mat &T, vector<PMDPoint> &oldResult, vector<PMDPoint> &newResult);

void createBoundingBox(vector<PMDPoint> &result, vector<PMDPoint> features, BildData *bildData);

void createBoundingPoints(vector<PMDPoint> &result, vector<PMDPoint> features, BildData *bildData, int boxSize);

#endif
