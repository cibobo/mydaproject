#ifndef OPENGLDRAW_HPP
#define OPENGLDRAW_HPP



#include <cv.h>
#include <GL/glaux.h>

#include "BildData.hpp"
//#include "Graph.hpp"
#include "Object.hpp"
#include "MyDataIO.hpp"
//#include "ImageProcess.hpp"
#include "OpenGLWin.hpp"

using namespace cv;
using namespace std;

void display(OpenGLWinUI *pOpenGLWinUI);

void display(OpenGLWinUI *pOpenGLWinUI, BildData *bildData);    

void display(OpenGLWinUI *pOpenGLWinUI, vector<Point2f> features);

void display(OpenGLWinUI *pOpenGLWinUI, Graph *graph);

void displayObjects(OpenGLWinUI *pOpenGLWinUI, vector<Object*> objects);

void displayGraphs(OpenGLWinUI *pOpenGLWinUI, vector<Graph*> graphs);

#endif