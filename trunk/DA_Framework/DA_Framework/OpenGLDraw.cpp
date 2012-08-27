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


#ifdef KMEAN
	glBegin(GL_POINTS);
	for(int i=0;i<bildData->filted3DData.size();i++){
		int clusterIndex = bildData->clusterLabel.at<int>(i, 0);
		glColor3f(COLORLIST[clusterIndex][0], COLORLIST[clusterIndex][1], COLORLIST[clusterIndex][2]);
		glVertex3f(bildData->filted3DData[i].x, bildData->filted3DData[i].y, -bildData->filted3DData[i].z);
	}
	glEnd();

	//glBegin(GL_POINTS);
	//for(int i=0;i<bildData->culsterGroupe.size();i++){
	//	glColor3f(COLORLIST[i][0], COLORLIST[i][1], COLORLIST[i][2]);
	//	for(int j=0;j<bildData->culsterGroupe[i].size();j++){
	//		glVertex3f(bildData->culsterGroupe[i][j].x, bildData->culsterGroupe[i][j].y, -bildData->culsterGroupe[i][j].z);
	//	}
	//}
	//glEnd();

#else
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
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
			//glVertex3f((i-102)*factor, (j-102)*factor, -bildData->disData[i*204+j]);	
			glVertex3f(bildData->threeDData[(i*204+j)*3], bildData->threeDData[(i*204+j)*3 +1], -bildData->threeDData[(i*204+j)*3 +2]);
		}
	}
	glEnd();    

	// draw the features
	//glPointSize(10);
	////glBegin(GL_LINE_STRIP);
	//glBegin(GL_POINTS);
	//if(bildData->features.size()>0){

	//	//for(int i=0;i<bildData->features.size();i++){
	//	//	int colorIndex = bildData->featuresLabel.at<int>(i,0);
	//	//	glColor3f(COLORLIST[colorIndex][0], COLORLIST[colorIndex][1], COLORLIST[colorIndex][2]);
	//	//	glVertex3f(bildData->features[i].x, bildData->features[i].y, -bildData->features[i].z);
	//	//}
	//	glColor3f(1.0f, 0.0f, 0.0f);
	//	for(int i=0;i<bildData->features.size();i++){
	//		glVertex3f(bildData->features[i].coord.x, bildData->features[i].coord.y, -bildData->features[i].coord.z);
	//	}
	//	//for(int i=0;i<bildData->comFeatures.size();i++){
	//	//	glVertex3f(bildData->comFeatures[i].x, bildData->comFeatures[i].y, -bildData->comFeatures[i].z);
	//	//}
	//	glColor3f(1.0f,1.0f,1.0f);
	//}
	//glEnd();
	glPointSize(1);
