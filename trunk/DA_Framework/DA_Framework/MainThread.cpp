#include "stdafx.h"
#include "MainThread.hpp"

MainThread::MainThread(){
	pDFilter = new DistanceFilter();

	pDetector = new MyFeatureDetector();

	pLearning = new Learning();

	pIterator = new FrameIterator();

	pRecognition = new Recognition();

	pPMDCamIO = new PMDCamIO();

	//pLogger = new Evaluator();
	//pLogger->createCSVFile(this->loggerName);

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
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);
	LeaveCriticalSection (&cvInitCrs);
	LeaveCriticalSection (&glInitCrs);

#ifdef EVALUATION
	string EVA_RootPath = string("2012.10.18");
	EVA_RootPath.append("/");
	EVA_RootPath.append(this->INPUTPATH);
	//const char *EVA_RootPath = "2012.09.17/EmptyBox";

	Evaluator *pEvaluator = new Evaluator(EVA_RootPath.data());

	//DFilter
#ifdef EVA_DFILTER
#ifdef NO_DFILTER
	const char *dFilterFileName = "Without Distance Filter";
#else 
	const char *dFilterFileName = "With Distance Filter";
#endif
	pEvaluator->createCSVFile(dFilterFileName);
	pEvaluator->writeCSVTitle(dFilterFileName, "FrameIndex, index_x, index_y, Features, Sammed Features");
#endif

	//Brightness Control
#ifdef EVA_CBRIGHTNESS
	const char *brightnessFileName = "With Brightness Control";
	pEvaluator->createCSVFile(brightnessFileName);
	pEvaluator->writeCSVTitle(brightnessFileName, "FrameIndex, index_x, index_y, Features, Sammed Features, End Contrast, End Energie");
#endif

	//Association
#ifdef EVA_ASSOCIATION
	const char *associationFileName = "With alternative Variance";
	//const char *associationFileName = "Without all Improvement";
	pEvaluator->createCSVFile(associationFileName);
	pEvaluator->writeCSVTitle(associationFileName, "FrameIndex, index_x, index_y, Features, Sammed Features, Correspondenz Pairs, Association Variance, Summed Maximal Value, Fixed Nodes, Rotation Angle");
#endif

	//Frame Controll
#ifdef EVA_FRAMECONTROL
	const char *frameControlFileName = "Without Frame Control";
	pEvaluator->createCSVFile(frameControlFileName);
	pEvaluator->writeCSVTitle(frameControlFileName, "FrameIndex, index_x, index_y, Features, Sammed Features, Correspondenz Pairs, Association Variance, Fixed Nodes, Rotation Angle, Jummped Frames");
#endif

	//Graph Update
#ifdef EVA_GRAPHUPDATE
	const char *graphUpdateFileName = "Graph Update";
	pEvaluator->createCSVFile(graphUpdateFileName);
	//pEvaluator->writeCSVTitle(graphUpdateFileName, "Distance Threshold, Lifetime Threshold, Number of fixed Nodes, Number of summaried fixed Nodes");
#endif

	//Running Time
#ifdef EVA_TIME
	const char *timeFileName = "Time";
	pEvaluator->createCSVFile(timeFileName);
	pEvaluator->writeCSVTitle(timeFileName,"Frame Index, Disstance Filter, Star Detector & Brightness Control, Summerized Features, Segmentation, Correspondence, Orientation, Frame Control, Total Calculation, Total Frame");
#endif

#ifdef EVA_RECOGNITION
	const char *recogFileName = "Recognition with 2 model_without Improvement";
	pEvaluator->createCSVFile(recogFileName);
	//pEvaluator->writeCSVTitle(recogFileName, "Frame Index, Reslutlist Size, Is Find, Is Model 1 found, Is Model 2 found, Result 1, Result 2, Run Time");
	pEvaluator->writeCSVTitle(recogFileName, "Frame Index, Reslutlist Size, Is Find, Is Model 1 found, Is Model 2 found, Nodes Count, Run Time");
#endif

	//Graph Isomorphismus
#ifdef EVA_ISOMORPHISMUS
	const char *graphIsoFileName = "Graph Isomorphismus 1 Model";
	pEvaluator->createCSVFile(graphIsoFileName);
	//pEvaluator->writeCSVTitle(graphIsoFileName, "Proportion of Distance, Proportion of similar Nodes, Recognized Frames, Right Recognized Frames, Wrong Recognized Frames, Checked Node Count, Frame Count");
