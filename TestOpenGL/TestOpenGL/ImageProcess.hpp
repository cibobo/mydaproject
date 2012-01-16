#include <cv.h>
#include "Object.hpp"

using namespace cv;
using namespace std;

void transFloatToChar(float *src, unsigned char *dst, float balance, float contrast);

void transFloatTo3Char(float *src, unsigned char *dst, float balance, float contrast);

void transIntensityToGrayValue(float *src, unsigned char *dst);

void transAmplitudeToGrayValue(float *src, unsigned char *dst);

void calibration(vector<Point2f> points, vector<Object> &objects);

void calibration(vector<vector<Point3f>> &result, vector<Point3f> points, float eps);

void findMaxPointsSet(vector<vector<Point3f>> pointsSets, vector<Point3f> &maxSet);

void featureAssociate(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, vector<int> &findIndexOld, vector<int> &findIndexNew);

void featureAssociate2(vector<Point3f> oldFeature, vector<Point3f> newFeature, float sigma, vector<Point3f> &findFeatureOld, vector<Point3f> &findFeatureNew);