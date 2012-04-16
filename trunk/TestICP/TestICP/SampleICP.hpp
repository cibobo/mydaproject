#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc_c.h>

using namespace cv;

#include <iostream>
#include <vector>
#include <limits>
using namespace std;


void ICP(Mat& X, Mat& destination) ;