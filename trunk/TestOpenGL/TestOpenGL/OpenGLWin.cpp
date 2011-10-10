/* An example of the minimal Win32 & OpenGL program.  It only works in   16 bit color modes or higher (since it doesn't create a   palette). */
#include "stdafx.h"
#include "OpenGLWin.hpp"

#define DATA_SIZE 204;
using namespace std;


float glDisData[204*204];
float *pglDisData;

float glIntData[204*204];
float *pglIntData;

float glAmpData[204*204];
float *pglAmpData;

// Declared static (no need for object reference
static float X = 0.0f;        // Translate screen to x direction (left or right)
static float Y = 0.0f;        // Translate screen to y direction (up or down)
static float Z = 2.0f;        // Translate screen to z direction (zoom in or out)
static float rotX = 0.0f;    // Rotate screen on x axis 
static float rotY = 0.0f;    // Rotate screen on y axis
static float rotZ = 0.0f;    // Rotate screen on z axis

static float rotLx = 0.0f;   // Translate screen by using the glulookAt function 
                             // (left or right)
static float rotLy = 0.0f;   // Translate screen by using the glulookAt function 
                             // (up or down)
static float rotLz = 5.0f;   // Translate screen by using the glulookAt function 
                                     // (zoom in or out)

// The Width and Height of the Window
int width;
int height;

static float contrast = 3200;
static float balance = 5000;