#endif

#endif

	while(!bDone){

#ifdef EVA_TIME
		vector<float> timmer;
		timmer.push_back(this->pIterator->frameIndex);
		int start, end;
		int calStart, calEnd;
		int frameStart, frameEnd;
		frameStart = clock();
#endif

		if(isDataUsed){
			continue;
		}
		//call the calculate function after the init of PMD Camera
		//EnterCriticalSection (&filterInitCrs);



		EnterCriticalSection(&pauseCrs);
		EnterCriticalSection(&frameCrs);
		EnterCriticalSection (&calcCrs);

#ifdef EVA_TIME
		//Begin of the Calculation
		calStart = clock();
#endif
		//get the current Bild Data, which is saved at the last position of the List
		BildData *currentBildData = pIterator->getCurrentBildData();
		
		// Gaussian Filter
		ImageProcess::filterDepthDate(currentBildData->threeDData, 0.3);

#ifndef NO_DFILTER

#ifdef EVA_TIME
		//Begin of the Distance Filter
		start = clock();
#endif
		// Distance Filter
		bool isDiff = this->pDFilter->Filte(currentBildData, currentBildData->filteredData);

#ifdef EVA_TIME
		//End of the Distance Filter
		end = clock();
		timmer.push_back(end-start);
#endif

#else
		// For the evalutation, close the DistanceFilter
		bool isDiff = true;
		memcpy(currentBildData->filteredData, currentBildData->ampData, sizeof(float)*H_BILDSIZE*V_BILDSIZE);
#endif


		if(isDiff){
			pDetector->setDetectedData(currentBildData);

#ifdef EVA_TIME
			//Begin of the Star Detector
			start = clock();
#endif
			pDetector->usingSTAR();

#ifdef EVA_TIME
			//End of the Star Detector
			end = clock();
			timmer.push_back(end-start);
			//Begin of the Summerized Features
			start = clock();
#endif
			pDetector->createPMDFeatures();	

#ifdef EVA_TIME
			//End of the Summerized Features
			end = clock();
			timmer.push_back(end-start);
#endif
		}

#ifdef EVA_TIME
		//If Distance Filter not run
		else{
			timmer.push_back(-1);
			timmer.push_back(-1);
		}
#endif

		if(this->isLearning){
			pLearning->setTrainingFeatures(pDetector->PMDFeatures);
			pLearning->setCurrentBildData(currentBildData);
			pLearning->setHistoricalBildData(pIterator->getHistoricalBildData());

#ifdef EVA_TIME
			//Begin of the Segmentation
			start = clock();
#endif
			pLearning->findObjectFeatures();

#ifdef EVA_TIME
			//End of the Segmentation
			end = clock();
			timmer.push_back(end-start);
#endif
			if(pLearning->hisBildData->features.size()>0 &&
			   pLearning->curBildData->features.size()>0){

#ifdef EVA_TIME
				//Begin of the Correspondenz
				start = clock();
#endif

				pLearning->findAssociations();

#ifdef EVA_TIME
				//End of the Correspondenz
				end = clock();
				timmer.push_back(end-start);
				//Begin of the Orientation
				start = clock();
#endif
				pLearning->findTransformation();

#ifdef EVA_TIME
				//End of the Orientation
				end = clock();
				timmer.push_back(end-start);
				//Begin of the Frame Control
				start = clock();
#endif
				//pLearning->updateObject();

				//Choosing frame
				pIterator->framesControl(pLearning);

#ifdef EVA_TIME
				//End of the Frame Control
				end = clock();
				timmer.push_back(end-start);
#endif
			}

#ifdef EVA_TIME
			//If there is no feature detected
			else{
				timmer.push_back(-1);
				timmer.push_back(-1);
				timmer.push_back(-1);
			}
#endif

		//Code of the Evaluation
#ifdef EVALUATION
#ifndef EVA_RECOGNITION
		vector<float> evaData;
		evaData.push_back(this->pIterator->frameIndex);
		//Point3f midPoint = this->pLearning->pObject->getMiddelPoint();
		//evaData.push_back(midPoint.x);
		//evaData.push_back(midPoint.y);
		//evaData.push_back(midPoint.z);
		Point2i center = this->pLearning->getCenter();
		evaData.push_back(center.x);
		evaData.push_back(center.y);
		evaData.push_back(this->pDetector->PMDFeatures.size());
		evaData.push_back(this->pLearning->curBildData->features.size());
#endif

#ifdef EVA_DFILTER
		pEvaluator->saveCSVData(dFilterFileName, evaData);
#endif

#ifdef EVA_CBRIGHTNESS
		evaData.push_back(this->pDetector->contrast);
		evaData.push_back(this->pDetector->energie);
		pEvaluator->saveCSVData(brightnessFileName, evaData); 
#endif

#ifdef EVA_ASSOCIATION
		evaData.push_back(this->pLearning->curAssPoints.size());
		evaData.push_back(this->pLearning->associateVariance);
		evaData.push_back(this->pLearning->sumMaxValue);
		evaData.push_back(this->pLearning->pObject->getFixedNodeCount());
		evaData.push_back(this->pLearning->angle);
		pEvaluator->saveCSVData(associationFileName, evaData);
#endif

#ifdef EVA_FRAMECONTROL
		evaData.push_back(this->pLearning->curAssPoints.size());
		evaData.push_back(this->pLearning->associateVariance);
		evaData.push_back(this->pLearning->pObject->getFixedNodeCount());
		//If frame has been jumped, set the rotated angle as 0 for the current frame
		if(this->pIterator->jumpedFeatures>0){
			evaData.push_back(0);
		} else {
			evaData.push_back(this->pLearning->angle);
		}
		evaData.push_back(this->pIterator->jumpedFeatures);
		pEvaluator->saveCSVData(frameControlFileName, evaData);
#endif


#endif
		} 


		if(this->isRecognise){
#ifdef EVA_RECOGNITION
			int beginTime, endTime;
			beginTime = clock();
#endif
			pRecognition->objectRecognition(pDetector->PMDFeatures);
#ifdef EVA_RECOGNITION
			endTime = clock();
			vector<float> recogData;
			recogData.push_back(this->pIterator->frameIndex);
			recogData.push_back(this->pRecognition->multiResultList.size());
			//if(this->pRecognition->multiResultList.size() == 0){
			//	recogData.push_back(-1);
			//	recogData.push_back(-1);
			//	recogData.push_back(-1);
			//} else {
			//	recogData.push_back(this->pRecognition->isInCurrentFound);
			//	recogData.push_back(this->pRecognition->multiResultList[0]->modelIndex);
			//	recogData.push_back(this->pRecognition->multiResultList[0]->weight);
			//}
			recogData.push_back(this->pRecognition->isInCurrentFound);
			recogData.push_back(this->pRecognition->statisticResult[0]);
			recogData.push_back(this->pRecognition->statisticResult[1]);
			//switch(this->pRecognition->multiResultList.size()){
			//	case 0: recogData.push_back(-1);
			//			recogData.push_back(-1);
			//			break;
			//	case 1: recogData.push_back(this->pRecognition->multiResultList[0]->modelIndex);
			//			recogData.push_back(-1);
			//			break;
			//	default: recogData.push_back(this->pRecognition->multiResultList[0]->modelIndex);
			//			recogData.push_back(this->pRecognition->multiResultList[1]->modelIndex);
			//			break;
			//}
			recogData.push_back(this->pRecognition->evaNodeCount);
			recogData.push_back(endTime-beginTime);
			pEvaluator->saveCSVData(recogFileName, recogData);
#endif
		}
		//OpenCV Bild
#ifdef EVA_SAVECVBILD
		if(pIterator->frameIndex % FRAME_INTERVAL == 0){
			Mat curMat = Mat(H_BILDSIZE, V_BILDSIZE, CV_8UC3);
			ImageProcess::transFloatToMat3(this->pIterator->getCurrentBildData()->ampData, curMat, pDetector->BEGINBRIGHTNESS, pDetector->BEGINCONTRAST);

			if(this->isLearning){
				vector<PMDPoint> features = this->pLearning->curBildData->features;
				for(int i=0;i<features.size();i++){
					circle(curMat, features[i].index, 3, Scalar(0,0,255,0), -1);
				}
			}

			if(this->isRecognise){
				for(int i=0;i<this->pRecognition->segResult.size();i++){
					Scalar color = scalarColorList[i];
					for(int j=0;j<this->pRecognition->segResult[i].size();j++){
						circle(curMat, this->pRecognition->segResult[i][j].index, 2, color, -1);
					}
				}	
			}

			//Translate the frame index into the file name
			stringstream ss;
			ss<<pIterator->frameIndex;
			string path(ss.str());
			pEvaluator->saveCVBild(path.data(),curMat);
		}
#endif

		//Screenshot
#ifdef SCREENSHOT
	if(this->pIterator->frameIndex == SCREENSHOT_FRAME){
//#ifdef EVA_GRAPHUPDATE
//		//save the VTK file with the life time
//		stringstream ss;
//		ss<<this->pLearning->updateTThreshold;
//		string fileName = ss.str();
//		this->pLearning->pObject->saveToVTKFile(fileName.data());
//#endif
		//pLearning->pObject->joinSimilarNodes(0.045);
		EnterCriticalSection(&pauseCrs);
	}
#endif



		this->isDataUsed = true;

#ifdef EVA_TIME
		//End the time of Calculation
		calEnd = clock();
		timmer.push_back(calEnd-calStart);
#endif


		LeaveCriticalSection (&calcCrs);

		// display 3D structur of the Object in an OpenGL Window
		PostMessage(pObjViewContext->hWnd, WM_PAINT, 0, 0);	

		LeaveCriticalSection(&frameCrs);
		LeaveCriticalSection(&pauseCrs);

		//LeaveCriticalSection (&filterInitCrs);

		//Running Time
#ifdef EVA_TIME
		frameEnd = clock();
		timmer.push_back(frameEnd-frameStart);
		pEvaluator->saveCSVData(timeFileName, timmer);
#endif
		
	}
