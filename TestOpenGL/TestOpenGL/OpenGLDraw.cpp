#include "stdafx.h"
#include "OpenGLDraw.hpp"



/*
 *
 */
void display(OpenGLWinUI *pOpenGLWinUI){    
	  
	// Clear the Color Buffer and Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//set the view point
	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();   // It is important to push the Matrix before 
                                 
	// calling glRotatef and glTranslatef
	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
    glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
    glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z

	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
	
	glBegin(GL_POLYGON);
		glColor3f(0.5f,0.0f,0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		//glVertex3f(0.86f, 0.0f, 0.5f);
		//glVertex3f(-0.86f, 0.0f, 0.5f);

		glColor3f(0.0f,0.5f,0.0f);
		//glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.86f, 0.0f, 0.5f);
		//glVertex3f(0.0f, 0.0f, -1.0f);

		glColor3f(0.0f,0.0f,0.5f);
		//glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.86f, 0.0f, 0.5f);
		//glVertex3f(0.0f, 0.0f, -1.0f);

		glColor3f(1.0f,0.5f,0.5f);
		glVertex3f(0.0f, 0.0f, -1.0f);
		//glVertex3f(0.86f, 0.0f, 0.5f);
		//glVertex3f(-0.86f, 0.0f, 0.5f);
	glEnd();
	glPopMatrix();  
	
	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
    //glutPostRedisplay();           // Redraw the scene

	//glutSwapBuffers();
	glFlush();
}


void display(OpenGLWinUI *pOpenGLWinUI, float *disData, float *intData, float *ampData){    
	  
	int height = pOpenGLWinUI->height;
	int width = pOpenGLWinUI->width;

	// Clear the Color Buffer and Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// 3D View Port
	glViewport(0, 0, height, height);

	//set the view point
	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();   // It is important to push the Matrix before 
                                 
	// calling glRotatef and glTranslatef
	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
    glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
    glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z

	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
	glColor3f(1.0f,1.0f,1.0f);
 
	float factor = 2.04/204;

	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			glVertex3f((i-102)*factor, (j-102)*factor, -disData[i*204+j]);
		}
	}
			
	glEnd();    

    glPopMatrix();                   // Don't forget to pop the Matrix

	// Grayscale View Port
	glLoadIdentity();
	//glViewport(height, height/2, (width-height), height/2.0);
	glViewport(height, height/2, 204, 204);
	glPushMatrix();   // It is important to push the Matrix before 
	glTranslatef(0, 0, -3);
	float grayValue;
	//cout<<"Balance: "<<gBalance<<"    Contrast: "<<gContrast<<endl;
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			grayValue = (intData[i*204+j]-pOpenGLWinUI->gBalance)/pOpenGLWinUI->gContrast;
			if(grayValue>1){
				grayValue = 1;
			}
			if(grayValue<0){
				grayValue = 0;
			}
			glColor3f(grayValue,grayValue,grayValue);
			glVertex3f((i-102)*factor, (j-102)*factor, 0);
		}
	}
	//glVertex3f(0,0,-3);
	glEnd();
	glPopMatrix();        
	//cout<<"Gray Value: "<<pglDisData[204*201]<<endl;


	glLoadIdentity();
	//glViewport(height, 0, (width-height), height/2.0);
	glViewport(height, 0, 204, 204);
	glPushMatrix();   // It is important to push the Matrix before 
	glTranslatef(0, 0, -3);
	//cout<<"Balance: "<<aBalance<<"    Contrast: "<<aContrast<<endl;
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			//using HSV color mode. some variables has been omited
			float h = pOpenGLWinUI->aBalance-ampData[i*204+j]/pOpenGLWinUI->aContrast;
			//if(h<0) h=0;
			int hi = int(h/60);
			float f = h/60-hi;
			switch(hi){
				case 0:
					glColor3f(1,f,0);
					break;
				case 1:
					glColor3f(1-f,1,0);
					break;
				case 2:
					glColor3f(0,1,f);
					break;
				case 3:
					glColor3f(0,1-f,1);
					break;
				case 4:
					glColor3f(f,0,1);
					break;
				default:
					glColor3f(1,0,1-f);
			}
			glVertex3f((i-102)*factor, (j-102)*factor, 0);
		}
	}
	glEnd();
	glPopMatrix();  
	
	
	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
    //glutPostRedisplay();           // Redraw the scene

	//glutSwapBuffers();
	glFlush();
}

