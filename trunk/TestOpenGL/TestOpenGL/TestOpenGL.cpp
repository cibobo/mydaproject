// TestOpenGL.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "TestOpenGL.h"




#ifndef _MT 
#define _MT 
#endif 

// Das einzige Anwendungsobjekt

CWinApp theApp;

using namespace std;

CRITICAL_SECTION crs;

// global flag 
bool bDone = false;
// The handle for OpenGL Windows
HWND  openGLhnd;				

float mData[204*204];
float *mainData = mData;

// this function is called by a new thread 
void openGLThreadPorc( void *param ) 
{ 
	MSG msg;
	BOOL done = FALSE;

	static OpenGLWinUI *pOpenGLWinUI = new OpenGLWinUI;
	//if(!CreateOpenGLWindow("minimal", 0, 0, 800, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI)){
	if((openGLhnd=CreateOpenGLWindow("minimal", 0, 0, 800, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI))==NULL){
		exit(0);
	}

	cout<<"Thread OpenGL running"<<endl;

	//EnterCriticalSection (&crs);
	while(!bDone){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			if(msg.message==WM_QUIT){
				bDone = TRUE;
			} else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
			
	}
	//LeaveCriticalSection (&crs);

	KillGLWindow();


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

	//for(int i=0;i<532;i+=2){
	//	loadNormalDataFromFile(i, mainData);
	//	openGLLoadData(mainData);
	//	//updata the OpenGL Window
	//	PostMessage(openGLhnd, WM_PAINT, 0, 0);	
	//	Sleep(100);
	//}

	EnterCriticalSection (&crs);
	if(!createPMDCon()){
		exit(1);
	}
	LeaveCriticalSection (&crs);
	while(!bDone){
		hdrImage();
		mainData = getPMDDataPointer();
		
		openGLLoadData(mainData);
		
		PostMessage(openGLhnd, WM_PAINT, 0, 0);
		cout<<"Input Process with PMD Cam is running!"<<endl;
		Sleep(100);
	}
	closePMDCon();

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
		//if(mainData == NULL){
			//mainData = new float[204*204];
		//}

		//Start Input Thread
		if(_beginthread (inputThreadProc, 0, NULL) == -1){
			cout<<"Failed to create input thread"<<endl;
		}

		// Start OpenGL Window Thread 
		if(_beginthread (openGLThreadPorc, 0, NULL)==-1){
			cout<<"Failed to create openGL thread"<<endl;
		}

		//createDefaultPMDDataDirectory();
		//char* dir = "data/4";
		//cout<<isDirectoryExist(dir)<<endl;
		//if(!isDirectoryExist(dir)){
		//	CreateDirectory(dir, NULL);
		//	CreateDirectory("data/4/distance",NULL);
		//}
		
		
	

//			if(!createPMDCon()){
//		exit(1);
//	}
//cout<<"successful create"<<endl;
//		hdrImage();
//
//		cout<<"Input Process with PMD Cam is running!"<<endl;
//	closePMDCon();
		
		
		//Main Thread
		while (!bDone ) 
		{ 
			//EnterCriticalSection (&crs);
			// sleep 3 seonds 
			::Sleep(3000); 
			printf("main thread running\n"); 
			//LeaveCriticalSection (&crs);
		} 
		
		
	}

	//delete[] mainData;

	return nRetCode;
}
