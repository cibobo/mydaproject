/* An example of the minimal Win32 & OpenGL program.  It only works in   16 bit color modes or higher (since it doesn't create a   palette). */
#include "stdafx.h"
#include "OpenGLWin.hpp"

using namespace std;


float data[204*204];

void display(){    

	//gluLookAt(0.0, 0.0, 1.0, 0.0,0.0,0.0, 0.0,1.0,0.0);
	//glLoadIdentity();
	
	/* rotate a triangle around */    
	glClear(GL_COLOR_BUFFER_BIT);    
/*	glBegin(GL_TRIANGLES);    
	glColor3f(1.0f, 0.0f, 0.0f);    
	glVertex3f(0,  1, -5);    
	glColor3f(0.0f, 1.0f, 0.0f);    
	glVertex3f(-1, -1, -5);    
	glColor3f(0.0f, 0.0f, 1.0f);    
	glVertex3f(1, -1, -5);  */  
	float factor = 2.04/204;
	glBegin(GL_POINTS);
	for(int i=0;i<204;i++) {
		for(int j=0;j<204;j++) {
			glVertex3f((i-102)*factor, (j-102)*factor, -data[i*204+j]);
		}
	}
			
	glEnd();    
	
	glFlush();
}


/*

 */
int InitGL()
{
	/*  */
	glShadeModel(GL_FLAT);

	/* schwarzer Hintergrund */
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	

	//get data from file
	char path[] = {"data/data.dat"};
	if(!loadData<float>(path,data, 204*204)){
		cout<<"data load error!"<<endl;
		//exit(0);
	}
	cout<<"load complete!"<<endl;

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
	gluPerspective(45.0f, (GLfloat)iWidth/(GLfloat)iHeight, 1.5f, 120.0f);


	glMatrixMode(GL_MODELVIEW);
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
		
			if((int)wParam > 0)
				pOpenGLWinUI->fMouseWheel += 1.0f;
			else
				pOpenGLWinUI->fMouseWheel -= 1.0f;

		return TRUE;						

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

	int         pf;    
	HDC         hDC;    
	HWND        hWnd;    
	WNDCLASS    wc;    
	PIXELFORMATDESCRIPTOR pfd;    
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
	hWnd = CreateWindow("OpenGL", title, WS_OVERLAPPEDWINDOW |			
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,			
		x, y, width, height, NULL, NULL, hInstance, (LPVOID)pOpenGLWinUI);    
	if (hWnd == NULL) {	
		MessageBox(NULL, "CreateWindow() failed:  Cannot create a window.",		   
			"Error", MB_OK);	
		return NULL;    
	}    
	hDC = GetDC(hWnd);    
	/* there is no guarantee that the contents of the stack that become       
	the pfd are zeroed, therefore _make sure_ to clear these bits. */    
	memset(&pfd, 0, sizeof(pfd));    
	pfd.nSize        = sizeof(pfd);    
	pfd.nVersion     = 1;    
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;    
	pfd.iPixelType   = type;    
	pfd.cColorBits   = 32;    
	pf = ChoosePixelFormat(hDC, &pfd);    
	if (pf == 0) {	
		MessageBox(NULL, "ChoosePixelFormat() failed:  "		   
			"Cannot find a suitable pixel format.", "Error", MB_OK); 	
		return 0;    
	}      
	if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {	
		MessageBox(NULL, "SetPixelFormat() failed:  "		   
			"Cannot set format specified.", "Error", MB_OK);	
		return 0;    
	}     
	DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);    
	ReleaseDC(hWnd, hDC);    
	return hWnd;
}    