/* An example of the minimal Win32 & OpenGL program.  It only works in   16 bit color modes or higher (since it doesn't create a   palette). */
#include "stdafx.h"
#include "OpenGLWin.hpp"

#define DATA_SIZE 204;
using namespace std;

DWORD tlsIndex = 1001;

int InitGL()
{
	/* schwarzer Hintergrund */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glShadeModel(GL_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
    glClearDepth(1.0f);          // Set the Depth buffer value (ranges[0,1])
    glEnable(GL_DEPTH_TEST);  // Enable Depth test
    glDepthFunc(GL_LEQUAL);   // If two objects on the same coordinate 
                                            // show the first drawn
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);



	GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0};
	GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0};
	GLfloat light_specular[] = {0.5f, 0.5f, 0.5f , 1.0f};
	GLfloat light_position[] = {-1.0f, -1.0f, -10.0f, -1.0f};
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	//glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHT1);

	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	//if (iWidth <= iHeight)
 //     glOrtho (-1.5, 1.5, -1.5*(GLfloat)iHeight/(GLfloat)iWidth,
 //        1.5*(GLfloat)iHeight/(GLfloat)iWidth, -10.0, 10.0);
 //  else
 //     glOrtho (-1.5*(GLfloat)iHeight/(GLfloat)iWidth,
 //        1.5*(GLfloat)iHeight/(GLfloat)iWidth, -1.5, 1.5, -10.0, 10.0);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

}


LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){     
	PAINTSTRUCT ps;

	OpenGLWinUI *pOpenGLWinUI;
	pOpenGLWinUI = (OpenGLWinUI *)TlsGetValue(tlsIndex);
	//static float fMousePosX;
	//static float fMousePosY;
	//static float fMousePosOldX;
	//static float fMousePosOldY;

	/*
	 * isZChanged controlled by key Shift, which will be set true if the Shift key pressed and false if the key released
	 */
	static bool isZChanged = false;


	switch(uMsg) {
		case WM_CREATE:
			/*  */
			pOpenGLWinUI = (OpenGLWinUI*)((CREATESTRUCT*)lParam)->lpCreateParams;

			/* Init */
			pOpenGLWinUI->fMousePosX = 0.0f;
			pOpenGLWinUI->fMousePosY = 0.0f;
			
			pOpenGLWinUI->fMousePosOldX = 0.0f;
			pOpenGLWinUI->fMousePosOldY = 0.0f;

			pOpenGLWinUI->fMouseWheel = 0.0f;

			// Declared static (no need for object reference
			pOpenGLWinUI->X = 0.0f;        
			pOpenGLWinUI->Y = 0.0f;        
			pOpenGLWinUI->Z = 2.0f;       

			pOpenGLWinUI->rotX = 0.0f;    
			pOpenGLWinUI->rotY = 0.0f;    
			pOpenGLWinUI->rotZ = 0.0f;    

			pOpenGLWinUI->rotLx = 0.0f;                           
			pOpenGLWinUI->rotLy = 0.0f;  
			pOpenGLWinUI->rotLz = 5.0f;   

			pOpenGLWinUI->gContrast = 3200;
			pOpenGLWinUI->gBalance = 5000;

			pOpenGLWinUI->aContrast = 12;
			pOpenGLWinUI->aBalance = 250;
                                    
			pOpenGLWinUI->colorIndex = 0;

			//pOpenGLWinUI->colorList = 


		return TRUE;

		/* Controlle Message for Mouse */
		case WM_LBUTTONDOWN:
		
			pOpenGLWinUI->fMousePosOldX = GET_X_LPARAM(lParam);
			pOpenGLWinUI->fMousePosOldY = GET_Y_LPARAM(lParam);

		return TRUE;						
		

		case WM_RBUTTONDOWN:
		
			pOpenGLWinUI->fMousePosOldX = GET_X_LPARAM(lParam);
			pOpenGLWinUI->fMousePosOldY = GET_Y_LPARAM(lParam);

		return TRUE;						


		case WM_MOUSEMOVE:
		
			if(wParam == MK_LBUTTON)
			{
				pOpenGLWinUI->fMousePosX = GET_X_LPARAM(lParam);
				pOpenGLWinUI->fMousePosY = GET_Y_LPARAM(lParam);

				/*
				 * Horizontal displacement displayed the rotation around Z axis and 
				 * Vertical displacement displayed the rotation around X axis
				 */
				if(pOpenGLWinUI->rotLz!=0){
					float angleZ = (pOpenGLWinUI->fMousePosX - pOpenGLWinUI->fMousePosOldX)/pOpenGLWinUI->rotLz;
					float angleX = (pOpenGLWinUI->fMousePosY - pOpenGLWinUI->fMousePosOldY)/pOpenGLWinUI->rotLz;
					//calculate the Radian
					pOpenGLWinUI->rotZ +=angleZ/1;
					pOpenGLWinUI->rotX +=angleX/1;
					//cout<<(fMousePosY - fMousePosOldY)<<"   "<<angleZ<<endl;
				}
			
				pOpenGLWinUI->fMousePosOldX = pOpenGLWinUI->fMousePosX;
				pOpenGLWinUI->fMousePosOldY = pOpenGLWinUI->fMousePosY;

				PostMessage(hWnd, WM_PAINT, 0, 0);	
			}

			if(wParam == MK_RBUTTON)
			{
				pOpenGLWinUI->fMousePosX = GET_X_LPARAM(lParam);
				pOpenGLWinUI->fMousePosY = GET_Y_LPARAM(lParam);
			
				pOpenGLWinUI->fMousePosOldX = pOpenGLWinUI->fMousePosX;
				pOpenGLWinUI->fMousePosOldY = pOpenGLWinUI->fMousePosY;
			}
		
		
		return TRUE;						
		

		case WM_LBUTTONDBLCLK:
		
			/* Init */
			pOpenGLWinUI->fMousePosX = 0.0f;
			pOpenGLWinUI->fMousePosY = 0.0f;
			
			pOpenGLWinUI->fMousePosOldX = 0.0f;
			pOpenGLWinUI->fMousePosOldY = 0.0f;

			pOpenGLWinUI->fMouseWheel = 0.0f;

		return TRUE;						
		

		case WM_MOUSEWHEEL:
			//if SHIFT is pressed
			if(isZChanged){
				if((int)wParam > 0){
					pOpenGLWinUI->fMouseWheel += 1.0f;
					pOpenGLWinUI->Z += 0.3;
				} else {
					pOpenGLWinUI->fMouseWheel -= 1.0f;
					pOpenGLWinUI->Z -= 0.3;
				}
			} else {
				if((int)wParam > 0){
					pOpenGLWinUI->fMouseWheel += 1.0f;
					pOpenGLWinUI->rotLz += 0.3;
				} else {
					pOpenGLWinUI->fMouseWheel -= 1.0f;
					pOpenGLWinUI->rotLz -= 0.3;
				}
			}
		PostMessage(hWnd, WM_PAINT, 0, 0);	
		return TRUE;			

		case WM_KEYDOWN:
		
			//printf("\n\nKey = %d", (int)wParam);

			switch(wParam)
			{
				case 120:    // x             // Rotates screen on x axis 
                    pOpenGLWinUI->rotX -= 2.0f;
                    break;
                case 88:    // X            // Opposite way 
                    pOpenGLWinUI->rotX += 2.0f;
                    break;
                case 121:    // y            // Rotates screen on y axis
                    pOpenGLWinUI->rotY -= 2.0f;
                    break;
                case 89:    // Y            // Opposite way
                    pOpenGLWinUI->rotY += 2.0f;
                    break;
                case 122:    // z            // Rotates screen on z axis
                    pOpenGLWinUI->rotZ -= 2.0f;
                    break;
                case 90:    // Z            // Opposite way
                    pOpenGLWinUI->rotZ += 2.0f;
                    break;

				// the input value of case must be a capital letter 
				// controll key of the grayscale
				case 'B':
					pOpenGLWinUI->gBalance += 10;
					break;
				case 'V':
					pOpenGLWinUI->gBalance -= 10;
					break;
				case 'N':
					pOpenGLWinUI->gContrast += 5;
					break;
				case 'M':
					pOpenGLWinUI->gContrast -=5;
					break;

				// controll key of the amplitude
				case 'H':
					pOpenGLWinUI->aBalance += 2;
					break;
				case 'G':
					if(pOpenGLWinUI->aBalance >0){
						pOpenGLWinUI->aBalance -= 2;
					}
					break;
				case 'J':
					pOpenGLWinUI->aContrast += 1;
					break;
				case 'K':
					if(pOpenGLWinUI->aContrast > 0){
						pOpenGLWinUI->aContrast -=1;
					}
					break;

				// Change the color of the balls
				case 'C':
					if(++pOpenGLWinUI->colorIndex>=6){
						pOpenGLWinUI->colorIndex = 0;
					}
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
			BeginPaint(hWnd, &ps);	
			//display(pOpenGLWinUI);
			EndPaint(hWnd, &ps);	
			return 0;  

		case WM_SIZE:	
			ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
			pOpenGLWinUI->width = LOWORD(lParam);
			pOpenGLWinUI->height = HIWORD(lParam);
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

bool CreateOpenGLWindow(char* title, int x, int y, int iWidth, int iHeight, 		   
					   BYTE type, DWORD flags, OpenGLWinUI *pOpenGLWinUI, OpenGLContext *pOpenGLContext){    

	pOpenGLWinUI->width = iWidth;
	pOpenGLWinUI->height = iHeight;

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
	}

	/*  */
	static	PIXELFORMATDESCRIPTOR pfd =					// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),					// Size Of This Pixel Format Descriptor
		1,								// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,						// Must Support Double Buffering
		PFD_TYPE_RGBA,							// Request An RGBA Format
		32,								// Select Our Color Depth
		0, 0, 0, 0, 0, 0,						// Color Bits Ignored
		0,								// No Alpha Buffer
		0,								// Shift Bit Ignored
		0,								// No Accumulation Buffer
		0, 0, 0, 0,							// Accumulation Bits Ignored
		16,								// 16Bit Z-Buffer (Depth Buffer)
		0,								// No Stencil Buffer
		0,								// No Auxiliary Buffer
		PFD_MAIN_PLANE,							// Main Drawing Layer
		0,								// Reserved
		0, 0, 0								// Layer Masks Ignored
	};
 
	if (!(hDC=GetDC(hWnd)))                         // Did We Get A Device Context? 
	{ 
		KillGLWindow(hInstance, hWnd, hDC, hRC);                         // Reset The Display 
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE;                           // Return FALSE 
	} 

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

	if(!InitGL())
	{
		KillGLWindow(hInstance, hWnd, hDC, hRC);                         // Reset The Display 
		MessageBox(NULL,"Initialization Failed!","ERROR",MB_OK|MB_ICONEXCLAMATION); 
		return FALSE; 
	}

	if(pOpenGLContext != NULL)
	{
		pOpenGLContext->hWnd = hWnd;
		pOpenGLContext->hRC = hRC;
		pOpenGLContext->hDC = hDC;
	}

	//DescribePixelFormat(hDC, PixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);    
	//ReleaseDC(hWnd, hDC);    
	//return hWnd;
	return TRUE;
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