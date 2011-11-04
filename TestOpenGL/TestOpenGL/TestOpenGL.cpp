// TestOpenGL.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "TestOpenGL.h"


#define OFFLINE

#ifndef _MT 
#define _MT 
#endif 

const int DATA_SIZE = 204*204;

DWORD ThreadIndex = 1001;

using namespace std;

CRITICAL_SECTION crs;
CRITICAL_SECTION frameCrs;

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

// Array and the Pointer of Amplitude Data
float aData[DATA_SIZE];
float *ampData;


// this function is called by a new thread 
void openGLThreadPorc( void *param ) 
{ 
	MSG msg;
	//BOOL done = FALSE;
	static OpenGLWinUI *pOpenGLWinUI = new OpenGLWinUI;

	TlsSetValue(ThreadIndex, pOpenGLWinUI);

	EnterCriticalSection (&crs);

	//if(!CreateOpenGLWindow("minimal", 0, 0, 800, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI)){
	if((openGLhnd=CreateOpenGLWindow("OpenGL Window", 0, 0, 900, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI))==NULL){
		exit(0);
	}

	cout<<"Thread OpenGL running"<<endl;

	LeaveCriticalSection (&crs);

	// call the display function and send the data direct with the pointer of array as parameter
	display(pOpenGLWinUI, disData, intData, ampData);

	//parameter to mark, whether the input process is in pause
	bool isPause = false;

	while(!bDone){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
		//if(GetMessage(&msg, openGLhnd, 0, 0)){
			if(msg.message==WM_QUIT){
				bDone = TRUE;
			} else if(msg.message==WM_RBUTTONDOWN){
				// klick right button of mouse to stop and rerun the input of frame
				if(isPause){
					cout<<"frame running!"<<endl;
					LeaveCriticalSection(&frameCrs);
					isPause = false;
				} else {
					cout<<"frame pause!"<<endl;
					EnterCriticalSection(&frameCrs);
					isPause = true;
				}
			} else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				//display(pOpenGLWinUI, disData, intData, ampData);
			}
		}
		display(pOpenGLWinUI, disData, intData, ampData);
			
	}

	delete pOpenGLWinUI;
} 


void arToolKitThreadProc(void *param){
	MSG msg;
	static OpenGLWinUI *pARToolKitUI = new OpenGLWinUI;

	TlsSetValue(ThreadIndex, pARToolKitUI);

	//EnterCriticalSection (&crs);

	////if(!CreateOpenGLWindow("minimal", 0, 0, 800, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI)){
	//if((openGLhnd=CreateOpenGLWindow("ARToolKit", 500, 0, 204, 204, PFD_TYPE_RGBA, 0, pARToolKitUI))==NULL){
	//	exit(0);
	//}

	//cout<<"Thread OpenGL running"<<endl;

	//LeaveCriticalSection (&crs);

	ARParam  wparam;
	ARParam  cparam;
	char     *cparam_name    = "data/AR/camera_para.dat";
	char     *patt_name      = "data/AR/patt.hiro";
	int		  patt_id;

	//* set the initial camera parameters */
	if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
		printf("Camera parameter load error !!\n");
		exit(0);
	}

	arParamChangeSize( &wparam, 204, 204, &cparam );
	arInitCparam( &cparam );
	printf("*** Camera Parameter ***\n");
	arParamDisp( &cparam );

	if( (patt_id=arLoadPatt(patt_name)) < 0 ) {
		printf("pattern load error !!\n");
		exit(0);
	}

	argInit( &cparam, 1.0, 0, 0, 0, 0 );

	EnterCriticalSection (&crs);

	arVideoCapStart();
	argMainLoop( NULL, keyEvent, mainLoop );

	LeaveCriticalSection (&crs);


	// call the display function and send the data direct with the pointer of array as parameter
	//draw2(intData, patt_id);
	////mainLoop();
	////draw();

	////parameter to mark, whether the input process is in pause
	//bool isPause = false;

	//while(!bDone){
	//	if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
	//	//if(GetMessage(&msg, openGLhnd, 0, 0)){
	//		if(msg.message==WM_QUIT){
	//			bDone = TRUE;
	//		} else{
	//			TranslateMessage(&msg);
	//			DispatchMessage(&msg);
	//			draw2(intData, patt_id);
	//			//mainLoop();
	//			//draw();
	//		}
	//	}
	//	
	//		
	//}

	delete pARToolKitUI;
}

void inputThreadProc(void *param){
	//EnterCriticalSection (&crs);
	//cout<<"input thread running!"<<endl;
	//Sleep(3000);
	//LeaveCriticalSection (&crs);

#ifdef OFFLINE
	setDefaultLoadPath("translate");
	for(int i=0;i<349;i++){
		EnterCriticalSection(&frameCrs);
		loadNormalDataFromFile("distance", i, disData);
		loadNormalDataFromFile("intensity", i, intData);
		loadNormalDataFromFile("amplitude", i, ampData);
		//openGLLoadData(disData, intData, ampData);	
		//updata the OpenGL Window
		PostMessage(openGLhnd, WM_PAINT, 0, 0);	
		setARData(intData);
		LeaveCriticalSection(&frameCrs);
		Sleep(100);
	}

#else
	EnterCriticalSection (&crs);
	createDefaultPMDDataDirectory();
	setIsDataSaved(true);
	if(!createPMDCon()){
		exit(1);
		//cout<<"input thread running!"<<endl;
		
	}
	//Sleep(3000);
	//cout<<"input thread running!!!!"<<endl;
	LeaveCriticalSection (&crs);

	while(!bDone){
		EnterCriticalSection(&frameCrs);
		getPMDData(disData, intData, ampData);
		setARData(intData);
		//disData = getPMDDataPointer();
		
		//openGLLoadData(disData, intData, ampData);
		
		//PostMessage(openGLhnd, WM_PAINT, 0, 0);
		//cout<<"Input Process with PMD Cam is running!"<<endl;
		LeaveCriticalSection(&frameCrs);
		Sleep(100);
	}
	closePMDCon();
#endif
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	//just for the situation, that more than one threads are calling the same data at the same time
	InitializeCriticalSection (&crs);
	InitializeCriticalSection (&frameCrs);

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

		// create a new array for amplitude data
		ampData = new float[DATA_SIZE];

		//Start Input Thread
		if(_beginthread (inputThreadProc, 0, NULL) == -1){
			cout<<"Failed to create input thread"<<endl;
		}

		// Start OpenGL Window Thread 
		//if(_beginthread (openGLThreadPorc, 0, NULL)==-1){
		//	cout<<"Failed to create openGL thread"<<endl;
		//}

		// Start ARToolKit Window Thread 
		if(_beginthread (arToolKitThreadProc, 0, NULL)==-1){
			cout<<"Failed to create ARToolKit thread"<<endl;
		}

		//Main Thread
		while (!bDone ) 
		{ 
			//EnterCriticalSection (&crs);
			// sleep 3 seonds 
			::Sleep(3000);

			printf("main thread running\n"); 
			//LeaveCriticalSection (&crs);
		} 
		
		// release the memory block of the data
		delete [] disData;
		delete [] intData;
		delete [] ampData;
		
	}

	return nRetCode;
}

