/* An example of the minimal Win32 & OpenGL program.  It only works in   16 bit color modes or higher (since it doesn't create a   palette). */
#include <windows.h>			/* must include this before GL/gl.h */
#include <GL/gl.h>			/* OpenGL header file */
#include <GL/glu.h>			/* OpenGL utilities header file */
#include <GL/glaux.h>
#include <stdio.h>

#include "glut.h"

#include "MyDataIO.hpp"

/*
	the Struct of the conrolle information
*/
struct OpenGLWinUI
{
	float fMouseMoveLBX;

	float fMouseMoveLBY;

	float fMouseMoveRBX;

	float fMouseMoveRBY;

	float fMouseWheel;

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

void display();

int InitGL();

GLvoid ReSizeGLScene(GLsizei iWidth, GLsizei iHeight);

LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CreateOpenGLWindow(char* title, int x, int y, int width, int height, 		   
					   BYTE type, DWORD flags, OpenGLWinUI *pOpenGLWinUI);

GLvoid KillGLWindow(GLvoid); 