void display(){    
	  
	// Clear the Color Buffer and Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// 3D View Port
	glViewport(0, 0, height, height);

	//set the view point
	gluLookAt(rotLx, rotLy, rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();   // It is important to push the Matrix before 
                                 
	// calling glRotatef and glTranslatef
	glRotatef(rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z

	//set the origin depth
	//Z = data[204*102];
	//Z = 2;
	glTranslatef(X, Y, Z);
	glColor3f(1.0f,1.0f,1.0f);
 
	float factor = 2.04/204;

	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			glVertex3f((i-102)*factor, (j-102)*factor, -pglDisData[i*204+j]);
		}
	}
			
	glEnd();    


	

    glPopMatrix();                   // Don't forget to pop the Matrix

	
	// Grayscale View Port
	glLoadIdentity();
	glViewport(height, height/2, (width-height), height/2.0);
	glPushMatrix();   // It is important to push the Matrix before 
	glTranslatef(0, 0, -3);
	float grayValue;
	//cout<<"Balance: "<<balance<<"    contrast: "<<contrast<<endl;
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			grayValue = (pglIntData[i*204+j]-balance)/contrast;
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
	glViewport(height, 0, (width-height), height/2.0);
	glPushMatrix();   // It is important to push the Matrix before 
	glTranslatef(0, 0, -3);
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			//using HSV color mode. some variables has been omited
			float h = 240-pglAmpData[i*204+j]/6;
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




/**
 * load data with a pointer of float array
 */
void openGLLoadData(float *disData, float *intData, float *ampData){
	pglDisData = disData;
	pglIntData = intData;
	pglAmpData = ampData;
	//PostMessage(hWnd, WM_PAINT, 0, 0);
}

/*

 */
int InitGL()
{
	/*  */
	glShadeModel(GL_FLAT);

	/* schwarzer Hintergrund */
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    glClearDepth(1.0f);          // Set the Depth buffer value (ranges[0,1])
    glEnable(GL_DEPTH_TEST);  // Enable Depth test
    glDepthFunc(GL_LEQUAL);   // If two objects on the same coordinate 
                                            // show the first drawn
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	if(pglDisData ==  NULL){
		pglDisData = glDisData;
	}
	if(pglIntData ==  NULL){
		pglIntData = glIntData;
	}
	if(pglAmpData ==  NULL){
		pglAmpData = glAmpData;
	}

	//char *ch = "data/3/amplitude/0000.dat";
	//if(!loadData<float>(ch, pglAmpData, 204*204)){
	//	cout<<"data 0050 load error!"<<endl;
	//	//exit(0);
	//} else {
	//	cout<<"data 0050 successful load!"<<endl;
	//	cout<<"The middel is "<<pglAmpData[204*102]<<endl;
	//}

	return TRUE;
}


/*

 */
GLvoid ReSizeGLScene(GLsizei iWidth, GLsizei iHeight)
{
	/*  */
	glViewport(0, 0, iWidth, iHeight);	
	
	/* Projection Transformation */
	glMatrixMode(GL_PROJECTION);						
	glLoadIdentity();

	// set the rate of width and height as 1
	gluPerspective(45.0f, (GLfloat)iHeight/(GLfloat)iHeight, 0.5f, 120.0f);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	width = iWidth;
	height = iHeight;

}


LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){     
	static PAINTSTRUCT ps;

	static OpenGLWinUI *pOpenGLWinUI;
	static float fMousePosX;
	static float fMousePosY;
	static float fMousePosOldX;
	static float fMousePosOldY;

	/*
	 * isZChanged controlled by key Shift, which will be set true if the Shift key pressed and false if the key released
	 */
	static bool isZChanged = false;

	switch(uMsg) {
		case WM_CREATE:
			/*  */
			pOpenGLWinUI = (OpenGLWinUI*)((CREATESTRUCT*)lParam)->lpCreateParams;

			/* Init */
			pOpenGLWinUI->fMouseMoveLBX = 0.0f;
			pOpenGLWinUI->fMouseMoveLBY = 0.0f;
			
			pOpenGLWinUI->fMouseMoveRBX = 0.0f;
			pOpenGLWinUI->fMouseMoveRBY = 0.0f;

			pOpenGLWinUI->fMouseWheel = 0.0f;

		return TRUE;

		/* Controlle Message for Mouse */
		case WM_LBUTTONDOWN:
		
			fMousePosOldX = GET_X_LPARAM(lParam);
			fMousePosOldY = GET_Y_LPARAM(lParam);

		return TRUE;						
		

		case WM_RBUTTONDOWN:
		
			fMousePosOldX = GET_X_LPARAM(lParam);
			fMousePosOldY = GET_Y_LPARAM(lParam);

		return TRUE;						


		case WM_MOUSEMOVE:
		
			if(wParam == MK_LBUTTON)
			{
				fMousePosX = GET_X_LPARAM(lParam);
				fMousePosY = GET_Y_LPARAM(lParam);

				pOpenGLWinUI->fMouseMoveLBX -= (fMousePosX - fMousePosOldX);
				pOpenGLWinUI->fMouseMoveLBY -= (fMousePosY - fMousePosOldY);

				/*
				 * Horizontal displacement displayed the rotation around Z axis and 
				 * Vertical displacement displayed the rotation around X axis
				 */
				if(rotLz!=0){
					float angleZ = (fMousePosX - fMousePosOldX)/rotLz;
					float angleX = (fMousePosY - fMousePosOldY)/rotLz;
					//calculate the Radian
					rotZ +=angleZ/1;
					rotX +=angleX/1;
					//cout<<(fMousePosY - fMousePosOldY)<<"   "<<angleZ<<endl;
				}
			
				fMousePosOldX = fMousePosX;
				fMousePosOldY = fMousePosY;

				PostMessage(hWnd, WM_PAINT, 0, 0);	
			}

			if(wParam == MK_RBUTTON)
			{
				fMousePosX = GET_X_LPARAM(lParam);
				fMousePosY = GET_Y_LPARAM(lParam);

				pOpenGLWinUI->fMouseMoveRBX -= (fMousePosX - fMousePosOldX);
				pOpenGLWinUI->fMouseMoveRBY -= (fMousePosY - fMousePosOldY);
			
				fMousePosOldX = fMousePosX;
				fMousePosOldY = fMousePosY;
			}
		
		
		return TRUE;						
		

		case WM_LBUTTONDBLCLK:
		
			/* Init */
			pOpenGLWinUI->fMouseMoveLBX = 0.0f;
			pOpenGLWinUI->fMouseMoveLBY = 0.0f;
			
			pOpenGLWinUI->fMouseMoveRBX = 0.0f;
			pOpenGLWinUI->fMouseMoveRBY = 0.0f;

			pOpenGLWinUI->fMouseWheel = 0.0f;

		return TRUE;						
		

		case WM_MOUSEWHEEL:
			//if SHIFT is pressed
			if(isZChanged){
				if((int)wParam > 0){
					pOpenGLWinUI->fMouseWheel += 1.0f;
					Z += 0.3;
				} else {
					pOpenGLWinUI->fMouseWheel -= 1.0f;
					Z -= 0.3;
				}
			} else {
				if((int)wParam > 0){
					pOpenGLWinUI->fMouseWheel += 1.0f;
					rotLz += 0.3;
				} else {
					pOpenGLWinUI->fMouseWheel -= 1.0f;
					rotLz -= 0.3;
				}
			}
		PostMessage(hWnd, WM_PAINT, 0, 0);	
		return TRUE;			

		case WM_KEYDOWN:
		
			//printf("\n\nKey = %d", (int)wParam);

			switch(wParam)
			{
				case 120:    // x             // Rotates screen on x axis 
                    rotX -= 2.0f;
                    break;
                case 88:    // X            // Opposite way 
                    rotX += 2.0f;
                    break;
                case 121:    // y            // Rotates screen on y axis
                    rotY -= 2.0f;
                    break;
                case 89:    // Y            // Opposite way
                    rotY += 2.0f;
                    break;
                case 122:    // z            // Rotates screen on z axis
                    rotZ -= 2.0f;
                    break;
                case 90:    // Z            // Opposite way
                    rotZ += 2.0f;
                    break;

				// the input value of case must be a capital letter 
				case 'B':
					balance += 10;
					break;
				case 'V':
					balance -= 10;
					break;
				case 'N':
					contrast += 5;
					break;
				case 'M':
					contrast -=5;
					break;

				case VK_SHIFT:
					//cout<<"Shift pressed!"<<endl;
					isZChanged = true;
					break;
				//case 'n':
				//	openGLLoadData()
			}
			PostMessage(hWnd, WM_PAINT, 0, 0);	
			return 0;

		case WM_KEYUP:
			switch(wParam){
				case VK_SHIFT:
					isZChanged = false;
					break;
			}
			PostMessage(hWnd, WM_PAINT, 0, 0);	
			return 0;

		case WM_PAINT:	
			display();
			BeginPaint(hWnd, &ps);	
			EndPaint(hWnd, &ps);	
			return 0;  

		case WM_SIZE:	
			ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));	
			PostMessage(hWnd, WM_PAINT, 0, 0);	
			return 0;

		case WM_CHAR:	
			switch (wParam) {	
				case 27:			/* ESC key */	    
					PostQuitMessage(0);	    
					break;	
			}	
			return 0; 

		case WM_CLOSE:	PostQuitMessage(0);	
			return 0;    
	}    
	return DefWindowProc(hWnd, uMsg, wParam, lParam); 
} 