void display(OpenGLWinUI *pOpenGLWinUI, BildData *bildData){    
	  
	int height = pOpenGLWinUI->height;
	int width = pOpenGLWinUI->width;

	// Clear the Color Buffer and Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// 3D View Port
	glViewport(0, 0, height, height);

	//set the view point
	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();   // It is important to push the Matrix before 
                                 
	// calling glRotatef and glTranslatef
	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
    glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
    glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z

	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
	glColor3f(1.0f,1.0f,1.0f);
 
	float factor = 2.04/204;

	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			//glVertex3f((i-102)*factor, (j-102)*factor, -bildData->disData[i*204+j]);
			glVertex3f(bildData->threeDData[(i*204+j)*3], bildData->threeDData[(i*204+j)*3 +1], -bildData->threeDData[(i*204+j)*3 +2]);
		}
	}
			
	glEnd();    

    glPopMatrix();                   // Don't forget to pop the Matrix

	// Grayscale View Port
	glLoadIdentity();
	//glViewport(height, height/2, (width-height), height/2.0);
	glViewport(height, height/2, 204, 204);
	glPushMatrix();   // It is important to push the Matrix before 
	glTranslatef(0, 0, -3);
	float grayValue;
	//cout<<"Balance: "<<gBalance<<"    Contrast: "<<gContrast<<endl;
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			grayValue = (bildData->intData[i*204+j]-pOpenGLWinUI->gBalance)/pOpenGLWinUI->gContrast;
			if(grayValue>1){
				grayValue = 1;
			}
			if(grayValue<0){
				grayValue = 0;
			}
			glColor3f(grayValue,grayValue,grayValue);
			glVertex3f((i-102)*factor, (j-102)*factor, 0);
		}
	}
	//glVertex3f(0,0,-3);
	glEnd();
	glPopMatrix();        
	//cout<<"Gray Value: "<<pglDisData[204*201]<<endl;


	glLoadIdentity();
	//glViewport(height, 0, (width-height), height/2.0);
	glViewport(height, 0, 204, 204);
	glPushMatrix();   // It is important to push the Matrix before 
	glTranslatef(0, 0, -3);
	//cout<<"Balance: "<<aBalance<<"    Contrast: "<<aContrast<<endl;
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			//using HSV color mode. some variables has been omited
			float h = pOpenGLWinUI->aBalance-bildData->ampData[i*204+j]/pOpenGLWinUI->aContrast;
			//if(h<0) h=0;
			int hi = int(h/60);
			float f = h/60-hi;
			switch(hi){
				case 0:
					glColor3f(1,f,0);
					break;
				case 1:
					glColor3f(1-f,1,0);
					break;
				case 2:
					glColor3f(0,1,f);
					break;
				case 3:
					glColor3f(0,1-f,1);
					break;
				case 4:
					glColor3f(f,0,1);
					break;
				default:
					glColor3f(1,0,1-f);
			}
			glVertex3f((i-102)*factor, (j-102)*factor, 0);
		}
	}
	glEnd();
	glPopMatrix();  
	
	
	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
    //glutPostRedisplay();           // Redraw the scene

	//glutSwapBuffers();
	glFlush();
}


void display(OpenGLWinUI *pOpenGLWinUI, vector<Point2f> features){
	int height = pOpenGLWinUI->height;
	int width = pOpenGLWinUI->width;

	// Clear the Color Buffer and Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//set the view point
	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();   // It is important to push the Matrix before 
                                 
	// calling glRotatef and glTranslatef
	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
    glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
    glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z

	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
	glTranslatef(-1.02, 1.02, 0);
	glColor3f(1.0f,1.0f,1.0f);
 
	float factor = 2.04/204;

	glPointSize(5);
	for(int i=0;i<features.size();i++){
		glColor3f(1.0,0.0,0.0);
		glBegin(GL_POINTS);
			glVertex3f(features[i].x*factor, -features[i].y*factor, 0);
		glEnd();  
		for(int j=i;j<features.size();j++){
			glColor3f(0.0,1.0,0.0);
			glBegin(GL_LINES);
			    glVertex3f(features[i].x*factor, -features[i].y*factor, 0);
				glVertex3f(features[j].x*factor, -features[j].y*factor, 0);
			glEnd();
		}
	}
			
    glPopMatrix();                   // Don't forget to pop the Matrix
	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
	glFlush();
}

