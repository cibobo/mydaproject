#include "stdafx.h"
#include "MainThread.hpp"

MainThread::MainThread(){
	pDFilter = new DistanceFilter();

	pDetector = new MyFeatureDetector();

	pLearning = new Learning();

	pIterator = new FrameIterator();

	pRecognition = new Recognition();

	pPMDCamIO = new PMDCamIO();

	ThreadIndex = 1001;

	isLearning = true;
	isRecognise = false;

	p3DDataViewContext = new OpenGLContext;
	pObjViewContext = new OpenGLContext;


	bDone = true;
	isPause = false;
	isDataUsed = false;

	associaterate = 0.02;

	FRAMERATE = 30;

	//INPUTPATH = "Eva2Boxes";
	isOnline = false;
	isOffline = !isOnline;

	INPUTPATH = new char[150];

	EVAOUTPUTPATH = "FirstTest";

	MODELNAME = "box2_all2";

	InitializeCriticalSection (&pauseCrs);
	InitializeCriticalSection (&frameCrs);
	InitializeCriticalSection (&glInitCrs);
	InitializeCriticalSection (&cvInitCrs);
	InitializeCriticalSection (&calcCrs);

	isObservingWindowVisible = true;
	isResultWindowVisible = true;
	isOpenCVWindowVisible = true;
}

MainThread::~MainThread(){
	delete pDFilter;
	delete pDetector;
	delete pLearning;
	delete pIterator;
	delete pRecognition;
	
	delete p3DDataViewContext;
	delete pObjViewContext;

}

/**
 * creat the static function to start a thread. See:
 * http://stackoverflow.com/questions/1372967/how-do-you-use-createthread-for-functions-which-are-class-members
 **/
DWORD WINAPI MainThread::beginOfflineInputThread(void *param){
	MainThread *This = (MainThread*)param;
	return This->offlineInputThreadProc();
}

DWORD WINAPI MainThread::beginOnlineInputThread(void *param){
	MainThread *This = (MainThread*)param;
	return This->onlineInputThreadProc();
}

DWORD WINAPI MainThread::beginOpenGLSceneThread(void *param){
	MainThread *This = (MainThread*)param;
	return This->openGLSceneThreadProc();
}
DWORD WINAPI MainThread::beginOpenGLResultThread(void *param){
	MainThread *This = (MainThread*)param;
	return This->openGLResultThreadProc();
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
		CreateThread(NULL, 0, this->beginOfflineInputThread, (void*)this, 0, &InputThreadID);
	}

	if(this->isOnline){
		CreateThread(NULL, 0, this->beginOnlineInputThread, (void*)this, 0, &InputThreadID);
	}

	// If OpenGL Observing window is visible
	if(this->isObservingWindowVisible){
		CreateThread(NULL, 0, this->beginOpenGLSceneThread, (void*)this, 0, &OpenGLThreadID);
	}
	
	// If OpenGL result window is visible
	if(this->isResultWindowVisible){
		CreateThread(NULL, 0, this->beginOpenGLResultThread, (void*)this, 0, &OpenGLThreadID);
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
		BildData *currentBildData = pIterator->getCurrentBildData();
		
		// Gaussian Filter
		ImageProcess::filterDepthDate(currentBildData->threeDData, 0.3);

		// Distance Filter
		bool isDiff = this->pDFilter->Filte(currentBildData, currentBildData->filteredData);

		if(isDiff){
			pDetector->setDetectedData(currentBildData);
			pDetector->usingSTAR();
			pDetector->createPMDFeatures();	
		}

		if(this->isLearning){
			pLearning->setTrainingFeatures(pDetector->PMDFeatures);
			pLearning->setCurrentBildData(currentBildData);
			pLearning->setHistoricalBildData(pIterator->getHistoricalBildData());

			pLearning->findObjectFeatures();
			if(pLearning->hisBildData->features.size()>0 &&
			   pLearning->curBildData->features.size()>0){
				pLearning->findAssociations();
				pLearning->findTransformation();
				//pLearning->updateObject();

				//Choosing frame
				pIterator->framesControl(pLearning);

			}
		} 

		if(this->isRecognise){
			pRecognition->objectRecognition(pDetector->PMDFeatures);
		}

		this->isDataUsed = true;
		LeaveCriticalSection (&calcCrs);

		// display 3D structur of the Object in an OpenGL Window
		PostMessage(pObjViewContext->hWnd, WM_PAINT, 0, 0);	

		LeaveCriticalSection(&frameCrs);
		LeaveCriticalSection(&pauseCrs);

		//LeaveCriticalSection (&filterInitCrs);
		
	}
	return 0;
}

