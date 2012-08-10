#include "stdafx.h"
#include "FrameworkMain.hpp"

DWORD ThreadIndex = 1001;

// for turn off or turn on the input data
CRITICAL_SECTION pauseCrs;
// for OpenGL Window
CRITICAL_SECTION glInitCrs;
// for all OpenCV Windows
CRITICAL_SECTION cvInitCrs;
// for the frame controll, which should be completely executed before the input of a new data 
CRITICAL_SECTION frameCrs;
// for the calculation process. 
CRITICAL_SECTION calcCrs;

// Windows message to controll the Showing windows
MSG msg;
// To mark the status of the OpenGLWindow and ObjectWindow
bool bDone = false;
// To mark the status of the input process
bool isPause = false;
// To restrict the used time of the new data
bool isDataUsed = false;

// The parameter of the points association, which deside by the last frame
float associaterate = 0.02;

// The Context for 3D data Window
OpenGLContext *p3DDataViewContext;
// The Context for Objekt structur Window
OpenGLContext *pObjViewContext;



// The Buffer for BildDatas, which is saving the newest BildData at last place and the oldest at the first place.
list<BildData*> bildDataBuffer;

// The difference between the index of the current frame and historical frame
int DETECTINGRATE = 2;
// The maximal number of allowed jumping frames 
int MAXJUMPEDFEATURES = 5;
// To save the number of jumped frames
int jumpedFeatures = 0;
// The current index of frame
int frameIndex;


// framerate
int FRAMERATE = 300;

// The input path
const char *INPUTPATH = "Eva2Boxes";
// The output path for the Online Data source
char *OUTPUTPATH = "Eva2Boxes";

bool ISDATASAVED = false;

// Evaluation output path
char *EVAOUTPUTPATH = "FirstTest";

// The vtk file name
char *MODELNAME = "box2_all2";

// The Distance Filter
DistanceFilter *dFilter;
// The two paramter of Distance Filter
float DISTANCEFILTER_EPS = 0.1;
float DISTANCEFILTER_DIFFRATE = 0.08;

//Object *obj;
//Recognition *recognition;



void FrameworkMain(){
	InitializeCriticalSection (&pauseCrs);
	InitializeCriticalSection (&frameCrs);
	InitializeCriticalSection (&glInitCrs);
	InitializeCriticalSection (&cvInitCrs);
	InitializeCriticalSection (&calcCrs);

	if(_beginthread (offlineInputThreadProc, 0, NULL) == -1){
		cout<<"Failed to create input thread"<<endl;
	}


	if(_beginthread (openGLSceneThreadPorc, 0, NULL) == -1){
		cout<<"Failed to create input thread"<<endl;
	}
	
	//while(1){
	//}
}


void offlineInputThreadProc(void *param){
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
}

void openGLSceneThreadPorc( void *param ) 
{ 
	static OpenGLWinUI *pOpenGLWinUI = new OpenGLWinUI;
	p3DDataViewContext = new OpenGLContext;

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

	delete p3DDataViewContext;
	delete pOpenGLWinUI;
} 