#ifdef EVA_GRAPHUPDATE
		//save the VTK file with the life time
		//stringstream ss;
		//ss<<this->pLearning->updateTThreshold;
		//string fileName = ss.str();
		//this->pLearning->pObject->saveToVTKFile(fileName.data());

		vector<float> endData;
		endData.push_back(this->pLearning->updateDThreshold);
		endData.push_back(this->pLearning->updateTThreshold);
		//Get the number of fixed Nodes
		endData.push_back(this->pLearning->pObject->getFixedNodeCount());
		//Join the similar Nodes
		pLearning->pObject->joinSimilarNodes(0.045);
		//Get the summaried Nodes
		endData.push_back(this->pLearning->pObject->nodeList.size());
		pEvaluator->saveCSVData(endData);
#endif

#ifdef EVA_ISOMORPHISMUS
		vector<float> endData;
		endData.push_back(this->pRecognition->distanceProportion);
		endData.push_back(this->pRecognition->nodesCountProportion);
		endData.push_back(this->pRecognition->evaIsoRecogCount);
		endData.push_back(this->pRecognition->statisticResult[0]);
		endData.push_back(this->pRecognition->statisticResult[1]);
		endData.push_back(this->pRecognition->evaNodeCount);
		endData.push_back(this->pIterator->frameIndex);
		pEvaluator->saveCSVData(graphIsoFileName, endData);
