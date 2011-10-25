#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <GL/gl.h>
#include "glut/glut.h"
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

#include "MyDataIO.hpp"

#include <iostream>

void setPattID(int pattID);

void setARData(float *temp);

void keyEvent(unsigned char key, int x, int y);

void mainLoop(void);

void cleanup(void);

void draw( void );