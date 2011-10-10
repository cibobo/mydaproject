// TestOpenGL.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "TestOpenGL.h"




#ifndef _MT 
#define _MT 
#endif 

const int DATA_SIZE = 204*204;

// Das einzige Anwendungsobjekt

CWinApp theApp;

using namespace std;

CRITICAL_SECTION crs;

// global flag 
bool bDone = false;
// The handle for OpenGL Window
HWND openGLhnd;	
// The handle for Grayscale Window
HWND grayScalehnd;

//TODO: direct pointer define
// Array and the Pointer of Distance Data
float dData[DATA_SIZE];
float *disData;// = dData;

// Array and the Pointer of Intensity Data
float iData[DATA_SIZE];
float *intData;



// this function is called by a new thread 
void openGLThreadPorc( void *param ) 
{ 
	MSG msg;
	//BOOL done = FALSE;
	static OpenGLWinUI *pOpenGLWinUI = new OpenGLWinUI;

	EnterCriticalSection (&crs);

	//if(!CreateOpenGLWindow("minimal", 0, 0, 800, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI)){
	if((openGLhnd=CreateOpenGLWindow("OpenGL Window", 0, 0, 900, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI))==NULL){
		exit(0);
	}

	cout<<"Thread OpenGL running"<<endl;

	LeaveCriticalSection (&crs);




	while(!bDone){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
		//if(GetMessage(&msg, openGLhnd, 0, 0)){
			if(msg.message==WM_QUIT){
				bDone = TRUE;
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
			
	}

		//SetTimer(hWnd,101,50,NULL);


	//KillGLWindow();


		//HDC hDC;				/* device context */    
		//HGLRC hRC;				/* opengl context */    
		//HWND  hWnd;				/* window */    
		//MSG   msg;				/* message */

		//hWnd = CreateOpenGLWindow("minimal", 0, 0, 256, 256, PFD_TYPE_RGBA, 0, NULL);    
		//if (hWnd == NULL)	
		//	exit(1);    
		//hDC = GetDC(hWnd);    
		//hRC = wglCreateContext(hDC);    
		//wglMakeCurrent(hDC, hRC);    
		//ShowWindow(hWnd, SW_SHOW);    
		//while(GetMessage(&msg, hWnd, 0, 0)) {	
		//	TranslateMessage(&msg);	
		//	DispatchMessage(&msg);    
		//}    
		//wglMakeCurrent(NULL, NULL);    
		//ReleaseDC(hWnd, hDC);    
		//wglDeleteContext(hRC);    
		//DestroyWindow(hWnd); 
} 

void inputThreadProc(void *param){
	//EnterCriticalSection (&crs);
	//cout<<"input thread running!"<<endl;
	//Sleep(3000);
	//LeaveCriticalSection (&crs);

	for(int i=0;i<400;i++){
		loadNormalDataFromFile("distance", i, disData);
		loadNormalDataFromFile("intensity", i, intData);
		openGLLoadData(disData, intData);
		//updata the OpenGL Window
		PostMessage(openGLhnd, WM_PAINT, 0, 0);	
		Sleep(100);
	}

	//EnterCriticalSection (&crs);
	//if(!createPMDCon()){
	//	//exit(1);
	//	cout<<"input thread running!"<<endl;
	//	
	//}
	//Sleep(3000);
	////cout<<"input thread running!!!!"<<endl;
	//LeaveCriticalSection (&crs);

	//while(!bDone){
	//	hdrImage();
	//	disData = getPMDDataPointer();
	//	
	//	openGLLoadData(disData);
	//	
	//	PostMessage(openGLhnd, WM_PAINT, 0, 0);
	//	cout<<"Input Process with PMD Cam is running!"<<endl;
	//	Sleep(100);
	//}
	//closePMDCon();

}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	//just for the situation, that more than one threads are calling the same data at the same time
	InitializeCriticalSection (&crs);

	// MFC initialisieren und drucken. Bei Fehlschlag Fehlermeldung aufrufen.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: Den Fehlercode an Ihre Anforderungen anpassen.
		_tprintf(_T("Schwerwiegender Fehler bei der MFC-Initialisierung\n"));
		nRetCode = 1;
	}
	else
	{
		// create a new array for distance data
		disData = new float[DATA_SIZE];

		// create a new array for intensity data
		intData = new float[DATA_SIZE];

		//Start Input Thread
		if(_beginthread (inputThreadProc, 0, NULL) == -1){
			cout<<"Failed to create input thread"<<endl;
		}

		// Start OpenGL Window Thread 
		if(_beginthread (openGLThreadPorc, 0, NULL)==-1){
			cout<<"Failed to create openGL thread"<<endl;
		}

		//static OpenGLWinUI *pOpenGLWinUI2 = new OpenGLWinUI;
		//if((grayScalehnd=CreateOpenGLWindow("Garyscale Window", 0, 0, 400, 300, PFD_TYPE_RGBA, 0, pOpenGLWinUI2))==NULL){
		//	exit(0);
		//}
		//
		//MSG msg;
		//while(!bDone){
		//	if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
		//	//if(GetMessage(&msg, grayScalehnd, 0, 0)){
		//		if(msg.message==WM_QUIT){
		//			bDone = TRUE;
		//		} else {
		//			TranslateMessage(&msg);
		//			DispatchMessage(&msg);
		//		}
		//	}
		//		
		//}

		//Main Thread
		while (!bDone ) 
		{ 
			//EnterCriticalSection (&crs);
			// sleep 3 seonds 
			::Sleep(3000); 
			printf("main thread running\n"); 
			//LeaveCriticalSection (&crs);
		} 

		//Sleep(3000);
		//KillGLWindow();
		
		// release the memory block of the data
		delete [] disData;
		delete [] intData;
		
	}

	return nRetCode;
}
