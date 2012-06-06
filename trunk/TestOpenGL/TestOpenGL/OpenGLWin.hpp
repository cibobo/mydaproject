/* An example of the minimal Win32 & OpenGL program.  It only works in   16 bit color modes or higher (since it doesn't create a   palette). */

#include <windows.h>			/* must include this before GL/gl.h */
#include <GL/gl.h>			/* OpenGL header file */
#include <GL/glu.h>			/* OpenGL utilities header file */

#include <stdio.h>


#include "glut/glut.h"

static GLfloat COLORLIST[10][4] = {{1.0, 0.0, 0.0, 1.0},
								  {1.0, 1.0, 0.0, 1.0},
								  {0.0, 1.0, 1.0, 1.0},
								  {1.0, 0.0, 1.0, 1.0},
								  {1.0, 0.5, 0.0, 1.0},
								  {0.737255, 0.560784, 0.560784, 1.0},
								  {0.196078, 0.6, 0.8, 1.0},
								  {0.576471, 0.858824, 0.439216, 1.0},
								  {0.36, 0.25, 0.2, 1.0},
								  {0.8, 0.498039, 0.196078, 1.0}};

struct OpenGLContext
{
	HGLRC hRC;
	HDC hDC;
	HWND hWnd;
};

/*
	the Struct of the conrolle information
*/
struct OpenGLWinUI
{
	//float fMouseMoveLBX;

	//float fMouseMoveLBY;

	//float fMouseMoveRBX;

	//float fMouseMoveRBY;

	// Mouse Wheel
	float fMouseWheel;
	// Mouse Position
	float fMousePosX;
	float fMousePosY;
	float fMousePosOldX;
	float fMousePosOldY;

	// Declared static (no need for object reference
	float X;        // Translate screen to x direction (left or right)
	float Y;        // Translate screen to y direction (up or down)
	float Z;        // Translate screen to z direction (zoom in or out)
	float rotX;    // Rotate screen on x axis 
	float rotY;    // Rotate screen on y axis
	float rotZ;    // Rotate screen on z axis

	float rotLx;   // Translate screen by using the glulookAt function 
				   // (left or right)
	float rotLy;   // Translate screen by using the glulookAt function 
				   // (up or down)
	float rotLz;   // Translate screen by using the glulookAt function 
				   // (zoom in or out)

	// The Width and Height of the Window
	int width;
	int height;

	float gContrast;
	float gBalance;

	float aContrast;
	float aBalance;

	int colorIndex;


	//int iKeyState;

	//bool iKeyPressedB;

	//bool iKeyPressedSpace;

	//bool iKeyPressedPageUp;

	//bool iKeyPressedPageDown;

	//bool iKeyPressedReturn;

	//bool iKeyPressedNumPadUp;

	//bool iKeyPressedNumPadDown;

	//bool iKeyPressedNumPadLeft;

	//bool iKeyPressedNumPadRight;

	//bool iKeyPressedNumPadCenter;

	//bool iKeyPressedNumPadInsert;
};



void openGLLoadData(int index);

void openGLLoadData(float *disData, float *intData, float *ampData);

int InitGL();

GLvoid ReSizeGLScene(GLsizei iWidth, GLsizei iHeight);

LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool CreateOpenGLWindow(char* title, int x, int y, int width, int height, 		   
					   BYTE type, DWORD flags, OpenGLWinUI *pOpenGLWinUI, OpenGLContext *pOpenGLContext);

GLvoid KillGLWindow(HINSTANCE hInstance, HWND hWnd, HDC hDC, HGLRC hRC); 
