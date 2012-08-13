#include "process.h" 
#include "resource.h"
#include "stdio.h" 
#include "windows.h" 
#include "string"

#include <iostream>
#include <list>

#include "BildData.hpp"
#include "DistanceFilter.hpp"
#include "MyDataIO.hpp"
#include "OpenGLDraw.hpp"

using namespace std;

class MainThread{
public:
	MainThread();
	~MainThread();

	// Announcement of subthreads
	DWORD offlineInputThreadProc(void);
	DWORD openGLSceneThreadPorc(void);

	// Static functions to satart the threads
	static DWORD WINAPI beginInputThread(void *param);
	static DWORD WINAPI beginOpenGLSceneThread(void *param);


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

	// The input data stream path for the offline mode
	char *INPUTPATH;
	// The output path for the Online Data source
	char *OUTPUTPATH;

	bool ISDATASAVED;

	// Evaluation output path
	char *EVAOUTPUTPATH;

	// The vtk file name
	char *MODELNAME;

	// The Distance Filter
	DistanceFilter *dFilter;
	// The two paramter of Distance Filter
	float DISTANCEFILTER_EPS;
	float DISTANCEFILTER_DIFFRATE;

	//Object *obj;
	//Recognition *recognition;
};