HWND CreateOpenGLWindow(char* title, int x, int y, int iWidth, int iHeight, 		   
					   BYTE type, DWORD flags, OpenGLWinUI *pOpenGLWinUI){    

	width = iWidth;
	height = iHeight;

	GLuint         PixelFormat;    
	//HDC         hDC;    
	//HWND        hWnd;    
	WNDCLASS    wc;    

	HDC hDC = NULL;				/* device context */    
	HGLRC hRC = NULL;				/* opengl context */    
	HWND  hWnd = NULL;				/* window */    

	DWORD	dwExStyle;
	DWORD	dwStyle;

	RECT WindowRect;                            // Grabs Rectangle Upper Left / Lower Right Values 
	WindowRect.left=(long)0;                        // Set Left Value To 0 
	WindowRect.right=(long)iWidth;                       // Set Right Value To Requested Width 
	WindowRect.top=(long)0;                         // Set Top Value To 0 
	WindowRect.bottom=(long)iHeight;                     // Set Bottom Value To Requested Height 

	static HINSTANCE hInstance = 0;    /* only register the window class once - use hInstance as a flag. */    
	if (!hInstance) {	
		hInstance = GetModuleHandle(NULL);	
		wc.style         = CS_OWNDC;	
		wc.lpfnWndProc   = (WNDPROC)WindowProc;	
		wc.cbClsExtra    = 0;	
		wc.cbWndExtra    = 0;	
		wc.hInstance     = hInstance;	
		wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);	
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);	
		wc.hbrBackground = NULL;	
		wc.lpszMenuName  = NULL;	
		wc.lpszClassName = "OpenGL";	
		if (!RegisterClass(&wc)) {	    
			MessageBox(NULL, "RegisterClass() failed:  "		       
				"Cannot register window class.", "Error", MB_OK);	    
			return NULL;	
		}    
	}    

	dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	dwStyle   = WS_OVERLAPPEDWINDOW;	
	

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!(hWnd=CreateWindowEx(
				dwExStyle,              // Extended Style For The Window 
                "OpenGL",               // Class Name 
                title,                  // Window Title 
                WS_CLIPSIBLINGS |           // Required Window Style 
                WS_CLIPCHILDREN |           // Required Window Style 
                dwStyle,                // Selected Window Style 
                0, 0,                   // Window Position 
                WindowRect.right-WindowRect.left,   // Calculate Adjusted Window Width 
                WindowRect.bottom-WindowRect.top,   // Calculate Adjusted Window Height 
                NULL,                   // No Parent Window 
                NULL,                   // No Menu 
                hInstance,              // Instance 
                (LPVOID)pOpenGLWinUI)))                 // Don't Pass Anything To WM_CREATE 
	{ 
		KillGLWindow(hInstance, hWnd, hDC, hRC);                         // Reset The Display 
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} else {
		if(!InitGL()){
			PostQuitMessage(0);
		}
	}

	//TODO pixel format
	/*  */
	//static	PIXELFORMATDESCRIPTOR pfd =					// pfd Tells Windows How We Want Things To Be
	//{
	//	sizeof(PIXELFORMATDESCRIPTOR),					// Size Of This Pixel Format Descriptor
	//	1,								// Version Number
	//	PFD_DRAW_TO_WINDOW |						// Format Must Support Window
	//	PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
	//	PFD_DOUBLEBUFFER,						// Must Support Double Buffering
	//	PFD_TYPE_RGBA,							// Request An RGBA Format
	//	32,								// Select Our Color Depth
	//	0, 0, 0, 0, 0, 0,						// Color Bits Ignored
	//	0,								// No Alpha Buffer
	//	0,								// Shift Bit Ignored
	//	0,								// No Accumulation Buffer
	//	0, 0, 0, 0,							// Accumulation Bits Ignored
	//	16,								// 16Bit Z-Buffer (Depth Buffer)
	//	0,								// No Stencil Buffer
	//	0,								// No Auxiliary Buffer
	//	PFD_MAIN_PLANE,							// Main Drawing Layer
	//	0,								// Reserved
	//	0, 0, 0								// Layer Masks Ignored
	//};
 
	if (!(hDC=GetDC(hWnd)))                         // Did We Get A Device Context? 
	{ 
		KillGLWindow(hInstance, hWnd, hDC, hRC);                         // Reset The Display 
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 
	static	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(pfd));    
	pfd.nSize        = sizeof(pfd);    
	pfd.nVersion     = 1;    
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;    
	pfd.iPixelType   = type;    
	pfd.cColorBits   = 32;    

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))             // Did Windows Find A Matching Pixel Format? 
	{ 
		KillGLWindow(hInstance, hWnd, hDC, hRC);                         // Reset The Display 
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))               // Are We Able To Set The Pixel Format? 
	{ 
		KillGLWindow(hInstance, hWnd, hDC, hRC);                         // Reset The Display 
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 
	
	if (!(hRC=wglCreateContext(hDC)))                   // Are We Able To Get A Rendering Context? 
	{ 
		KillGLWindow(hInstance, hWnd, hDC, hRC);                         // Reset The Display 
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 
	if(!wglMakeCurrent(hDC,hRC))                        // Try To Activate The Rendering Context 
	{ 
		KillGLWindow(hInstance, hWnd, hDC, hRC);                         // Reset The Display 
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 

	ShowWindow(hWnd,SW_SHOW);                       // Show The Window 
	SetForegroundWindow(hWnd);                      // Slightly Higher Priority 
	SetFocus(hWnd);                             // Sets Keyboard Focus To The Window 
	ReSizeGLScene(iWidth, iHeight);                       // Set Up Our Perspective GL Screen 

	DescribePixelFormat(hDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);    
	//ReleaseDC(hWnd, hDC);    
	return hWnd;
	//return TRUE;
}    

GLvoid KillGLWindow(HINSTANCE hInstance, HWND hWnd, HDC hDC, HGLRC hRC)                         // Properly Kill The Window 
{ 
	//if (fullscreen)                             // Are We In Fullscreen Mode? 
	//{ 
	//	ChangeDisplaySettings(NULL,0);                  // If So Switch Back To The Desktop 
	//	ShowCursor(TRUE);                       // Show Mouse Pointer 
	//} 
	if (hRC)                                // Do We Have A Rendering Context? 
	{ 
		if (!wglMakeCurrent(NULL,NULL))                 // Are We Able To Release The DC And RC Contexts? 
		{ 
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION); 
		} 
		if (!wglDeleteContext(hRC))                 // Are We Able To Delete The RC? 
		{ 
			 MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION); 
		} 
		hRC=NULL;                           // Set RC To NULL 
	} 
	if (hDC && !ReleaseDC(hWnd,hDC))                    // Are We Able To Release The DC 
	{ 
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION); 
		hDC=NULL;                           // Set DC To NULL 
	} 
	if (hWnd && !DestroyWindow(hWnd))                   // Are We Able To Destroy The Window? 
	{ 
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION); 
		hWnd=NULL;                          // Set hWnd To NULL 
	} 
	if (!UnregisterClass("OpenGL",hInstance))               // Are We Able To Unregister Class 
    { 
        MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION); 
        hInstance=NULL;                         // Set hInstance To NULL 
    } 
} 