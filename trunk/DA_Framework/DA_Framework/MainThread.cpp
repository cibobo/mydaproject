#include "stdafx.h"
#include "MainThread.hpp"

MainThread::MainThread(){
	pParameters = new Parameters();

	pDFilter = new DistanceFilter();

	pDetector = new MyFeatureDetector();

	pLearning = new Learning();

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

void MainThread::run(){
	DWORD InputThreadID, OpenGLThreadID, OpenCVThreadID, CalculateThreadID;
	if(this->isOffline){
		CreateThread(NULL, 0, this->beginInputThread, (void*)this, 0, &InputThreadID);
	} 

	if(this->isObservingWindowVisible){
		CreateThread(NULL, 0, this->beginOpenGLSceneThread, (void*)this, 0, &OpenGLThreadID);
	}

	// Begin the Calculation Process
	CreateThread(NULL, 0, this->beginCalculationThread, (void*)this, 0, &CalculateThreadID);

	// If OpenCV Widnows are selected to show
	if(this->isOpenCVWindowVisible){
		CreateThread(NULL, 0, this->beginOpenCVHelpThread, (void*)this, 0, &OpenCVThreadID);
	}
}

DWORD MainThread::calculationThreadProc(void){
	//DWORD InputThreadID, OpenGLThreadID, OpenCVThreadID;
	//if(this->isOffline){
	//	CreateThread(NULL, 0, this->beginInputThread, (void*)this, 0, &InputThreadID);
	//} 

	//if(this->isObservingWindowVisible){
	//	CreateThread(NULL, 0, this->beginOpenGLSceneThread, (void*)this, 0, &OpenGLThreadID);
	//}

	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);
	LeaveCriticalSection (&cvInitCrs);
	LeaveCriticalSection (&glInitCrs);

	while(!bDone){

		if(isDataUsed){
			continue;
		}
		//call the calculate function after the init of PMD Camera
		//EnterCriticalSection (&filterInitCrs);


		EnterCriticalSection(&pauseCrs);
		EnterCriticalSection(&frameCrs);
		EnterCriticalSection (&calcCrs);

		//get the current Bild Data, which is saved at the last position of the List
		BildData *currentBildData = bildDataBuffer.back();
		
		// Gaussian Filter
		ImageProcess::filterDepthDate(currentBildData->threeDData, 0.3);

		// Distance Filter
		bool isDiff = this->pDFilter->Filte(currentBildData, currentBildData->filteredData);

		if(isDiff){
			pDetector->setDetectedData(currentBildData);
			pDetector->usingSTAR();
			pDetector->createPMDFeatures();

			
		}

		pLearning->setTrainingFeatures(pDetector->PMDFeatures);
		pLearning->setCurrentBildData(currentBildData);
		pLearning->setHistoricalBildData(bildDataBuffer.front());

		pLearning->findObjectFeatures();
		pLearning->findAssociations();

		LeaveCriticalSection (&calcCrs);
		LeaveCriticalSection(&frameCrs);
		LeaveCriticalSection(&pauseCrs);

		//LeaveCriticalSection (&filterInitCrs);
		
	}
	return 0;
}

DWORD MainThread::offlineInputThreadProc(void){
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);
	//EnterCriticalSection (&filterInitCrs);
	setDefaultLoadPath(INPUTPATH);

	////get the distance data for the first step
	//BildData *temp = new BildData();
	//loadAllDataFromFile(3, temp);
	////init a distance filter
	////dFilter = new DistanceFilter(temp, this->pParameters->DISTANCEFILTER_EPS, this->pParameters->DISTANCEFILTER_DIFFRATE);
	//dFilter = new DistanceFilter(temp); 

	cout<<"Upgrading Distance Filter and Save Data into Buffer....."<<endl;
	for(int i=1;i<pDFilter->creatingFrames;i++){
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
		pDFilter->Upgrade(temp);
	}
	cout<<"Upgrading complete!"<<endl;
	
	//LeaveCriticalSection (&filterInitCrs);
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
			//EnterCriticalSection (&calcCrs);
			EnterCriticalSection(&frameCrs);
#ifdef RECOGNITION
			displayRecog(pOpenGLWinUI, recognition->graphList);
			glEnable(GL_LIGHTING);
#else
			display(pOpenGLWinUI, bildDataBuffer.back());
