#include <cv.h>
#include "Object.hpp"

using namespace cv;
using namespace std;

void transFloatToChar(float *src, unsigned char *dst, float balance, float contrast);

void transFloatTo3Char(float *src, unsigned char *dst, float balance, float contrast);

void transIntensityToGrayValue(float *src, unsigned char *dst);

void transAmplitudeToGrayValue(float *src, unsigned char *dst);

void calibration(vector<Point2f> points, vector<Object> &objects);

void calibration(vector<vector<Point2f>> &result, vector<Point2f> points, float eps);

void findMaxPointsSet(vector<vector<Point2f>> pointsSets, vector<Point2f> &maxSet);

void featureAssociate(vector<Point2f> oldFeature, vector<Point2f> newFeature, float sigma, vector<int> &findIndexOld, vector<int> &findIndexNew);

void featureAssociate2(vector<Point2f> oldFeature, vector<Point2f> newFeature, float sigma, vector<Point2f> &findFeatureOld, vector<Point2f> &findFeatureNew);