DWORD MainThread::offlineInputThreadProc(void){
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);

	setDefaultLoadPath(INPUTPATH);

	cout<<"Upgrading Distance Filter and Save Data into Buffer....."<<endl;
	for(int i=1;i<pDFilter->creatingFrames;i++){
		pIterator->frameIndex = i;
		//loadNormalDataFromFile("distance", i, bildData->disData);
		BildData *temp = new BildData();
		loadAllDataFromFile(i, temp);

		//add the new data into the DataBuffer
		pIterator->initDataBuffer(temp);

		//dFilter->Upgrade(temp->disData);
		pDFilter->Upgrade(temp);
	}
	cout<<"Upgrading complete!"<<endl;
	
	LeaveCriticalSection (&cvInitCrs);
	LeaveCriticalSection (&glInitCrs);

	for(int i=20;i<999;i++){
		cout<<endl<<"============================= "<<i<<" ==========================="<<endl;
		pIterator->frameIndex = i;
		EnterCriticalSection(&pauseCrs);
		EnterCriticalSection(&frameCrs);

		BildData *temp = new BildData();
		loadAllDataFromFile(i, temp);

		pIterator->updateDataBuffer(temp);

		//updata the OpenGL Window	
		PostMessage(p3DDataViewContext->hWnd, WM_PAINT, 0, 0);	
		isDataUsed = false;

		cout<<"Input running!"<<endl;
		LeaveCriticalSection(&frameCrs);
		LeaveCriticalSection(&pauseCrs);
		Sleep(FRAMERATE);
	}
	return 0;
}

DWORD MainThread::onlineInputThreadProc(void){
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);

	cout<<"PMD Camera Connecting..."<<endl;
	if(!pPMDCamIO->createPMDCon()){
		exit(1);
		//cout<<"input thread running!"<<endl;
		
	}
	cout<<"Upgrading Distance Filter and Save Data into Buffer....."<<endl;
	for(int i=0;i<pDFilter->creatingFrames;i++){
		BildData *temp = new BildData();
		pPMDCamIO->getPMDData(temp);

		//add the new data into the DataBuffer
		pIterator->initDataBuffer(temp);

		//dFilter->Upgrade(temp->disData);
		pDFilter->Upgrade(temp);
	}
	cout<<"Upgrading complete!"<<endl;

	LeaveCriticalSection (&glInitCrs);
	LeaveCriticalSection (&cvInitCrs);

	while(!bDone){
		EnterCriticalSection(&pauseCrs);
		EnterCriticalSection(&frameCrs);

		try {
			BildData *temp = new BildData();
			pPMDCamIO->getPMDData(temp);

			pIterator->updateDataBuffer(temp);

			//updata the OpenGL Window	
			PostMessage(p3DDataViewContext->hWnd, WM_PAINT, 0, 0);	
			isDataUsed = false;

		} catch (CMemoryException * e){
			AfxMessageBox("Irgendwas ist schiefgegangen!");
		}
		
		LeaveCriticalSection(&frameCrs);
		LeaveCriticalSection(&pauseCrs);
		Sleep(FRAMERATE);
	}
	pPMDCamIO->closePMDCon();
	return 0;
}

