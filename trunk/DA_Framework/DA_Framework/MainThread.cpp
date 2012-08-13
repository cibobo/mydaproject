#include "stdafx.h"
#include "MainThread.hpp"

MainThread::MainThread(){
	ThreadIndex = 1001;

	p3DDataViewContext = new OpenGLContext;

	bDone = false;
	isPause = false;
	isDataUsed = false;

	associaterate = 0.02;

	DETECTINGRATE = 2;
	MAXJUMPEDFEATURES = 5;
	jumpedFeatures = 0;

	FRAMERATE = 30;

	//INPUTPATH = "Eva2Boxes";
	INPUTPATH = new char[150];
	OUTPUTPATH = "Eva2Boxes";
	ISDATASAVED = false;

	EVAOUTPUTPATH = "FirstTest";

	MODELNAME = "box2_all2";

	DISTANCEFILTER_EPS = 0.1;
	DISTANCEFILTER_DIFFRATE = 0.08;

	InitializeCriticalSection (&pauseCrs);
	InitializeCriticalSection (&frameCrs);
	InitializeCriticalSection (&glInitCrs);
	InitializeCriticalSection (&cvInitCrs);
	InitializeCriticalSection (&calcCrs);

}

MainThread::~MainThread(){
	delete p3DDataViewContext;
}

/**
 * creat the static function to start a thread. See:
 * http://stackoverflow.com/questions/1372967/how-do-you-use-createthread-for-functions-which-are-class-members
 **/
DWORD WINAPI MainThread::beginInputThread(void *param){
	MainThread *This = (MainThread*)param;
	return This->offlineInputThreadProc();
}

DWORD WINAPI MainThread::beginOpenGLSceneThread(void *param){
	MainThread *This = (MainThread*)param;
	return This->openGLSceneThreadPorc();
}

DWORD MainThread::offlineInputThreadProc(void){
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);
	setDefaultLoadPath(INPUTPATH);

	//get the distance data for the first step
	BildData *temp = new BildData();
	loadAllDataFromFile(3, temp);
	//init a distance filter
	dFilter = new DistanceFilter(temp, DISTANCEFILTER_EPS, DISTANCEFILTER_DIFFRATE);

	cout<<"Upgrading Distance Filter and Save Data into Buffer....."<<endl;
	for(int i=1;i<20;i++){
		frameIndex = i;
		//loadNormalDataFromFile("distance", i, bildData->disData);
		BildData *temp = new BildData();
		loadAllDataFromFile(i, temp);
		if(bildDataBuffer.size()>=DETECTINGRATE){
			//delete the first element from the list
			bildDataBuffer.pop_front();
		}
		//add the new element into the end of the List
		bildDataBuffer.push_back(temp);

		//dFilter->Upgrade(temp->disData);
		dFilter->Upgrade(temp);
	}
	cout<<"Upgrading complete!"<<endl;
	
	LeaveCriticalSection (&cvInitCrs);
	LeaveCriticalSection (&glInitCrs);

	for(int i=20;i<999;i++){
		cout<<endl<<"============================= "<<i<<" ==========================="<<endl;
		frameIndex = i;
		EnterCriticalSection(&pauseCrs);
		EnterCriticalSection(&frameCrs);

		BildData *temp = new BildData();
		loadAllDataFromFile(i, temp);

		// Pop the first element of the Buffer, and the destructor will be called automatically 
		// if sepcial process begin, the old data would not be removed
		if(jumpedFeatures<1){
			bildDataBuffer.pop_front();
		}
		bildDataBuffer.push_back(temp);

		//updata the OpenGL Window
		//PostMessage(openGLhnd, WM_PAINT, 0, 0);	
		PostMessage(p3DDataViewContext->hWnd, WM_PAINT, 0, 0);	
		isDataUsed = false;

		cout<<"Input running!"<<endl;
		LeaveCriticalSection(&frameCrs);
		LeaveCriticalSection(&pauseCrs);
		Sleep(FRAMERATE);
	}
	return 0;
}

DWORD MainThread::openGLSceneThreadPorc(void) 
{ 
	static OpenGLWinUI *pOpenGLWinUI = new OpenGLWinUI;
	//p3DDataViewContext = new OpenGLContext;

	TlsSetValue(ThreadIndex, pOpenGLWinUI);

#ifdef RECOGNITION
	int width = 600;
	int height = 600;
#else
	int width = 900;
	int height = 600;
#endif

	EnterCriticalSection (&glInitCrs);

	if(!CreateOpenGLWindow("OpenGL Scene Window", 0, 0, width, height, PFD_TYPE_RGBA, 0, pOpenGLWinUI, p3DDataViewContext)){
		exit(0);
	}

	cout<<"Thread OpenGL running"<<endl;

	LeaveCriticalSection (&glInitCrs);
	
	// if the main process is not stop
	while(!bDone){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
			if(msg.message==WM_QUIT){
				bDone = TRUE;
			} else if(msg.message==WM_RBUTTONDOWN){
				// klick right button of mouse to stop and rerun the input of frame
				if(isPause){
					cout<<"frame running!"<<endl;
					LeaveCriticalSection(&pauseCrs);
					isPause = false;
				} else {
					cout<<"frame pause!"<<endl;
					EnterCriticalSection(&pauseCrs);
					isPause = true;
				}
			} else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		} else {
			//display(pOpenGLWinUI, bildDataBuffer[0]);
			EnterCriticalSection (&calcCrs);
#ifdef RECOGNITION
			displayRecog(pOpenGLWinUI, recognition->graphList);
			glEnable(GL_LIGHTING);
#else
			display(pOpenGLWinUI, bildDataBuffer.back());
#endif
			SwapBuffers(p3DDataViewContext->hDC);
			LeaveCriticalSection (&calcCrs);
		}
	}

	//delete p3DDataViewContext;
	delete pOpenGLWinUI;

	return 0;
} 