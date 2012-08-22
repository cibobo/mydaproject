#include "process.h" 
#include "resource.h"
#include "stdio.h" 
#include "windows.h" 
#include "string"

#include <iostream>
#include <list>

//#include "BildData.hpp"
#include "DistanceFilter.hpp"
//#include "ImageProcess.hpp"
#include "Learning.hpp"
#include "MyDataIO.hpp"
#include "MyFeatureDetector.hpp"
#include "OpenGLDraw.hpp"
#include "Parameters.hpp"

using namespace std;

class MainThread{
public:
	MainThread();
	~MainThread();

	// Instance of Parameters
	Parameters *pParameters;

	// The Distance Filter
	DistanceFilter *pDFilter;

	// Instance of Detector
	MyFeatureDetector *pDetector;

	// Instance of Learning
	Learning *pLearning;

	//// Instance of ImageProcess
	//ImageProcess *pImageProcess;

	// Announcement of subthreads
	DWORD offlineInputThreadProc(void);
	DWORD openGLSceneThreadProc(void);
	DWORD calculationThreadProc(void);
	DWORD openCVHelpThreadProc(void);

	// Static functions to satart the threads
	static DWORD WINAPI beginInputThread(void *param);
	static DWORD WINAPI beginOpenGLSceneThread(void *param);
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
	// for the depth filter init
	CRITICAL_SECTION filterInitCrs;

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



	// The Buffer for BildDatas, which is saving the newest BildData at last place and the oldest at the first place.
	list<BildData*> bildDataBuffer;

	// The difference between the index of the current frame and historical frame
	int DETECTINGRATE;
	// The maximal number of allowed jumping frames 
	int MAXJUMPEDFEATURES;
	// To save the number of jumped frames
	int jumpedFeatures;
	// The current index of frame
	int frameIndex;


	// framerate
	int FRAMERATE;

	// The Uniqueness controlled by the attributes of Radio Buttons
	// whether the Online mode is selected
	BOOL isOnline;
	// whether the Offline mode is selected
	BOOL isOffline;
	// The input data stream path for the offline mode
	char *INPUTPATH;
	// The output path for the Online Data source
	char *OUTPUTPATH;
	// Just for the Online Mode: whether save the data in local drive
	bool ISDATASAVED;

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
};