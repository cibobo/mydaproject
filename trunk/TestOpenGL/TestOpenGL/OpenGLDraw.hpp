#include "OpenGLWin.hpp"

#include <cv.h>
#include <GL/glaux.h>

#include "BildData.hpp"
#include "Graph.hpp"
#include "MyDataIO.hpp"
#include "ImageProcess.hpp"

using namespace cv;
using namespace std;

void display(OpenGLWinUI *pOpenGLWinUI);

void display(OpenGLWinUI *pOpenGLWinUI, float *disData, float *intData, float *ampData);

void display(OpenGLWinUI *pOpenGLWinUI, BildData *bildData);    

void display(OpenGLWinUI *pOpenGLWinUI, vector<Point2f> features);

void display(OpenGLWinUI *pOpenGLWinUI, Graph *graph);

void displayCoord(OpenGLWinUI *pOpenGLWinUI, Mat R);