#endif
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
		exit(100);
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

	int width;
	int height;

	if(this->isLearning){
		width = 750;
		height = 750;
	} 

	if(this->isRecognise){
		int modelSize = this->pRecognition->modelList.size();
		width = 750;
		if(modelSize!=0){
			height = width/modelSize;
		} else {
			height = width;
		}
	}


	if(!CreateOpenGLWindow("Object Window", 0, 0, width, height, PFD_TYPE_RGBA, 0, pObjGLWinUI, pObjViewContext)){
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
		
		BildData* curBildData = pIterator->getCurrentBildData();
		BildData* hisBildData = pIterator->getHistoricalBildData();
		//BildData* curBildData = pLearning->curBildData;
		//BildData* hisBildData = pLearning->hisBildData;

		EnterCriticalSection (&calcCrs);
		ImageProcess::transFloatToMat3(curBildData->filteredData, feaMat, pDetector->balance, pDetector->contrast);
		vector<KeyPoint> keyPoints = pDetector->keypoints;

		LeaveCriticalSection (&calcCrs);
		
		// The Window for feature Detection
		for(int i=0;i<keyPoints.size();i++){
			circle(feaMat, keyPoints[i].pt, 1, Scalar(0,0,255,0), -1);
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
				/*for(int j=i;j<curFeatures.size();j++){
					line(graphImg, curFeatures[i].index, curFeatures[j].index, Scalar(0, 255, 255, 0));
				}*/
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