DWORD MainThread::openGLSceneThreadProc(void) 
{ 
	static OpenGLWinUI *pOpenGLWinUI = new OpenGLWinUI;
	//p3DDataViewContext = new OpenGLContext;

	TlsSetValue(ThreadIndex, pOpenGLWinUI);


	int width;
	int height;

	if(this->isLearning){
		width = 900;
		height = 600;
	} 

	if(this->isRecognise){
		width = 600;
		height = 600;
	}


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
			if(this->isRecognise){
				displayGraphs(pOpenGLWinUI, pRecognition->graphList);
				glEnable(GL_LIGHTING);
			}
			if(this->isLearning){
				display(pOpenGLWinUI, pIterator->getCurrentBildData());
			}
			SwapBuffers(p3DDataViewContext->hDC);
			//LeaveCriticalSection (&calcCrs);
			LeaveCriticalSection(&frameCrs);
		}
	}

	//delete p3DDataViewContext;
	delete pOpenGLWinUI;

	return 0;
} 
DWORD MainThread::openGLResultThreadProc(void){
	static OpenGLWinUI *pObjGLWinUI = new OpenGLWinUI;

	TlsSetValue(ThreadIndex, pObjGLWinUI);

	if(!CreateOpenGLWindow("Object Window", 0, 0, 1000, 750, PFD_TYPE_RGBA, 0, pObjGLWinUI, pObjViewContext)){
		exit(0);
	}

	cout<<"The Object Structur OpenGL Windows is running"<<endl;

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
				//display(pOpenGLWinUI, disData, intData, ampData);
			}
		} else {
			EnterCriticalSection (&calcCrs);
			if(this->isRecognise){
				displayObjects(pObjGLWinUI, pRecognition->modelList);
			}
			if(this->isLearning){
				display(pObjGLWinUI, this->pLearning->pObject);
			}
			SwapBuffers(pObjViewContext->hDC);
			glEnable(GL_LIGHTING);
			LeaveCriticalSection(&calcCrs);
		}
	}
	delete pObjGLWinUI;
}
DWORD MainThread::openCVHelpThreadProc(void)
{
	EnterCriticalSection (&cvInitCrs);
	namedWindow("OpenCVFeatureDetection", CV_WINDOW_AUTOSIZE);
	if(this->isLearning){
		namedWindow("OpenCVCorrespondenz", CV_WINDOW_AUTOSIZE);
		namedWindow("OpenCVSummedFeatures", CV_WINDOW_AUTOSIZE);
	}
	if(this->isRecognise){
		namedWindow("OpenCVRecognition", CV_WINDOW_AUTOSIZE);
	}
	LeaveCriticalSection (&cvInitCrs);

	while(!bDone){	
		// Create Matrix to save the RGB images for both current and historical frames
		//Matrix for Feature Detection
		Mat feaMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);
		Mat curMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);
		Mat hisMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);

		// Get data from float array to Matrix
		EnterCriticalSection (&calcCrs);
		BildData* curBildData = pIterator->getCurrentBildData();
		BildData* hisBildData = pIterator->getHistoricalBildData();

		ImageProcess::transFloatToMat3(curBildData->filteredData, feaMat, pDetector->balance, pDetector->contrast);
		
		
		LeaveCriticalSection (&calcCrs);

		// The Window for feature Detection
		for(int i=0;i<pDetector->keypoints.size();i++){
			circle(feaMat, pDetector->keypoints[i].pt, 1, Scalar(0,0,255,0), -1);
		}
		imshow("OpenCVFeatureDetection", feaMat);



		if(this->isLearning){
			EnterCriticalSection (&calcCrs);
			ImageProcess::transFloatToMat3(curBildData->ampData, curMat, pDetector->BEGINBRIGHTNESS, pDetector->BEGINCONTRAST);
			ImageProcess::transFloatToMat3(hisBildData->ampData, hisMat, pDetector->BEGINBRIGHTNESS, pDetector->BEGINCONTRAST);

			// The correspondent window
			Mat testImg = Mat(V_BILDSIZE, H_BILDSIZE*2, CV_8UC3);
			Mat left = Mat(testImg, Range::all(), Range(0,H_BILDSIZE));
			Mat right = Mat(testImg, Range::all(), Range(H_BILDSIZE,H_BILDSIZE*2));
			hisMat.copyTo(left);
			curMat.copyTo(right);

			vector<PMDPoint> curFeatures = curBildData->features;
			vector<PMDPoint> hisFeatures = hisBildData->features;

			vector<PMDPoint> oldResult = this->pLearning->hisAssPoints;
			vector<PMDPoint> newResult = this->pLearning->curAssPoints;

			LeaveCriticalSection (&calcCrs);

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

			
			EnterCriticalSection (&calcCrs);

			// The Summed Features Window
			Mat graphImg;
			curMat.copyTo(graphImg);
			
			LeaveCriticalSection (&calcCrs);
			
			for(int i=0;i<curFeatures.size();i++){
				circle(graphImg, curFeatures[i].index, 2, Scalar(0,0,255), -1);
				for(int j=i;j<curFeatures.size();j++){
					line(graphImg, curFeatures[i].index, curFeatures[j].index, Scalar(0, 255, 255, 0));
				}
			}
			
			imshow("OpenCVSummedFeatures", graphImg);

			
		}

		if(this->isRecognise){
			//EnterCriticalSection (&calcCrs);
			Mat recMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);
			ImageProcess::transFloatToMat3(curBildData->ampData, recMat, pDetector->BEGINBRIGHTNESS, pDetector->BEGINCONTRAST);
			
			for(int i=0;i<this->pRecognition->segResult.size();i++){
				Scalar color = scalarColorList[i];
				for(int j=0;j<this->pRecognition->segResult[i].size();j++){
					circle(recMat, this->pRecognition->segResult[i][j].index, 2, color, -1);
						/*for(int k=j+1;k<this->pRecognition->segResult[i].size();k++){
							line(recMat, this->pRecognition->segResult[i][j].index, this->pRecognition->segResult[i][k].index, Scalar(0,255,255,0),1);
						}*/
				}
			}	
					//circle(drawMat, Point2f(i*10+5, 5), 5, color, -1);

			imshow("OpenCVRecognition", recMat);

			
			//LeaveCriticalSection (&calcCrs);
			
		}

		char c = waitKey(10);
		if(c == 27) break;
	}
	return 0;
}