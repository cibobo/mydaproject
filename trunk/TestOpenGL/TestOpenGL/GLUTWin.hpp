
#include <GL/gl.h>			/* OpenGL header file */
#include <GL/glu.h>			/* OpenGL utilities header file */
#include "glut/glut.h"

#include "Graph.hpp"

void init(void);

void reshape(int width, int height);

void mouse(int buton, int state, int x, int y);

void setGLUTGraph(Graph *ptr);

void keyboard(unsigned char key, int x, int y);

void display(void);