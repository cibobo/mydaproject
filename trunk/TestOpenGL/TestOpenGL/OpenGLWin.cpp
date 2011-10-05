/* An example of the minimal Win32 & OpenGL program.  It only works in   16 bit color modes or higher (since it doesn't create a   palette). */
#include "stdafx.h"
#include "OpenGLWin.hpp"

#define DATA_SIZE 204;
using namespace std;

HDC hDC;				/* device context */    
HGLRC hRC;				/* opengl context */    
HWND  hWnd;				/* window */    
HINSTANCE       hInstance;    // Holds The Instance Of The Application 

float data[204*204];
float *ptrData = data;

// Declared static (no need for object reference
static float X = 0.0f;        // Translate screen to x direction (left or right)
static float Y = 0.0f;        // Translate screen to y direction (up or down)
static float Z = 0.0f;        // Translate screen to z direction (zoom in or out)
static float rotX = 0.0f;    // Rotate screen on x axis 
static float rotY = 0.0f;    // Rotate screen on y axis
static float rotZ = 0.0f;    // Rotate screen on z axis

static float rotLx = 0.0f;   // Translate screen by using the glulookAt function 
                             // (left or right)
static float rotLy = 0.0f;   // Translate screen by using the glulookAt function 
                             // (up or down)
static float rotLz = 0.0f;   // Translate screen by using the glulookAt function 
                                     // (zoom in or out)



void display(){    
	  
	// Clear the Color Buffer and Depth Buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	//set the view point
	gluLookAt(rotLx, rotLy, 5.0f + 
                     rotLz, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();   // It is important to push the Matrix before 
                                 
	// calling glRotatef and glTranslatef
	glRotatef(rotX, 1.0f, 0.0f, 0.0f);            // Rotate on x
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);            // Rotate on y
    glRotatef(rotZ, 0.0f, 0.0f, 1.0f);            // Rotate on z

	//set the origin depth
	//Z = data[204*102];
	Z = 2;
	glTranslatef(X, Y, Z);

/*	glBegin(GL_TRIANGLES);    
	glColor3f(1.0f, 0.0f, 0.0f);    
	glVertex3f(0,  1, -3);    
	glColor3f(0.0f, 1.0f, 0.0f);    
	glVertex3f(-1, -1, -3);    
	glColor3f(0.0f, 0.0f, 1.0f);    
	glVertex3f(1, -1, -3); */   
	float factor = 2.04/204;
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			glVertex3f((i-102)*factor, (j-102)*factor, -ptrData[i*204+j]);
		}
	}
			
	glEnd();    
	
	glDisable(GL_LINE_STIPPLE);   // Disable the line stipple
    //glutPostRedisplay();           // Redraw the scene
    glPopMatrix();                   // Don't forget to pop the Matrix
	//glutSwapBuffers();
	glFlush();
}

/**
 * Load data with the index of the data file
 */
void openGLLoadData(int index){
	string path("data/2/distance/");
	string fileName;
	if(index<10){
		fileName.append(3,'0');
		fileName.append(1,char(index+48));
	} else if(index>=10 && index<100){
		fileName.append(2,'0');
		fileName.append(1,char((index/10)+48));
		fileName.append(1,char((index%10)+48));
	} else if(index>=100 && index<1000){
		fileName.append(1,'0');
		fileName.append(1,char((index/100)+48));
		fileName.append(1,char(((index%100)/10)+48));
		fileName.append(1,char((index%10)+48));
	} else {
		cout<<"the file name is bigger than 1000"<<endl;
		exit(0);
	}

	path.append(fileName);
	path.append(".dat");

	const char *ch = path.data();
	cout<<"Path: "<<ch<<endl;
	if(!loadData<float>(ch, ptrData, 204*204)){
		cout<<"data "<<index<<" load error!"<<endl;
		//exit(0);
	} else {
		cout<<"data "<<index<<" successful load!"<<endl;
	}

	//updata the Window
	//PostMessage(hWnd, WM_PAINT, 0, 0);	
}


/**
 * load data with a pointer of float arraz
 */
void openGLLoadData(float *d){
	ptrData = d;
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

	//get data from file
	//const char path[] = {"data/data"};
	//if(!loadData<float>(path,data, 204*204)){
	//	cout<<"data load error!"<<endl;
	//	//exit(0);
	//}
	//cout<<"load complete!"<<endl;

	//openGLLoadData(0);
	
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
	gluPerspective(45.0f, (GLfloat)iWidth/(GLfloat)iHeight, 0.5f, 120.0f);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	

}


LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){     
	static PAINTSTRUCT ps;

	static OpenGLWinUI *pOpenGLWinUI;
	static float fMousePosX;
	static float fMousePosY;
	static float fMousePosOldX;
	static float fMousePosOldY;

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
			
				fMousePosOldX = fMousePosX;
				fMousePosOldY = fMousePosY;
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
		
			if((int)wParam > 0){
				pOpenGLWinUI->fMouseWheel += 1.0f;
				rotLz += 0.3;
			} else {
				pOpenGLWinUI->fMouseWheel -= 1.0f;
				rotLz -= 0.3;
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
				//case 'n':
				//	openGLLoadData()
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

HWND CreateOpenGLWindow(char* title, int x, int y, int width, int height, 		   
					   BYTE type, DWORD flags, OpenGLWinUI *pOpenGLWinUI){    

	GLuint         PixelFormat;    
	//HDC         hDC;    
	//HWND        hWnd;    
	WNDCLASS    wc;    

	DWORD	dwExStyle;
	DWORD	dwStyle;

	RECT WindowRect;                            // Grabs Rectangle Upper Left / Lower Right Values 
	WindowRect.left=(long)0;                        // Set Left Value To 0 
	WindowRect.right=(long)width;                       // Set Right Value To Requested Width 
	WindowRect.top=(long)0;                         // Set Top Value To 0 
	WindowRect.bottom=(long)height;                     // Set Bottom Value To Requested Height 

	//static HINSTANCE hInstance = 0;    /* only register the window class once - use hInstance as a flag. */    
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
		KillGLWindow();                         // Reset The Display 
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
		KillGLWindow();                         // Reset The Display 
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
		KillGLWindow();                         // Reset The Display 
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))               // Are We Able To Set The Pixel Format? 
	{ 
		KillGLWindow();                         // Reset The Display 
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 
	
	if (!(hRC=wglCreateContext(hDC)))                   // Are We Able To Get A Rendering Context? 
	{ 
		KillGLWindow();                         // Reset The Display 
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 
	if(!wglMakeCurrent(hDC,hRC))                        // Try To Activate The Rendering Context 
	{ 
		KillGLWindow();                         // Reset The Display 
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 

	ShowWindow(hWnd,SW_SHOW);                       // Show The Window 
	SetForegroundWindow(hWnd);                      // Slightly Higher Priority 
	SetFocus(hWnd);                             // Sets Keyboard Focus To The Window 
	ReSizeGLScene(width, height);                       // Set Up Our Perspective GL Screen 

	DescribePixelFormat(hDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);    
	//ReleaseDC(hWnd, hDC);    
	return hWnd;
	//return TRUE;
}    

GLvoid KillGLWindow(GLvoid)                         // Properly Kill The Window 
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