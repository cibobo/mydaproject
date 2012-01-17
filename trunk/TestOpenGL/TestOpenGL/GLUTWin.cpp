#include "stdafx.h"
#include "GLUTWin.hpp"

float angle = 0;

Graph *graph = NULL;

void init(void){
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glShadeModel(GL_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    glClearDepth(1.0f);          // Set the Depth buffer value (ranges[0,1])
    glEnable(GL_DEPTH_TEST);  // Enable Depth test
    glDepthFunc(GL_LEQUAL);   // If two objects on the same coordinate 
		
	// Open the light
	GLfloat mat_ambient[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	
    GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat local_view[] = {0.0};

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

	GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0};
	GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0};
	GLfloat light_specular[] = {0.5f, 0.5f, 0.5f , 1.0f};
	GLfloat light_position[] = {-1.0f, -1.0f, -10.0f, -1.0f};
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    //glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

	// Normal vector
	glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

	// material
	glFrontFace(GL_CW);
	glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_DIFFUSE);
}

void reshape(int width, int height){
	glViewport(0, 0, width, height);	
	int factor = 2;
	
	/* Projection Transformation */
	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();

   if (width <= height)
      glOrtho (-factor, factor, -factor*(GLfloat)height/(GLfloat)width,
         factor*(GLfloat)height/(GLfloat)width, -10.0, 10.0);
   else
      glOrtho (-factor*(GLfloat)width/(GLfloat)height,
         factor*(GLfloat)width/(GLfloat)height, -factor, factor, -10.0, 10.0);

	//if (width <= height)
 //     glOrtho(0.0, 6.0, 0.0, 6.0*(GLfloat)height/(GLfloat)width,
 //             -10.0, 10.0);
 //  else
 //     glOrtho(0.0, 6.0*(GLfloat)height/(GLfloat)width, 0.0, 6.0,
 //             -10.0, 10.0);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
}

void mouse(int buton, int state, int x, int y){
}

void keyboard(unsigned char key, int x, int y){
	switch (key) {
		case 'r':
			angle += 0.5;
			glutPostRedisplay();
			break;
		case 27:
		    exit(0);
		    break;

	}
}

void setGLUTGraph(Graph *ptr){
	graph = ptr;
	//glutPostRedisplay();
	//angle += 0.5;
	//display();
}

void drawCoordi(){
	glPushMatrix();
	//glTranslatef(0,0,1);

	float border = 2.5;
	// how many lines on one side
	int count = 15;
	float step = border/count;
	
	//draw x achses
	glColor3f(0.0f,1.0f,0.0f);
	for(int i=0;i<count;i++){
		glBegin(GL_LINES);
			glVertex3f(-border, i*step, 0);
			glVertex3f(border, i*step, 0);

			glVertex3f(-border, -i*step, 0);
			glVertex3f(border, -i*step, 0);
		glEnd();
	}

	//draw y achses
	glColor3f(0.0f, 0.0f, 1.0f);
	for(int i=0;i<count;i++){
		glBegin(GL_LINES);
			glVertex3f(i*step, -border, 0);
			glVertex3f(i*step, border, 0);

			glVertex3f(-i*step, -border, 0);
			glVertex3f(-i*step, border, 0);
		glEnd();
	}
	glPopMatrix();
	//glutPostRedisplay();
}

void drawBalls(){
	if(graph != NULL){
		glPushMatrix();

		glColor3f(1.0f, 0.0f, 0.0f);
		GLfloat mat_ambient[] = {0.0215, 0.1745, 0.0215, 1.0};
		GLfloat mat_diffuse[] = {0.07568, 0.61424, 0.07568, 1.0};
		GLfloat mat_specular[] = {0.633, 0.727811, 0.633, 1.0};

		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0);

		//glRotatef(angle, 0.0f, 0.0f, 1.0f);
		float factor = 4.08/204;
		int size = graph->nodeList.size();
		for(int i=0;i<size;i++){
			glTranslatef(graph->nodeList[i]->x*factor,graph->nodeList[i]->y*factor,-graph->nodeList[i]->z);
			glutSolidSphere(0.04, 20, 16);
			glTranslatef(-graph->nodeList[i]->x*factor,-graph->nodeList[i]->y*factor,graph->nodeList[i]->z);
		}

		glPopMatrix();

	}
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0.3f, 0.3f, 0.15f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f); 

	//glRotatef(3.14, 0.0f, 1.0f, 0.0f);
	glTranslatef(0,0,2.5);


	drawCoordi();

	drawBalls();
	//glLoadIdentity();

	glFlush();
	//glutSwapBuffers();
	glutPostRedisplay();

	Sleep(150);
	angle += 0.5;
}