void drawCoordi(){
	glPushMatrix();
	glTranslatef(0,0,-5);

	float border = 5;
	// how many lines on one side
	int count = 25;
	float step = border/count;
	
	//draw x achses
	glColor3f(0.0f,1.0f,0.0f);
	for(int i=0;i<count;i++){
		glBegin(GL_LINES);
			glVertex3f(-border, i*step, 0);
			glVertex3f(border, i*step, 0);

			glVertex3f(-border, -i*step, 0);
			glVertex3f(border, -i*step, 0);
			//glVertex3f(0, -i*step, 0);
			//glVertex3f(border, -i*step, 0);
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
			//glVertex3f(i*step, -border, 0);
			//glVertex3f(i*step, 0, 0);
		glEnd();
	}

	// draw xz plane
	for(int i=0;i<count;i++){
		glBegin(GL_LINES);
			glVertex3f(border, i*step, 0);
			glVertex3f(border, i*step, 2*border);

			glVertex3f(border, -i*step, 0);
			glVertex3f(border, -i*step, 2*border);

			glVertex3f(border, -border, i*step);
			glVertex3f(border, border, i*step);

			glVertex3f(border, -border, border+i*step);
			glVertex3f(border, border, border+i*step);

		glEnd();
	}
	glPopMatrix();
	//glutPostRedisplay();
}

void display(OpenGLWinUI *pOpenGLWinUI, Graph *graph){
	int height = pOpenGLWinUI->height;
	int width = pOpenGLWinUI->width;

	// Clear the Color Buffer and Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//set the view point
	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();   // It is important to push the Matrix before 
                                 
	// calling glRotatef and glTranslatef
	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
    glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
    glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z

	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
	//glTranslatef(-2.04, 2.04, 0);
	//glColor3f(1.0f,0.0f,1.0f);
	drawCoordi();

	GLfloat mat_ambient[] = {0.0215, 0.1745, 0.0215, 1.0};
	GLfloat mat_diffuse[] = {0.07568, 0.61424, 0.07568, 1.0};
	GLfloat mat_specular[] = {0.633, 0.727811, 0.633, 1.0};

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
 
	float factor = 2.04;
	float rate = factor/204;
	int size = graph->nodeList.size();
	glTranslatef(-factor/2, factor/2, 0);
	for(int i=0;i<size;i++){
		if(graph->nodeList[i]->timmer>9){
			mat_diffuse[0] = 1.0;
			mat_diffuse[1] = 0.0;
		} else {
			mat_diffuse[0] = 0.07568;
			mat_diffuse[1] = graph->nodeList[i]->timmer*0.1+0.2;
		}
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

		glTranslatef(graph->nodeList[i]->x*rate,-graph->nodeList[i]->y*rate,-graph->nodeList[i]->z*rate*10);
		glutSolidSphere(0.03, 20, 16);
		glTranslatef(-graph->nodeList[i]->x*rate,graph->nodeList[i]->y*rate,graph->nodeList[i]->z*rate*10);
	}
	glTranslatef(factor/2, -factor/2, 0);

	//glPointSize(8);
	//for(int i=0;i<graph->nodeList.size();i++){
	//	//glColor3f(graph->nodeList[i]->timmer*0.15+0.2,0.0,0.0);
	//	if(graph->nodeList[i]->timmer>25){
	//		glColor3f(0.0f,1.0f,1.0f);
	//	} else {
	//		glColor3f(graph->nodeList[i]->timmer*0.15+0.2,0.0,0.0);
	//	}
	//	//glBegin(GL_POINTS);
	//	//	glVertex3f(graph->nodeList[i]->x*factor, -graph->nodeList[i]->y*factor, -graph->nodeList[i]->z);
	//	//glEnd();  
	//	glTranslatef(graph->nodeList[i]->x*factor, -graph->nodeList[i]->y*factor, -graph->nodeList[i]->z);
	//	glutSolidSphere(0.2, 20, 16);

	//	glColor3f(0.0,1.0,0.0);

	//	for(int j=0;j<graph->nodeList[i]->edgeList.size();j++){
	//		glBegin(GL_LINES);
	//			glVertex3f(graph->nodeList[i]->edgeList[j].orgNode->x*factor, -graph->nodeList[i]->edgeList[j].orgNode->y*factor, -graph->nodeList[i]->edgeList[j].orgNode->z); 
	//			glVertex3f(graph->nodeList[i]->edgeList[j].dstNode->x*factor, -graph->nodeList[i]->edgeList[j].dstNode->y*factor, -graph->nodeList[i]->edgeList[j].dstNode->z); 
	//		glEnd();
	//	}
	//}
			
	//glutSolidTeapot(0.8);
                    // Don't forget to pop the Matrix
	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
	glFlush();
	glPopMatrix();   
}

