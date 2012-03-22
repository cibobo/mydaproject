#ifndef IMAGEPROCESS_HPP
#define IMAGEPROCESS_HPP

#include <cv.h>
//#include "Object.hpp"
#include "Graph.hpp"

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

void calibrationWithDistance(vector<Point3f> &oldResult, vector<Point3f> &newResult);

void DBSCAN(vector<vector<Point3f>> &C, vector<Point3f> D, float eps, int minPts);

void findMaxPointsSet(vector<vector<Point3f>> pointsSets, vector<Point3f> &maxSet);

void featureAssociate2(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, vector<int> &findIndexOld, vector<int> &findIndexNew);

float featureAssociate(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, vector<Point3f> &findFeatureOld, vector<Point3f> &findFeatureNew, vector<int> &findIndexOld, vector<int> &findIndexNew);

void SVDFindRAndT(vector<Point3f> oldFeatures, vector<Point3f> newFeatures, Mat &R, Mat &T);

float UQFindRAndT(vector<Point3f> oldFeatures, vector<Point3f> newFeatures, Mat &R, Mat &T);

bool isBigNoised(Mat T, float angle, int frameDiff, float eLinear, float eAngular);

bool isBigNoised2(Graph *graph, vector<Point3f> points, Mat &R, Mat &T, float aRate, float e);

float getCorresRate(Graph *graph, vector<Point3f> points, Mat &R, Mat &T, float e);

void calcEulerAngleFromR(Mat R, Vec3d &euler1, Vec3d &euler2);

#endif
