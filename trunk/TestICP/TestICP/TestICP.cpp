// TestICP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SampleICP.hpp"
#include "MyFeatureDetector.hpp"


int main(int argc, char** argv) {
	//int _X[22] = {7, 198, 37, 155, 76, 138, 126, 123, 177, 112, 229, 114, 277, 110, 363, 136, 399, 151, 433, 196, 439, 222,};
	//Mat X(11,2,CV_32SC1,_X);
	
//	int _X_bar[22] = {6, 125, 67, 130, 97, 92, 150, 99, 188, 68, 208, 89, 285, 78, 305, 105, 369, 100, 390, 137, 432, 150,};
//	//int _X_bar[22] = {38, 232, 20, 184, 28, 127, 53, 69, 92, 33, 154, 12, 200, 12, 270, 13, 318, 26, 357, 46, 386, 76,};
////	int _X_bar[22] = {5, 217, 22, 168, 68, 141, 111, 125, 158, 115, 202, 111, 235, 106, 283, 111, 366, 136, 395, 151, 432, 188,};
//	//Mat X_bar(11,2,CV_32SC1,_X_bar); Mat X_bar_32f; X_bar.convertTo(X_bar_32f,CV_32F);
	double theta = -0.01*CV_PI;
	float _RotM[4] = { cos(theta), -sin(theta), sin(theta), cos(theta) };

	//RNG rng;
	//Mat X32f; X.convertTo(X32f,CV_32F);
	//Mat n(X32f.size(),X32f.type()); rng.fill(n,RNG::UNIFORM,Scalar(-35),Scalar(35));

	//Mat X_bar_32f = (X32f);
	////X_bar_32f(Range(0,X32f.rows),Range(1,2)) -= 45;
	//X_bar_32f = X_bar_32f * Mat(2,2,CV_32FC1,_RotM) + n;

	//Mat destinations; X_bar_32f.convertTo(destinations,CV_32S);

	RNG rng;
	Mat X(10,2,CV_32SC1);
	for(int i=0;i<10;i++) {
		X.at<Point>(i,0) = Point(100+sin(((double)i/10.0)*CV_PI)*50.0,100+cos(((double)i/10.0)*CV_PI)*50.0);
	}
	Mat destinations(50,2,X.type()); 
	rng.fill(destinations,RNG::NORMAL,Scalar(150.0),Scalar(125.0,50.0));

	//Mat X_1ch = X.reshape(2,X.rows);
	//Mat X_bar_1ch = destinations.reshape(2,X_bar.rows);

	//findBestTransform(X_1ch,X_bar_1ch);

	//ICP(X,destinations);

	MyFeatureDetector detector = MyFeatureDetector(42, 65, 30);
	
	Mat img1 = imread("./data/img/0095.png", 0);

	detector.setDetectedMat(img1);
	detector.usingSTAR();

	vector<Point2f> features1;
	detector.getFeaturePoints(features1, 5);

	IplImage *object1 = &IplImage(img1);
	IplImage *object_color1 = cvCreateImage(cvGetSize(object1), 8, 3);
	//The resource matrix of this function must have a head of IplImage
	cvCvtColor(object1, object_color1, CV_GRAY2BGR);

	Mat showMat1 = Mat(object_color1);
	namedWindow("STARFeatures1", CV_WINDOW_AUTOSIZE);
	for(int i=0;i<features1.size();i++){
		circle(showMat1, features1[i], 2, Scalar(0,255,0,0), -1); 
	}
	imshow("STARFeatures1", showMat1);



	Mat img2 = imread("./data/img/0087.png", 0);

	detector.setDetectedMat(img2);
	detector.usingSTAR();

	vector<Point2f> features2;
	detector.getFeaturePoints(features2, 5);

	IplImage *object2 = &IplImage(img2);
	IplImage *object_color2 = cvCreateImage(cvGetSize(object2), 8, 3);
	//The resource matrix of this function must have a head of IplImage
	cvCvtColor(object2, object_color2, CV_GRAY2BGR);

	Mat showMat2 = Mat(object_color2);
	namedWindow("STARFeatures2", CV_WINDOW_AUTOSIZE);
	for(int i=0;i<features2.size();i++){
		circle(showMat2, features2[i], 2, Scalar(0,255,0,0), -1); 
	}
	imshow("STARFeatures2", showMat2);

	Mat X1 = Mat(features1);
	Mat X2 = Mat(features2);

	X1 = X1.reshape(1);
	X2 = X2.reshape(1);

	ICP(X1,X2);

	cvWaitKey(0);

	char a;
	cin>>a;
	return 0;
}

