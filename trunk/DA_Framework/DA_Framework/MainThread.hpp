#include "process.h" 
#include "resource.h"
#include "stdio.h" 
#include "windows.h" 
#include "string"

#include <iostream>
#include <list>

//#include "BildData.hpp"
#include "Evaluator.hpp"
#include "DistanceFilter.hpp"
#include "FrameIterator.hpp"
//#include "ImageProcess.hpp"
//#include "Learning.hpp"
#include "MyDataIO.hpp"
#include "MyFeatureDetector.hpp"
#include "OpenGLDraw.hpp"
#include "PMDCamIO.hpp"
#include "Recognition.hpp"

using namespace std;

//Whether the Evaluator is active
#define EVALUATION

#ifdef EVALUATION

//#define EVA_DFILTER
#ifdef EVA_DFILTER
	#define NO_DFILTER
#endif

//#define EVA_CBRIGHTNESS

//#define EVA_ASSOCIATION

//#define EVA_FRAMECONTROL

//#define EVA_GRAPHUPDATE

//#define EVA_SAVECVBILD
#ifdef EVA_SAVECVBILD
	#define FRAME_INTERVAL 30
#endif

#define SCREENSHOT
#ifdef SCREENSHOT
	#define SCREENSHOT_FRAME 180
#endif

//#define EVA_TIME

//#define EVA_RECOGNITION

#endif


class MainThread{
public:
	MainThread();
	~MainThread();

	// The Distance Filter
	DistanceFilter *pDFilter;

	// Pointer of Instance of Detector
	MyFeatureDetector *pDetector;

	// Pointer of Instance of Learning
	Learning *pLearning;

	// Pointer of Instance of FrameIterator
	FrameIterator *pIterator;

	// Pointer of Instance of Recognition
	Recognition *pRecognition;

	// Pointer of Instance of PMDCamera IO 
	PMDCamIO *pPMDCamIO;

	// Pointer of Logger
	//Evaluator *pLogger;

	// Announcement of subthreads
	DWORD offlineInputThreadProc(void);
	DWORD onlineInputThreadProc(void);
	DWORD openGLSceneThreadProc(void);
	DWORD openGLResultThreadProc(void);
	DWORD calculationThreadProc(void);
	DWORD openCVHelpThreadProc(void);

	// Static functions to satart the threads
	static DWORD WINAPI beginOfflineInputThread(void *param);
	static DWORD WINAPI beginOnlineInputThread(void *param);
	static DWORD WINAPI beginOpenGLSceneThread(void *param);
	static DWORD WINAPI beginOpenGLResultThread(void *param);
	static DWORD WINAPI beginCalculationThread(void *param);
	// Thread for all OpenCV help windows
	static DWORD WINAPI beginOpenCVHelpThread(void *param);

	void run();


	DWORD ThreadIndex;

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

	// The Uniqueness controlled by the attributes of Radio Buttons
	// Is learning mode
	BOOL isLearning;
	// Is recognition mode
	BOOL isRecognise;

	// Windows message to controll the Showing windows
	MSG msg;
	// To mark the status of the OpenGLWindow and ObjectWindow
	bool bDone;
	// To mark the status of the input process
	bool isPause;
	// To restrict the used time of the new data
	bool isDataUsed;

	// The parameter of the points association, which deside by the last frame
	float associaterate;

	// The Context for 3D data Window
	OpenGLContext *p3DDataViewContext;
	// The Context for Objekt structur Window
	OpenGLContext *pObjViewContext;

	// framerate
	int FRAMERATE;

	// The Uniqueness controlled by the attributes of Radio Buttons
	// whether the Online mode is selected
	BOOL isOnline;
	// whether the Offline mode is selected
	BOOL isOffline;
	// The input data stream path for the offline mode
	char *INPUTPATH;

	// Evaluation output path
	char *EVAOUTPUTPATH;

	// The vtk file name
	char *MODELNAME;

	//Object *obj;
	//Recognition *recognition;

	// Whether the real-time Observing Window is visible
	BOOL isObservingWindowVisible;
	// Whether the Result Window is visible
	BOOL isResultWindowVisible;
	// Whether the OpenCV help Window is visible
	BOOL isOpenCVWindowVisible;

	// Logger name
	//const char *loggerName = "Log";
};