#endif

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
			//glVertex3f(i*factor, j*factor, 0);
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

	glViewport(0, 0, height, height);
	//set the view point
	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();   // It is important to push the Matrix before 
                                 
	// calling glRotatef and glTranslatef
	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
    glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
    glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z

	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);

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
	//int size = graph->nodeList.size();
	float ballSize;
	//glTranslatef(-factor/2, factor/2, 0);
	for(int i=0;i<graph->nodeList.size();i++){
		if(graph->nodeList[i]->isFixed){
			ballSize = 0.02;

			// if the color of the node are not set
			if(graph->nodeList[i]->color == -1){
				graph->nodeList[i]->color = pOpenGLWinUI->colorIndex;
			}
			mat_diffuse[0] = COLORLIST[graph->nodeList[i]->color][0];
			mat_diffuse[1] = COLORLIST[graph->nodeList[i]->color][1];
			mat_diffuse[2] = COLORLIST[graph->nodeList[i]->color][2];
			mat_diffuse[3] = COLORLIST[graph->nodeList[i]->color][3];
		} else {
			ballSize = 0.01;
			mat_diffuse[0] = 0.07568;
			mat_diffuse[1] = graph->nodeList[i]->timmer*0.1+0.2;
			mat_diffuse[2] = 0.07568;
			mat_diffuse[3] = 1.0;
		}
		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

		
		glTranslatef(graph->nodeList[i]->x,graph->nodeList[i]->y,-graph->nodeList[i]->z);
		glutSolidSphere(ballSize, 20, 16);
		glTranslatef(-graph->nodeList[i]->x,-graph->nodeList[i]->y,graph->nodeList[i]->z);
	}

	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
	glFlush();
	glPopMatrix();   


	glLoadIdentity();
	glViewport(height, 0, height, height);

	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();   // It is important to push the Matrix before 

	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
	glTranslatef(-1, 0, 0);

	drawCoordi();
	int oldColor = -1;
	float x = 0;
	for(int i=0;i<graph->nodeList.size();i++){
		if(graph->nodeList[i]->isFixed){
			ballSize = 0.02;
			int newColor = graph->nodeList[i]->color;
			mat_diffuse[0] = COLORLIST[graph->nodeList[i]->color][0];
			mat_diffuse[1] = COLORLIST[graph->nodeList[i]->color][1];
			mat_diffuse[2] = COLORLIST[graph->nodeList[i]->color][2];
			mat_diffuse[3] = COLORLIST[graph->nodeList[i]->color][3];
			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

			if(newColor == oldColor){
				x += 0.3;
			} else {
				x = 0;
				oldColor = newColor;
			}
			//float x = graph->nodeList[i]->timmer/30.0;
			float y = graph->nodeList[i]->color/8.0;
			glTranslatef(x,y,0);
			glutSolidSphere(ballSize, 20, 16);
			glTranslatef(-x,-y,0);
		}
	}
	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
	glFlush();
	glPopMatrix();   
}
//
//void display(OpenGLWinUI *pOpenGLWinUI, vector<Object*> objects){
//	int height = pOpenGLWinUI->height;
//	int width = pOpenGLWinUI->width;
//
//	// Clear the Color Buffer and Depth Buffer
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	glLoadIdentity();
//	int objCount = objects.size();
//	if(objCount != 0){
//		width = width/objCount;
//	}
//
//	GLfloat mat_ambient[] = {0.0215, 0.1745, 0.0215, 1.0};
//	GLfloat mat_diffuse[] = {0.07568, 0.61424, 0.07568, 1.0};
//	GLfloat mat_specular[] = {0.633, 0.727811, 0.633, 1.0};
//
//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//    glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
//
//	//drawCoordi();
//	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
//	// calling glRotatef and glTranslatef
//	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
//	glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
//	glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z
//
//	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
//	glTranslatef(-1, 0, 0);
//
//	for(int i=0;i<objCount;i++){
//		glViewport(i*width, 0, width, width);
//		glPushMatrix();   // It is important to push the Matrix before 
//
//		drawCoordi();
//		
//		//mat_diffuse[0] = COLORLIST[i][0];
//		//mat_diffuse[1] = COLORLIST[i][1];
//		//mat_diffuse[2] = COLORLIST[i][2];
//		//mat_diffuse[3] = COLORLIST[i][3];
//
//		Object *curObject = objects[i];
//
//		for(int j=0;j<curObject->nodeList.size();j++){
//			float ballSize = 0.01;
//			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//
//			//if(curObject->nodeList[j]->timmer == -1){
//			//	//ballSize = 0.03;
//			//	mat_diffuse[0] = 1;//-mat_diffuse[0];
//			//	mat_diffuse[1] = 1;//-mat_diffuse[1];
//			//	mat_diffuse[2] = 1;//-mat_diffuse[2];
//			//	mat_diffuse[3] = 1.0;
//			//} else if(curObject->nodeList[j]->timmer == -2){
//			//	//ballSize = 0.03;
//			//	mat_diffuse[0] = 0;//-mat_diffuse[0];
//			//	mat_diffuse[1] = 1;//-mat_diffuse[1];
//			//	mat_diffuse[2] = 0;//-mat_diffuse[2];
//			//	mat_diffuse[3] = 1.0; 
//			//} else {
//			//	//ballSize = 0.02;
//			//	mat_diffuse[0] = COLORLIST[i][0];
//			//	mat_diffuse[1] = COLORLIST[i][1];
//			//	mat_diffuse[2] = COLORLIST[i][2];
//			//	mat_diffuse[3] = COLORLIST[i][3];
//			//}
//
//			//if(curObject->nodeList[j]->color == -1){
//			//	mat_diffuse[0] = COLORLIST[0][0];
//			//	mat_diffuse[1] = COLORLIST[0][1];
//			//	mat_diffuse[2] = COLORLIST[0][2];
//			//	mat_diffuse[3] = COLORLIST[0][3];
//			//} else {
//				mat_diffuse[0] = COLORLIST[curObject->nodeList[j]->color][0];
//				mat_diffuse[1] = COLORLIST[curObject->nodeList[j]->color][1];
//				mat_diffuse[2] = COLORLIST[curObject->nodeList[j]->color][2];
//				mat_diffuse[3] = COLORLIST[curObject->nodeList[j]->color][3];
//			//}
//
//			// The material should be new set
//			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//
//			glTranslatef(curObject->nodeList[j]->x,curObject->nodeList[j]->y,-curObject->nodeList[j]->z);
//			glutSolidSphere(ballSize, 20, 16);
//			glTranslatef(-curObject->nodeList[j]->x,-curObject->nodeList[j]->y,curObject->nodeList[j]->z);
//
//			//for(int k=0;k<curObject->nodeList[j]->edgeList.size();k++){
//			//	//mat_diffuse[0] = mat_diffuse[1] = mat_diffuse[2] = mat_diffuse[3] = 1.0;
//			//	glBegin(GL_LINES);
//			//		glVertex3f(curObject->nodeList[j]->edgeList[k].orgNode->x, curObject->nodeList[j]->edgeList[k].orgNode->y, -curObject->nodeList[j]->edgeList[k].orgNode->z); 
//			//		glVertex3f(curObject->nodeList[j]->edgeList[k].dstNode->x, curObject->nodeList[j]->edgeList[k].dstNode->y, -curObject->nodeList[j]->edgeList[k].dstNode->z); 
//			//	glEnd();
//			//}
//			map<Node*, float>::iterator k = curObject->nodeList[j]->neighbors.begin();
//			for(;k!=curObject->nodeList[j]->neighbors.end();k++){
//				//mat_diffuse[0] = mat_diffuse[1] = mat_diffuse[2] = mat_diffuse[3] = 1.0;
//				glBegin(GL_LINES);
//					glVertex3f(curObject->nodeList[j]->x, curObject->nodeList[j]->y, -curObject->nodeList[j]->z); 
//					glVertex3f(k->first->x, k->first->y, -k->first->z); 
//				glEnd();
//			}
//
//		}
//		glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
//		glFlush();
//		glPopMatrix();   
//	}
//}
//
//
//
//
//
//void displayCoord(OpenGLWinUI *pOpenGLWinUI, Mat R){
//	// Clear the Color Buffer and Depth Buffer
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glLoadIdentity();
//
//	glPushMatrix();   // It is important to push the Matrix before 
//
//
//	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
//	
//                                 
//	// calling glRotatef and glTranslatef
//	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
//    glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
//    glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z
//
//	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
//
//	if(!R.empty()){
//	float glParam[16];
//	glParam[0] = R.at<float>(0,0);
//	glParam[1] = R.at<float>(0,1);
//	glParam[2] = R.at<float>(0,2);
//	glParam[3] = 0;
//
//	glParam[4] = R.at<float>(1,0);
//	glParam[5] = R.at<float>(1,1);
//	glParam[6] = R.at<float>(1,2);
//	glParam[7] = 0;
//
//	glParam[8] = R.at<float>(2,0);
//	glParam[9] = R.at<float>(2,1);
//	glParam[10] = R.at<float>(2,2);
//	glParam[11] = 0;
//
//	glParam[12] = glParam[13] = glParam[14] = 0;
//	glParam[15] = 1;
//
//	//cout<<"The R is: "<<R<<endl;
//	//glLoadMatrixf(glParam);
//	glMultMatrixf(glParam);
//	}
//
//	
//
//	//float scale = sqrt(R.at<float>(0,1)*R.at<float>(0,1) + R.at<float>(0,2)*R.at<float>(0,2) + R.at<float>(0,3)*R.at<float>(0,3));
//	//float rX = R.at<float>(0,1)/scale;
//	//float rY = R.at<float>(0,2)/scale;
//	//float rZ = R.at<float>(0,3)/scale;
//	//float angle = acos(R.at<float>(0,0))*2*180/3.14;
//
//	//glRotatef(angle, rX, rY, rZ);
//
//	glBegin(GL_LINES);
//		glColor3f(1.0f, 0.0f, 0.0f);
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(1.0f, 0.0f, 0.0f);
//
//		glColor3f(0.0f, 1.0f, 0.0f);
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(0.0f, 1.0f, 0.0f);
//
//		glColor3f(1.0f, 1.0f, 1.0f);
//		glVertex3f(0.0f, 0.0f, 0.0f);
//		glVertex3f(0.0f, 0.0f, 1.0f);
//	glEnd();
//
//	glFlush();
//	glPopMatrix();   
//}
//
//void displayRecog(OpenGLWinUI *pOpenGLWinUI, Graph *graph){
//	int height = pOpenGLWinUI->height;
//	int width = pOpenGLWinUI->width;
//
//	// Clear the Color Buffer and Depth Buffer
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glLoadIdentity();
//
//	glViewport(0, 0, height, height);
//	//set the view point
//	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
//	glPushMatrix();   // It is important to push the Matrix before 
//                                 
//	// calling glRotatef and glTranslatef
//	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
//    glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
//    glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z
//
//	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
//	//glTranslatef(-2.04, 2.04, 0);
//	//glColor3f(1.0f,0.0f,1.0f);
//
//
//	GLfloat mat_ambient[] = {0.0215, 0.1745, 0.0215, 1.0};
//	GLfloat mat_diffuse[] = {0.07568, 0.61424, 0.07568, 1.0};
//	GLfloat mat_specular[] = {0.633, 0.727811, 0.633, 1.0};
//
//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//    glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
// 
//	drawCoordi();
//
//	//int size = graph->nodeList.size();
//	float ballSize;
//	//glTranslatef(-factor/2, factor/2, 0);
//	for(int i=0;i<graph->nodeList.size();i++){
//		
//		ballSize = 0.02;
//
//		mat_diffuse[0] = COLORLIST[graph->nodeList[i]->color][0];
//		mat_diffuse[1] = COLORLIST[graph->nodeList[i]->color][1];
//		mat_diffuse[2] = COLORLIST[graph->nodeList[i]->color][2];
//		mat_diffuse[3] = COLORLIST[graph->nodeList[i]->color][3];
//
//		glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//
//		//if(graph->nodeList[i]->isFixed){
//		glTranslatef(graph->nodeList[i]->x,graph->nodeList[i]->y,-graph->nodeList[i]->z);
//		glutSolidSphere(ballSize, 20, 16);
//		glTranslatef(-graph->nodeList[i]->x,-graph->nodeList[i]->y,graph->nodeList[i]->z);
//		//}
//	}
//	
//	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
//	glFlush();
//	glPopMatrix();   
//}
//
//void displayRecog(OpenGLWinUI *pOpenGLWinUI, vector<Graph*> graphs){
//	int height = pOpenGLWinUI->height;
//	int width = pOpenGLWinUI->width;
//
//	// Clear the Color Buffer and Depth Buffer
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glLoadIdentity();
//
//	glViewport(0, 0, height, height);
//	//set the view point
//	gluLookAt(pOpenGLWinUI->rotLx, pOpenGLWinUI->rotLy, pOpenGLWinUI->rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
//	glPushMatrix();   // It is important to push the Matrix before 
//                                 
//	// calling glRotatef and glTranslatef
//	glRotatef(pOpenGLWinUI->rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
//    glRotatef(pOpenGLWinUI->rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
//    glRotatef(pOpenGLWinUI->rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z
//
//	glTranslatef(pOpenGLWinUI->X, pOpenGLWinUI->Y, pOpenGLWinUI->Z);
//	//glTranslatef(-2.04, 2.04, 0);
//	//glColor3f(1.0f,0.0f,1.0f);
//
//
//	GLfloat mat_ambient[] = {0.0215, 0.1745, 0.0215, 1.0};
//	GLfloat mat_diffuse[] = {0.07568, 0.61424, 0.07568, 1.0};
//	GLfloat mat_specular[] = {0.633, 0.727811, 0.633, 1.0};
//
//	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
//	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//    glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
// 
//	drawCoordi();
//
//	//int size = graph->nodeList.size();
//	float ballSize;
//	//glTranslatef(-factor/2, factor/2, 0);
//	for(int j=0;j<graphs.size();j++){
//		Graph *graph = graphs[j];
//		for(int i=0;i<graph->nodeList.size();i++){
//			
//			ballSize = 0.02;
//
//			mat_diffuse[0] = COLORLIST[graph->nodeList[i]->color][0];
//			mat_diffuse[1] = COLORLIST[graph->nodeList[i]->color][1];
//			mat_diffuse[2] = COLORLIST[graph->nodeList[i]->color][2];
//			mat_diffuse[3] = COLORLIST[graph->nodeList[i]->color][3];
//
//			glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//
//			//if(graph->nodeList[i]->isFixed){
//			glTranslatef(graph->nodeList[i]->x,graph->nodeList[i]->y,-graph->nodeList[i]->z);
//			glutSolidSphere(ballSize, 20, 16);
//			glTranslatef(-graph->nodeList[i]->x,-graph->nodeList[i]->y,graph->nodeList[i]->z);
//			//}
//		}
//	}
//	
//	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
//	glFlush();
//	glPopMatrix();   
//}