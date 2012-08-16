#include "stdafx.h"
#include "MainThread.hpp"

MainThread::MainThread(){
	pParameters = new Parameters();

	ThreadIndex = 1001;

	isLearning = true;
	isRecognise = false;

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
	isOnline = false;
	isOffline = !isOnline;
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
	InitializeCriticalSection (&filterInitCrs);

	isObservingWindowVisible = false;
	isResultWindowVisible = false;
	isOpenCVWindowVisible = true;
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
	return This->openGLSceneThreadProc();
}

DWORD WINAPI MainThread::beginCalculationThread(void *param){
	MainThread *This = (MainThread*)param;
	return This->calculationThreadProc();
}

DWORD WINAPI MainThread::beginOpenCVHelpThread(void *param){
	MainThread *This = (MainThread*)param;
	return This->openCVHelpThreadProc();
}

DWORD MainThread::calculationThreadProc(void){
	DWORD InputThreadID, OpenGLThreadID, OpenCVThreadID;
	if(this->isOffline){
		CreateThread(NULL, 0, this->beginInputThread, (void*)this, 0, &InputThreadID);
	} 

	if(this->isObservingWindowVisible){
		CreateThread(NULL, 0, this->beginOpenGLSceneThread, (void*)this, 0, &OpenGLThreadID);
	}


	//while (!bDone) 
	//{ 	
	//	if(isPause){
	//		continue;
	//	}

	//	if(isDataUsed){
	//		continue;
	//	}
	if(!bDone && !isPause && !isDataUsed){

		//call the calculate function after the init of PMD Camera
		//EnterCriticalSection (&filterInitCrs);

		//EnterCriticalSection (&glInitCrs);
		EnterCriticalSection (&cvInitCrs);
		//EnterCriticalSection (&calcCrs);
		//EnterCriticalSection(&frameCrs);

		//get the current Bild Data, which is saved at the last position of the List
		BildData *currentBildData = bildDataBuffer.back();
		
		// Gaussian Filter
		ImageProcess::filterDepthDate(currentBildData->threeDData, 0.3);

		//LeaveCriticalSection (&filterInitCrs);

		//LeaveCriticalSection (&calcCrs);
		

		// If OpenCV Widnows are selected to show
		//if(this->isOpenCVWindowVisible){
		//	CreateThread(NULL, 0, this->beginOpenCVHelpThread, (void*)this, 0, &OpenCVThreadID);
		//}

		//LeaveCriticalSection(&frameCrs);
		LeaveCriticalSection (&cvInitCrs);
		//LeaveCriticalSection (&glInitCrs);
		
	}
	return 0;
}

DWORD MainThread::offlineInputThreadProc(void){
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);
	EnterCriticalSection (&filterInitCrs);
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
	
	LeaveCriticalSection (&filterInitCrs);
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

DWORD MainThread::openGLSceneThreadProc(void) 
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
DWORD MainThread::openCVHelpThreadProc(void)
{
	while(!bDone){
		EnterCriticalSection (&cvInitCrs);
		// Create Matrix to save the RGB images for both current and historical frames
		Mat curMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);
		Mat hisMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);

		// Get data from float array to Matrix
		ImageProcess::transFloatToMat(bildDataBuffer.back()->ampData, curMat, pParameters->balance, pParameters->contrast);
		ImageProcess::transFloatToMat(bildDataBuffer.front()->ampData, hisMat, pParameters->balance, pParameters->contrast);

		// The correspondent window
		Mat testImg = Mat(V_BILDSIZE, H_BILDSIZE*2, CV_8UC3);
		Mat left = Mat(testImg, Range::all(), Range(0,H_BILDSIZE));
		Mat right = Mat(testImg, Range::all(), Range(H_BILDSIZE,H_BILDSIZE*2));
		hisMat.copyTo(left);
		curMat.copyTo(right);

		//for(int i=0;i<hisFeatures.size();i++){
		//	// show the old features
		//	circle(left, hisFeatures[i].index, 2, Scalar(0,255,0,0), -1);
		//	// show the current features
		//	circle(right, curFeatures[i].index, 2, Scalar(0,0,255,0), -1);
		//}

		////if(hisFeatures.size()>0&&curFeatures.size()>0){
		////	featureAssociate(hisFeatures, curFeatures, ASSOCIATESITA, oldIndexResult, newIndexResult);
		//	for(int i=0;i<oldResult.size();i++){
		//		Point2f trans(204,0);
		//		//line(testImg, hisFeatures[oldResult[i]], curFeatures[newResult[i]]+trans, Scalar(255,255,0,0));
		//		line(testImg, oldResult[i].index, newResult[i].index+trans, Scalar(255,255,0,0));
		//	}
		////	cout<<"The number of useful features is: "<<oldResult.size()<<endl;
		////}
		namedWindow("OpenCVCorrespondenz", CV_WINDOW_AUTOSIZE);
		imshow("OpenCVCorrespondenz", testImg);


		// The Summed Features Window
		Mat graphImg;
		curMat.copyTo(graphImg);

		//for(int i=0;i<curFeatures.size();i++){
		//	circle(graphImg, curFeatures[i].index, 2, Scalar(0,0,255), -1);
		//	for(int j=i;j<curFeatures.size();j++){
		//		line(graphImg, curFeatures[i].index, curFeatures[j].index, Scalar(0, 255, 255, 0));
		//	}
		//}
		namedWindow("OpenCVSummedFeatures", CV_WINDOW_AUTOSIZE);
		imshow("OpenCVSummedFeatures", graphImg);

		LeaveCriticalSection (&cvInitCrs);

		char c = waitKey(100);
		if(c == 27) break;
	}
	return 0;
}