// TestOpenGL.cpp : Definiert den Einstiegspunkt f�r die Konsolenanwendung.
//

#include "stdafx.h"
#include "TestOpenGL.h"
#include "OpenGLWin.hpp"

#include "stdio.h" 
#include "windows.h" 
#include "process.h" 
#include "resource.h" 



#ifndef _MT 
#define _MT 
#endif 




// Das einzige Anwendungsobjekt

CWinApp theApp;

using namespace std;


// global flag 
bool bDone = false; 

// this function is called by a new thread 
void InputThreadProc( void *dummy ) 
{ 
		HDC hDC;				/* device context */    
		HGLRC hRC;				/* opengl context */    
		HWND  hWnd;				/* window */    
		MSG   msg;				/* message */

		hWnd = CreateOpenGLWindow("minimal", 0, 0, 256, 256, PFD_TYPE_RGBA, 0);    
		if (hWnd == NULL)	
			exit(1);    
		hDC = GetDC(hWnd);    
		hRC = wglCreateContext(hDC);    
		wglMakeCurrent(hDC, hRC);    
		ShowWindow(hWnd, SW_SHOW);    
		while(GetMessage(&msg, hWnd, 0, 0)) {	
			TranslateMessage(&msg);	
			DispatchMessage(&msg);    
		}    
		wglMakeCurrent(NULL, NULL);    
		ReleaseDC(hWnd, hDC);    
		wglDeleteContext(hRC);    
		DestroyWindow(hWnd); 
} 




int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// MFC initialisieren und drucken. Bei Fehlschlag Fehlermeldung aufrufen.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: Den Fehlercode an Ihre Anforderungen anpassen.
		_tprintf(_T("Schwerwiegender Fehler bei der MFC-Initialisierung\n"));
		nRetCode = 1;
	}
	else
	{

		HDC hDC;				/* device context */    
		HGLRC hRC;				/* opengl context */    
		HWND  hWnd;				/* window */    
		MSG   msg;				/* message */

		hWnd = CreateOpenGLWindow("minimal", 0, 0, 256, 256, PFD_TYPE_RGBA, 0);    
		if (hWnd == NULL)	
			exit(1);    
		hDC = GetDC(hWnd);    
		hRC = wglCreateContext(hDC);    
		wglMakeCurrent(hDC, hRC);    
		ShowWindow(hWnd, SW_SHOW);    
		while(GetMessage(&msg, hWnd, 0, 0)) {	
			TranslateMessage(&msg);	
			DispatchMessage(&msg);    
		}    
		wglMakeCurrent(NULL, NULL);    
		ReleaseDC(hWnd, hDC);    
		wglDeleteContext(hRC);    
		DestroyWindow(hWnd);    
		return msg.wParam;
		
		
		//printf("Hello, world of console apps\n"); 
		//// create a new thread to allow user input 
		//if( _beginthread(InputThreadProc, 0, NULL )==-1) 
		//{ 
		//	printf("Failed to create thread"); 
		//	return 0; 
		//} 
		//// wait for the new thread to finish 
		//while ( !bDone ) 
		//{ 
		//	// sleep 3 seonds 
		//	::Sleep(3000); 
		//	printf("main thread running\n"); 
		//} 

		
	}

	return nRetCode;
}
