/* An example of the minimal Win32 & OpenGL program.  It only works in   16 bit color modes or higher (since it doesn't create a   palette). */
#include <windows.h>			/* must include this before GL/gl.h */
#include <GL/gl.h>			/* OpenGL header file */
#include <GL/glu.h>			/* OpenGL utilities header file */
#include <GL/glaux.h>
#include <stdio.h>

#include "glut/glut.h"

#include "BildData.hpp"
#include "MyDataIO.hpp"

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

void display(OpenGLWinUI *pOpenGLWinUI);

void display(OpenGLWinUI *pOpenGLWinUI, float *disData, float *intData, float *ampData);

void display(OpenGLWinUI *pOpenGLWinUI, BildData *bildData);    

void openGLLoadData(int index);

void openGLLoadData(float *disData, float *intData, float *ampData);

int InitGL();

GLvoid ReSizeGLScene(GLsizei iWidth, GLsizei iHeight);

LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND CreateOpenGLWindow(char* title, int x, int y, int width, int height, 		   
					   BYTE type, DWORD flags, OpenGLWinUI *pOpenGLWinUI);

GLvoid KillGLWindow(HINSTANCE hInstance, HWND hWnd, HDC hDC, HGLRC hRC); 