#endif
			SwapBuffers(p3DDataViewContext->hDC);
			//LeaveCriticalSection (&calcCrs);
			LeaveCriticalSection(&frameCrs);
		}
	}

	//delete p3DDataViewContext;
	delete pOpenGLWinUI;

	return 0;
} 
DWORD MainThread::openCVHelpThreadProc(void)
{
	EnterCriticalSection (&cvInitCrs);
	namedWindow("OpenCVFeatureDetection", CV_WINDOW_AUTOSIZE);
	namedWindow("OpenCVCorrespondenz", CV_WINDOW_AUTOSIZE);
	namedWindow("OpenCVSummedFeatures", CV_WINDOW_AUTOSIZE);
	LeaveCriticalSection (&cvInitCrs);

	while(!bDone){	
		// Create Matrix to save the RGB images for both current and historical frames
		//Matrix for Feature Detection
		Mat feaMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);
		Mat curMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);
		Mat hisMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);


		// Get data from float array to Matrix
		EnterCriticalSection (&calcCrs);
		ImageProcess::transFloatToMat3(bildDataBuffer.back()->filteredData, feaMat, pDetector->balance, pDetector->contrast);
		ImageProcess::transFloatToMat3(bildDataBuffer.back()->ampData, curMat, pDetector->BEGINBRIGHTNESS, pDetector->BEGINCONTRAST);
		ImageProcess::transFloatToMat3(bildDataBuffer.front()->ampData, hisMat, pDetector->BEGINBRIGHTNESS, pDetector->BEGINCONTRAST);

		// The Window for feature Detection
		for(int i=0;i<pDetector->keypoints.size();i++){
			circle(feaMat, pDetector->keypoints[i].pt, 1, Scalar(0,0,255,0), -1);
		}
		imshow("OpenCVFeatureDetection", feaMat);

		LeaveCriticalSection (&calcCrs);

		// The correspondent window
		Mat testImg = Mat(V_BILDSIZE, H_BILDSIZE*2, CV_8UC3);
		Mat left = Mat(testImg, Range::all(), Range(0,H_BILDSIZE));
		Mat right = Mat(testImg, Range::all(), Range(H_BILDSIZE,H_BILDSIZE*2));
		hisMat.copyTo(left);
		curMat.copyTo(right);

		EnterCriticalSection (&calcCrs);

		vector<PMDPoint> curFeatures = bildDataBuffer.back()->features;
		vector<PMDPoint> hisFeatures = bildDataBuffer.front()->features;

		vector<PMDPoint> oldResult = this->pLearning->hisAssPoints;
		vector<PMDPoint> newResult = this->pLearning->curAssPoints;

		for(int i=0;i<hisFeatures.size();i++){
			// show the old features
			circle(left, hisFeatures[i].index, 2, Scalar(0,255,0,0), -1);
		}

		for(int i=0;i<curFeatures.size();i++){
			// show the current features
			circle(right, curFeatures[i].index, 2, Scalar(0,0,255,0), -1);
		}

		//if(hisFeatures.size()>0&&curFeatures.size()>0){
		//	featureAssociate(hisFeatures, curFeatures, ASSOCIATESITA, oldIndexResult, newIndexResult);
			for(int i=0;i<oldResult.size();i++){
				Point2f trans(204,0);
				//line(testImg, hisFeatures[oldResult[i]], curFeatures[newResult[i]]+trans, Scalar(255,255,0,0));
				line(testImg, oldResult[i].index, newResult[i].index+trans, Scalar(255,255,0,0));
			}
		//	cout<<"The number of useful features is: "<<oldResult.size()<<endl;
		//}
		
		imshow("OpenCVCorrespondenz", testImg);

		LeaveCriticalSection (&calcCrs);


		EnterCriticalSection (&calcCrs);
		// The Summed Features Window
		Mat graphImg;
		curMat.copyTo(graphImg);
		


		for(int i=0;i<curFeatures.size();i++){
			circle(graphImg, curFeatures[i].index, 2, Scalar(0,0,255), -1);
			for(int j=i;j<curFeatures.size();j++){
				line(graphImg, curFeatures[i].index, curFeatures[j].index, Scalar(0, 255, 255, 0));
			}
		}
		
		imshow("OpenCVSummedFeatures", graphImg);

		LeaveCriticalSection (&calcCrs);

		char c = waitKey(10);
		if(c == 27) break;
	}
	return 0;
}