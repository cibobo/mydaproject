#include "stdafx.h"
#include "ARToolKit.hpp"

using namespace std;

#define VAR_PARAM

char           *patt_name      = "Data/patt.hiro";
int				patt_id;
double          patt_width     = 80.0;
double          patt_center[2] = {0.0, 0.0};
double          patt_trans[3][4];

int             xsize, ysize;
int             thresh = 100;
int             count = 0;
ARUint8         *dataPtr;


//test data
int kCount = 0;

void setPattID(int pattID){
	patt_id = pattID;
}

void setARData(float *temp){
	int balance = 4900;
	int contrast = 38;
	//unsigned char data[166464];
	unsigned char data[41616];
	for(int i=0;i<204*204;i++){
		float gray = (temp[i]-balance)/contrast;
		if(gray>255){
			gray = 255;
		} else if(gray <0){
			gray = 0;
		}

		//data[4*i] = data[4*i+1] = data[4*i+2] = int(gray);
		//data[4*i+3] = 0;
		data[i] = gray;
	}
	dataPtr = data;
}

void thresholding(int param){
	CvSize size;
	size.height=204;
	size.width=204;
	IplImage* src = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage* dst = cvCreateImage(size, IPL_DEPTH_8U, 1);
	//cout<<"the size of imageData is: "<<sizeof(src->imageData)<<endl;
	src->imageData = (char*)dataPtr;

	cvThreshold(src, dst, param, param, CV_THRESH_TOZERO);
	dataPtr = (ARUint8*)dst->imageData;
	//dst->imageData = NULL;
	cvReleaseImage(&src);
	//cvReleaseImage(&dst);
}

void keyEvent(unsigned char key, int x, int y)
{
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
        exit(0);
    }
}

/* main loop */
void mainLoop(void)
{
    
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             j, k;

    /* grab a vide frame */
    //if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
    //    arUtilSleep(2);
    //    return;
    //}
	//saveARDataToFile(count,dataPtr);
	if( count == 0 ) {
		arUtilTimerReset();
	}

	//if( count == 10) {
	//	long ii = 0;
	//	unsigned char c = dataPtr[0];
	//	//cout<<"c is: "<<c<<endl;
	//	while(ii<76850){
	//		ii++;
	//		if(ii>=76780){
	//			c = dataPtr[ii];
	//			cout<<ii<<" c is: "<<int(c)<<endl;
	//		}
	//	}
	//	cout<<"Size of array: "<<ii<<endl;
	//}


	//if(count < 374){
	//	if((dataPtr = loadNormalDataForAR(count)) == NULL){
	//		arUtilSleep(2);
	//		return;
	//	}
	//
	//} else {
	//	return;
	//}

	//dataPtr = loadARDataFromFile(count);
		Sleep(100);
    count++;
	drawFrame(dataPtr);

	//start a loop to find the best contrast for Tracking
	int startParam = 100;
	
#ifdef VAR_PARAM
	int i;
	for(i=0;i<10;i++){
		//add the parameter of thredhold 10 everytime
		thresholding(startParam + 10*i);
#else
	thresholding(100);
#endif
		//argDrawMode2D();
		//argDispImage( dataPtr, 0,0 );


		//* detect the markers in the video frame */
		if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
			cleanup();
			exit(0);
		}
		//cout<<"find "<<marker_num<<" markers!"<<endl;
		arVideoCapNext();
		

		//* check for object visibility */
		k = -1;
		for( j = 0; j < marker_num; j++ ) {
			if( patt_id == marker_info[j].id ) {
				if( k == -1 ) k = j;
				else if( marker_info[k].cf < marker_info[j].cf ) k = j;
			}
		}

		//if there is a marker found, break the loop; else, continue
#ifdef VAR_PARAM
		if(k!=-1){
			kCount++;
			break; 
		}
	}
	cout<<"Usedddddddddddddddd  "<<i<<"  steps"<<endl;
#else
	if(k!=-1) kCount++;
#endif
    if( k == -1 ) {
        argSwapBuffers();
        return;
    } 
	
	

    //* get the transformation between the marker and the real camera */
    arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);
	cout<<"The transforation vektor: ("<<patt_trans[0][3]<<", "<<patt_trans[1][3]<<", "<<patt_trans[2][3]<<") "<<endl;   
    draw();


    argSwapBuffers();
	cout<<"kCount = "<<kCount<<endl;
}

void cleanup(void)
{
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}

void draw2(float *temp, int patt_id){
	//ARUint8 *dataPtr;
	//ARMarkerInfo    *marker_info;
 //   int             marker_num;
 //   int             j, k;

	//int balance = 4900;
	//int contrast = 38;
	////dataPtr = new unsigned char[41616];
	////for(int i=0;i<204*204;i++){
	////	float gray = (temp[i]-balance)/contrast;
	////	if(gray>255){
	////		gray = 255;
	////	} else if(gray <0){
	////		gray = 0;
	////	}
	////	dataPtr[i] = gray;
	////}

	//argDrawMode2D();
 //   argDispImage( dataPtr, 0,0 );
	//
	//if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
 //       cleanup();
 //       exit(0);
 //   }

	//k = -1;
 //   for( j = 0; j < marker_num; j++ ) {
 //       if( patt_id == marker_info[j].id ) {
 //           if( k == -1 ) k = j;
 //           else if( marker_info[k].cf < marker_info[j].cf ) k = j;
 //       }
 //   }
 //   if( k == -1 ) {
 //       argSwapBuffers();
 //        return;
 //   }

 //   //* get the transformation between the marker and the real camera */
 //   arGetTransMat(&marker_info[k], patt_center, patt_width, patt_trans);

 //   draw();

 //   argSwapBuffers();
	////delete []dataPtr;
}

	

void drawFrame(unsigned char *intData){
	argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	
	float factor = 40.8/204;
	glPushMatrix();   // It is important to push the Matrix before 
	glRotatef(90, 0.0, 0.0, 1.0);
	glTranslatef(0, 0, 50);

	float grayValue;
	//cout<<"Balance: "<<gBalance<<"    Contrast: "<<gContrast<<endl;
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			grayValue = intData[i*204+j]/256.0;
			glColor3f(grayValue,grayValue,grayValue);
			glVertex3f((i-102)*factor, -(j-102)*factor, 0);
		}
	}
	//glVertex3f(0,0,-3);
	glEnd();
	glPopMatrix();        
}

void draw(void)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
    double    gl_para[16];
    GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
    
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    
    
    /* load the camera transformation matrix */
    argConvGlpara(patt_trans, gl_para);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixd( gl_para );

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMatrixMode(GL_MODELVIEW);
    glTranslatef( 0.0, 0.0, 25.0 );
    glutSolidCube(50.0);
    glDisable( GL_LIGHTING );

    glDisable( GL_DEPTH_TEST );
}

