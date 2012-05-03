// TestOpenGL.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "TestOpenGL.h"


/*
 * Defination for Programm Model
 */
#define RECOGNITION

#define OFFLINE

/*
 * Defination for the Algorithmen
 */
//#define SVDTRACK
#define UQTRACK

// frame choosen strategy with the percent of associate points
//#define FRAME1
// frame choosen strategy with compare of rotated angel
//#define FRAME2 
// frame choosen strategy with the percent of associate points and the average distance between them
#define FRAME3

//#define USINGICP

#define DEPTHFILTER

/*
 * Defination for the Evaluation
 */
//#define EVALUATION


//#define TEST

#ifndef _MT 
#define _MT 
#endif 

const int DATA_SIZE = 204*204;

DWORD ThreadIndex = 1001;

using namespace std;

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



MSG msg;
// To mark the status of the OpenGLWindow and ObjectWindow
bool bDone = false;
// To mark the status of the input process
bool isPause = false;
// To restrict the used time of the new data
bool isDataUsed = false;

int jumpedFeatures = 0;
// The parameter of the points association, which deside by the last frame
float associaterate = 0.02;

// The Context for 3D data Window
//HWND openGLhnd;	
OpenGLContext *p3DDataViewContext;
// The Context for Objekt structur Window
//HWND objGLhnd;
OpenGLContext *pObjViewContext;

OpenGLContext *pKoordViewContext;


// The Buffer for BildDatas, which is saving the newest BildData at last place and the oldest at the first place.
list<BildData*> bildDataBuffer;
// The difference between the index of the current frame and historical frame
int DETECTINGRATE = 2;

int MAXJUMPEDFEATURES = 5;


// framerate
int FRAMERATE = 30;

// The input path
const char *INPUTPATH = "TestRecognition";

char *OUTPUTPATH = "TestRecognition";
bool ISDATASAVED = true;

// Evaluation output path
char *EVAOUTPUTPATH = "FirstTest";

//int HISFRAMEINDEX = 3;

DistanceFilter *dFilter;

int frameIndex;

Object *obj;

Mat RR = Mat::eye(3,3,CV_32FC1);
Mat R = Mat::eye(3,3,CV_32FC1);
Mat T = Mat::zeros(3,1,CV_32FC1);


template<class T>
void findMaxSet(vector<vector<T>> sets, vector<T> &maxSet){
	int maxSize = 0;
	int maxSizeIndex = 0;
	for(int i=0;i<sets.size();i++){
		if(sets[i].size()>maxSize){
			maxSize = sets[i].size();
			maxSizeIndex = i;
		}
	}
	if(maxSize > 0){
		maxSet = sets[maxSizeIndex];
	}
}

// this function is called by a new thread 
void openGLThreadPorc( void *param ) 
{ 
	
	//BOOL done = FALSE;
	static OpenGLWinUI *pOpenGLWinUI = new OpenGLWinUI;
	p3DDataViewContext = new OpenGLContext;

	TlsSetValue(ThreadIndex, pOpenGLWinUI);

	EnterCriticalSection (&glInitCrs);

	if(!CreateOpenGLWindow("OpenGL Window", 0, 0, 900, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI, p3DDataViewContext)){
	//if((openGLhnd=CreateOpenGLWindow("OpenGL Window", 0, 0, 900, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI))==NULL){
		exit(0);
	}

	cout<<"Thread OpenGL running"<<endl;

	LeaveCriticalSection (&glInitCrs);

	// call the display function and send the data direct with the pointer of array as parameter
	//display(pOpenGLWinUI, disData, intData, ampData);
	//EnterCriticalSection(&pauseCrs);
	//display(pOpenGLWinUI, bildDataBuffer[0]);
	//display(pOpenGLWinUI, bildDataBuffer.back());
	//LeaveCriticalSection(&pauseCrs);

	//parameter to mark, whether the input process is in pause
	

	while(!bDone){
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
		//if(GetMessage(&msg, openGLhnd, 0, 0)){
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
			//display(pOpenGLWinUI, bildDataBuffer[0]);
			EnterCriticalSection (&calcCrs);
			display(pOpenGLWinUI, bildDataBuffer.back());
			SwapBuffers(p3DDataViewContext->hDC);
			LeaveCriticalSection (&calcCrs);
		}
		//display(pOpenGLWinUI, disData, intData, ampData);
		//EnterCriticalSection(&pauseCrs);
		//display(pOpenGLWinUI, bildDataBuffer[0]);
		//LeaveCriticalSection(&pauseCrs);
			
	}

	delete p3DDataViewContext;
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

	//EnterCriticalSection (&crs);

	arVideoCapStart();
	argMainLoop( NULL, keyEvent, mainLoop );

	//LeaveCriticalSection (&crs);


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
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);
	setDefaultLoadPath(INPUTPATH);

	//get the distance data for the first step
	//loadNormalDataFromFile("distance", 3, bildData->disData);
	BildData *temp = new BildData();
	loadAllDataFromFile(3, temp);
	//init a distance filter
	//dFilter = new DistanceFilter(temp->disData);
	dFilter = new DistanceFilter(temp);

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
		
		//loadNormalDataFromFile("distance", i, bildData->disData);
		//loadNormalDataFromFile("intensity", i, bildData->intData);
		//loadNormalDataFromFile("amplitude", i, bildData->ampData);

		BildData *temp = new BildData();
		loadAllDataFromFile(i, temp);

		// Pop the first element of the Buffer, and the destructor will be called automatically 
#ifdef FRAME1
		if(bildDataBuffer.size()>=DETECTINGRATE){
			//delete the first element from the list
			bildDataBuffer.pop_front();
		}
#endif

#ifdef FRAME2
		// if sepcial process begin, the old data would not be removed
		if(jumpedFeatures<1){
			bildDataBuffer.pop_front();
		}
#endif

#ifdef FRAME3
		// if sepcial process begin, the old data would not be removed
		if(jumpedFeatures<1){
			bildDataBuffer.pop_front();
		}
#endif
		bildDataBuffer.push_back(temp);

		//updata the OpenGL Window
		//PostMessage(openGLhnd, WM_PAINT, 0, 0);	
		PostMessage(p3DDataViewContext->hWnd, WM_PAINT, 0, 0);	
		isDataUsed = false;
		//setARData(intData);
		cout<<"Input running!"<<endl;
		LeaveCriticalSection(&frameCrs);
		LeaveCriticalSection(&pauseCrs);
		Sleep(FRAMERATE);
	}

#else
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);
	if(ISDATASAVED){
		createDefaultPMDDataDirectory(OUTPUTPATH);
		setIsDataSaved(true);
	}
	cout<<"PMD Camera Connecting..."<<endl;
	if(!createPMDCon()){
		exit(1);
		//cout<<"input thread running!"<<endl;
		
	}
	//Sleep(3000);
	//cout<<"input thread running!!!!"<<endl;


	//calibration();

	// init distance filter
	BildData *temp = new BildData();
	getPMDData(temp);
	dFilter = new DistanceFilter(temp);

	cout<<"Upgrading Distance Filter and Save Data into Buffer....."<<endl;
	for(int i=0;i<20;i++){
		BildData *temp = new BildData();
		getPMDData(temp);

		if(bildDataBuffer.size()>=DETECTINGRATE){
			//delete the first element from the list
			bildDataBuffer.pop_front();
		}

		bildDataBuffer.push_back(temp);

		dFilter->Upgrade(temp);
	}
	cout<<"Upgrading complete!"<<endl;

	LeaveCriticalSection (&glInitCrs);
	LeaveCriticalSection (&cvInitCrs);

	while(!bDone){
		EnterCriticalSection(&pauseCrs);
		EnterCriticalSection(&frameCrs);

		//setARData(intData);
		try {
			BildData *temp = new BildData();
			getPMDData(temp);

#ifdef FRAME1
			if(bildDataBuffer.size()>=DETECTINGRATE){
				//delete the first element from the list
				bildDataBuffer.pop_front();
			}
#endif

#ifdef FRAME2
		// if sepcial process begin, the old data would not be removed
		if(jumpedFeatures<1){
			bildDataBuffer.pop_front();
		}
#endif

#ifdef FRAME3
			// if sepcial process begin, the old data would not be removed
			if(jumpedFeatures<1){
				bildDataBuffer.pop_front();
			}
#endif
			bildDataBuffer.push_back(temp);

		} catch (CMemoryException * e){
			AfxMessageBox("Irgendwas ist schiefgegangen!");
		}

		isDataUsed = false;
		LeaveCriticalSection(&frameCrs);
		LeaveCriticalSection(&pauseCrs);
		Sleep(FRAMERATE);
	}
	closePMDCon();
#endif
}


void objWindowThreadPorc(void *param ){
	static OpenGLWinUI *pObjGLWinUI = new OpenGLWinUI;
	pObjViewContext = new OpenGLContext;

	TlsSetValue(ThreadIndex, pObjGLWinUI);

	

	if(!CreateOpenGLWindow("Object Window", 0, 0, 1500, 750, PFD_TYPE_RGBA, 0, pObjGLWinUI, pObjViewContext)){
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
			display(pObjGLWinUI, obj);
			SwapBuffers(pObjViewContext->hDC);
			glEnable(GL_LIGHTING);
			LeaveCriticalSection(&calcCrs);
		}
	}

	delete pObjViewContext;
	delete pObjGLWinUI;

}


// koordinaten window
void koordWindowThreadPorc(void *param ){
	static OpenGLWinUI *pKoordGLWinUI = new OpenGLWinUI;
	pKoordViewContext = new OpenGLContext;

	TlsSetValue(ThreadIndex, pKoordGLWinUI);

	if(!CreateOpenGLWindow("Coordinaten System Window", 500, 0, 600, 600, PFD_TYPE_RGBA, 0, pKoordGLWinUI, pKoordViewContext)){
		exit(0);
	}

	cout<<"The Koordinatensystem Window is running"<<endl;

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
			//EnterCriticalSection (&calcCrs);
			displayCoord(pKoordGLWinUI, RR);
			SwapBuffers(pKoordViewContext->hDC);
			//glEnable(GL_LIGHTING);
			//LeaveCriticalSection(&calcCrs);
		}
	}
	delete pKoordViewContext;
	delete pKoordGLWinUI;
}



int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	//just for the situation, that more than one threads are calling the same data at the same time
	InitializeCriticalSection (&pauseCrs);
	InitializeCriticalSection (&frameCrs);
	InitializeCriticalSection (&glInitCrs);
	InitializeCriticalSection (&cvInitCrs);
	InitializeCriticalSection (&calcCrs);

	// MFC initialisieren und drucken. Bei Fehlschlag Fehlermeldung aufrufen.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: Den Fehlercode an Ihre Anforderungen anpassen.
		_tprintf(_T("Schwerwiegender Fehler bei der MFC-Initialisierung\n"));
		nRetCode = 1;
	}
	else
	{
		//Start Input Thread
		if(_beginthread (inputThreadProc, 0, NULL) == -1){
			cout<<"Failed to create input thread"<<endl;
		}

		 //Start OpenGL Window Thread 
		if(_beginthread (openGLThreadPorc, 0, NULL)==-1){
			cout<<"Failed to create openGL thread"<<endl;
		}

		//Start Obj Window Thread 
		if(_beginthread (objWindowThreadPorc, 0, NULL)==-1){
			cout<<"Failed to create ObjWindow thread"<<endl;
		}

		//Start Koordinatensystem Window Thread 
		//if(_beginthread (koordWindowThreadPorc, 0, NULL)==-1){
		//	cout<<"Failed to create KoordWindow thread"<<endl;
		//}
		// Start ARToolKit Window Thread 
		//if(_beginthread (arToolKitThreadProc, 0, NULL)==-1){
		//	cout<<"Failed to create ARToolKit thread"<<endl;
		//}

		MSG msg;
		//BOOL done = FALSE;
		


		//LeaveCriticalSection (&glInitCrs);

		//Main Thread
		//using cv namespace
		{
			using namespace cv; 
			using namespace std;

			//Object *testObj = new Object();
			//testObj->loadFromVTKFile("Test2");

			//namedWindow("OpenCVGrayScale", CV_WINDOW_AUTOSIZE);
			namedWindow("OpenCVCorrespondenz", CV_WINDOW_AUTOSIZE);
			//namedWindow("OpenCVFeatures", CV_WINDOW_AUTOSIZE);
			namedWindow("OpenCVSummedFeatures", CV_WINDOW_AUTOSIZE);
			Size size = Size(204, 204);
			Mat img, featuresImg, testImg;
			//bool isPause = false;
			int balance = 200;
			float contrast = 10;


			double energie = 0;
			float maxValue = 0;



			//Parameter for STAR Detector
			//for Markers 
			//int MINFEATURECOUNT = 24;
			//int MAXFEATURECOUNT = 45;
			//for Objects
			int MINFEATURECOUNT = 68;//42;
			int MAXFEATURECOUNT = 96;//65;
			int MAXDETECTLOOPS = 30;

			int MAXSIZE = 8;
			//int RESPONSETHRESHOLD = 70;
			int detecParam = 70;
			int LINETHRESHOLDPROJECTED = 5;
			int LINETHRESHOLDBINARIZED = 6;
			int SUPPRESSNONMAXSIZE = 1;
			int MAXLOOPS = 30;

			MyFeatureDetector myFeatureDetector = MyFeatureDetector(MINFEATURECOUNT, MAXFEATURECOUNT, MAXDETECTLOOPS);
			

			//Test
			int testP1 = 5;
			int testP2 = 6;
			int testP3 = 1;

			

			//The summerized KeyPoint fot the current frame
			vector<KeyPoint> sumFeatures;

			unsigned char hisShowdata[41616*3];

			//obj = new Graph();
			obj = new Object();


			// Parameters for the frame choice
			float memAngle = 360;
			float memAvrDis = numeric_limits<float>::max();
			float memDisPE = numeric_limits<float>::max();
			float memSumP = -1;
			Mat memR, memT;
			vector<Point3f> memNewResult;
			int memIndex = 0;

			// init Kalman filter
			initKalmanFilter();
			initQKFilter();

			Recognition *recognition = new Recognition();

#ifdef EVALUATION
			// Evaluation
			Evaluation evaluation = Evaluation();
			evaluation.createCVSFile("DetectionRate");
#endif
			while (!bDone) 
			{ 	
				if(isPause){
					continue;
				}

				if(isDataUsed){
					continue;
				}
				//call the calculate function after the init of PMD Camera
				EnterCriticalSection (&cvInitCrs);
				EnterCriticalSection (&calcCrs);

				EnterCriticalSection(&frameCrs);

				//get the current Bild Data
				//BildData *currentBildData = bildDataBuffer[0];
				//get the current Bild Data, which is saved at the last position of the List
				BildData *currentBildData = bildDataBuffer.back();
				
				// Gaussian Filter
				filterDepthDate(currentBildData->threeDData, 0.3);

				// create the data for a RGB image
				unsigned char showdata[41616*3];
				

				//transFloatTo3Char(currentBildData->ampData, showdata, balance, contrast);
				//transFloatTo3Char(bildDataBuffer[bildDataBuffer.size()-1]->ampData, hisShowdata, balance, contrast);
				//transFloatTo3Char(bildDataBuffer[bufferIterator]->ampData, hisShowdata, balance, contrast);

				transFloatTo3Char(currentBildData->ampData, showdata, balance, contrast);
				transFloatTo3Char(bildDataBuffer.front()->ampData, hisShowdata, balance, contrast);

				// set the data to the RGB image
				Mat curMat = Mat(size, CV_8UC3, showdata);
				curMat.copyTo(featuresImg);

				Mat hisMat = Mat(size, CV_8UC3, hisShowdata);

				
#ifdef DEPTHFILTER
				// create the array to save the filted data
				float filteData[204*204];
				//bool isDiff = dFilter->Filte(currentBildData->disData, currentBildData->ampData, filteData);
				bool isDiff = dFilter->Filte(currentBildData, filteData);
#endif

				
				//memcpy(filteData, currentBildData->ampData, sizeof(currentBildData->ampData));
				

				/********************************************************
				 *
				 * Using K-means algorithm to calibrate the differen objects
				 *
				 *******************************************************/
#ifdef KMEAN
				// 3D Daten
				//int fDSize = currentBildData->filted3DData.size();
				//Mat samples = Mat(currentBildData->filted3DData, true);
				//int clusterCount = 5;
				//Mat centers = Mat(5,1,CV_32FC3);

				// 2D Daten
				int fDSize = currentBildData->filted2DData.size();
				Mat samples = Mat(currentBildData->filted2DData, true);
				int clusterCount = 4;
				Mat centers = Mat(clusterCount,1,CV_32FC2);

				currentBildData->clusterLabel = Mat::zeros(fDSize, 1, CV_8UC1);
				TermCriteria termcrit = TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0);
				

				kmeans(samples, clusterCount, currentBildData->clusterLabel, termcrit, 3, KMEANS_RANDOM_CENTERS, centers);

				cout<<"The centers are: "<<centers<<endl;

				if(obj->fixNodeCount>3){
					Point3f midNode = obj->getMiddelPoint();
					float minDis = numeric_limits<float>::max();
					int minCluster = 0;
					for(int i=0;i<clusterCount;i++){
						centers = centers.reshape(1);
						//Point3f center = Point3f(centers.at<float>(i,0), centers.at<float>(i,1), centers.at<float>(i,2));
						//float currDis = getEuclideanDis(center, midNode);
						// 2D
						float currDis = sqrt((centers.at<float>(i,0)-midNode.x)*(centers.at<float>(i,0)-midNode.x) + 
											 (centers.at<float>(i,1)-midNode.y)*(centers.at<float>(i,1)-midNode.y));
						if(currDis < minDis){
							minCluster = i;
							minDis = currDis;
						}
					}
					cout<<"The min Cluster is: "<<minCluster<<endl;
					for(int i=0;i<204*204;i++){
						filteData[i] = 0;
					}
					for(int i=0;i<fDSize;i++){
						if(currentBildData->clusterLabel.at<int>(i,0) == minCluster){
							filteData[currentBildData->filtedPointIndex[i]] = currentBildData->ampData[currentBildData->filtedPointIndex[i]];
						}
					}

				}

#endif
				//if(!isDiff) continue;


				vector<KeyPoint> features;
				
				unsigned char data[41616];

				int safeCount = 0;

				/****************************************************************
				 *
				 * The Loop for the Features' Detection. The result is just a between solution
				 *
				 ***************************************************************/
				
				cout<<endl<<"================= Begin the Loop of the Brightness Controll =================="<<endl;
#ifdef DEPTHFILTER				
				if(isDiff){
					
					myFeatureDetector.setDetectedData(filteData);
					myFeatureDetector.usingSTAR();
					features = myFeatureDetector.keypoints;

					//saveNormalDataToPNG(INPUTPATH, frameIndex, myFeatureDetector.drawMat);
				}
#else 
				myFeatureDetector.setDetectedData(currentBildData->ampData);
				myFeatureDetector.usingSTAR();
				features = myFeatureDetector.keypoints;
#endif
				
//#ifndef KMEAN
//				while(safeCount < MAXLOOPS){
//					
//					safeCount ++;
//#endif	
//					//Sleep(500);
//					// translate the filted data to a grayscale image 
//					transFloatToChar(filteData, data, balance, contrast);
//					//for(int i=0;i<204*204;i++){
//					//	data[i] = 255-data[i];
//					//}
//					img = Mat(size, CV_8UC1, data);
//					
//					features.clear();
//					//set the parameter of the STAR Detector
//					StarDetector detector = StarDetector(MAXSIZE, detecParam, LINETHRESHOLDPROJECTED, LINETHRESHOLDBINARIZED, SUPPRESSNONMAXSIZE);
//					detector(img, features);
//
//
//					
//					//SURF surfDetector = SURF(500, 1, 2, false);
//					//Mat mask;
//					//vector<float> descriptors;
//					//surfDetector(img, mask, features,descriptors); 
//					////draw features
//					int vectorSize = features.size();
//					cout<<"find "<<features.size()<<" features!"<<endl;
//					for(int i=0;i<features.size();i++){
//						circle(featuresImg, features[i].pt, 1, Scalar(0,0,255,0), -1); 
//					//	CvPoint center;
//					//	int radius;
//					//	center.x = cvRound(features[i].pt.x);
//					//	center.y = cvRound(features[i].pt.y);
//					//	radius = cvRound(descriptors[i]);
//					//	circle(featuresImg, center, radius, Scalar(0,0,255,0), 1, 8, 0);
//					}
//
//
//					//CvMemStorage* storage = cvCreateMemStorage(0);
//					////Define sequence for storing surf keypoints and descriptors
//					//CvSeq *imageKeypoints = 0, *imageDescriptors = 0;
//
//					////Extract SURF points by initializing parameters
//					//CvSURFParams params = cvSURFParams(100, 2);
//					//cvExtractSURF( &IplImage(img), 0, &imageKeypoints, &imageDescriptors, storage, params );
//					////printf("Image Descriptors: %d\n", imageDescriptors->total);
//
//					////draw the keypoints on the captured frame
//					//for(int i = 0; i < imageKeypoints->total; i++ )
//					//{
//					//	CvSURFPoint* r = (CvSURFPoint*)cvGetSeqElem( imageKeypoints, i );
//					//	if(r->size > 25){
//					//		CvPoint center;
//					//		int radius;
//					//		center.x = cvRound(r->pt.x);
//					//		center.y = cvRound(r->pt.y);
//					//		radius = cvRound(r->size*1.2/9.*2);
//					//		cout<<"The descriptor: "<<r->size<<endl;
//					//		cvCircle( &IplImage(featuresImg), center, radius, Scalar(0,0,255,0), 1, 8, 0 );
//					//		KeyPoint *kPoint = (KeyPoint*)cvGetSeqElem( imageKeypoints, i );
//					//		features.push_back(*kPoint);
//					//	}
//					//}
//					//	
//					//int vectorSize = features.size();
//
//					//draw historical features
//					//if(hisFeatures.size()>0){
//					//	for(int i=0;i<hisFeatures.size();i++){
//					//		circle(testImg, hisFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
//					//	}
//					//}
//
//					//draw
//					imshow("OpenCVGrayScale", img);
//					imshow("OpenCVFeatures", featuresImg);
//					
//					/**********************************
//					 *
//					 * The Distance Filter
//					 *
//					 *********************************/
//#ifndef KMEAN
//					if(!isDiff) break;
//
//					if(!brightnessControll(vectorSize, contrast, detecParam, data)){
//						break;
//					}
//					
//				} // Ende of the Loop for the Features' Detection
//#endif
				cout<<"=================================== End  ===================================="<<endl<<endl;
				
				/***********************************
				 *
				 * Summarize the near features
				 * 
				 * Because the last parameter of STAR detector SUPPRESSNONMAXSIZE set as 1, more features will be found as expected.
				 * The close features will be set as one feature
				 *
				 ***********************************/
				//sumFeatures.clear();
				//sumFeatures = features;
				//The loop of all features
				//for(int i=0;i<sumFeatures.size();i++){
				//	//The loop from current features to the others, which is behind the current feature
				//	for(int j=i+1;j<sumFeatures.size();j++){
				//		//calculate the distance between two features
				//		float xDis = fabs(sumFeatures[i].pt.x - sumFeatures[j].pt.x);
				//		float yDis = fabs(sumFeatures[i].pt.y - sumFeatures[j].pt.y);
				//		//if they are too close
				//		//if(xDis<eps && yDis<eps){
				//		if((xDis*xDis + yDis*yDis)<eps*eps){
				//			sumFeatures[i].pt.x = 0.5*(sumFeatures[i].pt.x + sumFeatures[j].pt.x);
				//			sumFeatures[i].pt.y = 0.5*(sumFeatures[i].pt.y + sumFeatures[j].pt.y);
				//			//delete the feature at jth position
				//			sumFeatures.erase(sumFeatures.begin() + j);
				//			//j=i+1;
				//		}
				//		//cout<<xDis<<"  ";
				//	}
				//	//circle(testImg, sumFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
				//	circle(left, sumFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
				//	circle(right, sumFeatures[i].pt, 1, Scalar(0,0,255,0), -1);

				//	//translate vector from left to right
				//	Point2f trans(204, 0);
				//	line(testImg, sumFeatures[i].pt, sumFeatures[i].pt+trans, Scalar(255, 255, 0, 0));
				//}
				//cout<<"After Summerize get "<<sumFeatures.size()<<" features!"<<endl;
				//int vectorSize = sumFeatures.size();

				/**************************************************
				 * 
				 * Test for the greate meng of SVD
				 *
				 *************************************************/
				// save all features without calibration into the data
				//currentBildData->comFeatures.clear();
				//currentBildData->comFeatures2D.clear();
				//for(int i=0;i<features.size();i++){
				//	int indexI = int(features[i].pt.y);
				//	int indexJ = int(features[i].pt.x);
				//	float x = currentBildData->threeDData[(indexI*204 + indexJ)*3];
				//	float y = currentBildData->threeDData[(indexI*204 + indexJ)*3 +1];
				//	float z = currentBildData->threeDData[(indexI*204 + indexJ)*3 +2];
				//	Point3f tdp = Point3f(x,y,z);
				//	//tdFeatures.push_back(tdp);
				//	currentBildData->comFeatures.push_back(tdp);
				//	currentBildData->comFeatures2D.push_back(Point2f(indexJ, indexI));
				//}

				////vector<Point3f> hisComFeatures = bildDataBuffer[bildDataBuffer.size()-1]->comFeatures;
				//vector<Point3f> hisComFeatures = bildDataBuffer.front()->comFeatures;
				//vector<Point3f> oldComResult;
				//vector<Point3f> newComResult;

				//cout<<"======= "<<isDataUsed<<" ======= "<<hisComFeatures.size()<<" ======= "<<currentBildData->comFeatures.size()<<endl<<endl;
				//if(hisComFeatures.size()>0 && currentBildData->comFeatures.size()>0){
				//	featureAssociate2(hisComFeatures, currentBildData->comFeatures, 15, oldComResult, newComResult);
				//	
				//	//SVDFindRAndT(oldComResult, newComResult, R, T);
				//	UQFindRAndT(oldComResult, newComResult, R, T);
				//	cout<<"The rotation matrix is: "<<R<<endl<<endl;
				//	cout<<"The translation matrix is: "<<T<<endl<<endl;
	
				//	isDataUsed = true;
				//	//cout<<"The loop Count is: "<<isDataUsed<<endl;

				//	for(int i=0;i<oldComResult.size();i++){
				//		Point2f trans(204,0);
				//		line(testImg, point3To2(oldComResult[i]), point3To2(newComResult[i])+trans, Scalar(255,255,0,0));
				//	}
				//	cout<<"The number of useful features is: "<<oldComResult.size()<<endl;
				//}
						

				/**************************************************
				 *
				 * A new Algorithm to make sure, that every marker will be as a just one KeyPoint recognized
				 *
				 * 1. Spread the features to different set, which are close to each others. The parameter eps set the size of the area
				 * 2. Calculate the middel point of each set and show them
				 * 3. Save the resault into the vector: SummerizedFeatures
				 *
				 *************************************************/
				// to save the sets and the elements of set
				//vector<vector<KeyPoint>> groupFeatures;
				float eps = 5;
				// first calibration for the original features, which deal with the 2D point
				//calibration2D(groupFeatures, features, eps);

				// Using DBSCAN to calirate the points
				vector<vector<Point3f>> groupFeatures;
				if(features.size()>0){
					vector<Point3f> featuresCali3D;
					for(int i=0;i<features.size();i++){
						Point3f tempPoint(features[i].pt.x, features[i].pt.y, 0);
						featuresCali3D.push_back(tempPoint);
					}
					DBSCAN(groupFeatures, featuresCali3D, 5, 2);
				}
				cout<<"After Summerize get "<<groupFeatures.size()<<" features!"<<endl;

				vector<PMDPoint> summerizedFeatures; 
				for(int i=0;i<groupFeatures.size();i++){
					float avrX2D = 0;
					float avrY2D = 0;

					float avrX3D = 0;
					float avrY3D = 0;
					float avrZ3D = 0;
					int groupSize = groupFeatures[i].size();
					for(int j=0;j<groupSize;j++){
						//TODO: Why is the x and y exchanged
						//int indexI = int(groupFeatures[i][j].pt.y);
						//int indexJ = int(groupFeatures[i][j].pt.x);
						int indexI = int(groupFeatures[i][j].y);
						int indexJ = int(groupFeatures[i][j].x);

						avrX2D += indexJ;
						avrY2D += indexI;

						avrX3D += currentBildData->threeDData[(indexI*204 + indexJ)*3];
						avrY3D += currentBildData->threeDData[(indexI*204 + indexJ)*3 + 1];
						avrZ3D += currentBildData->threeDData[(indexI*204 + indexJ)*3 + 2];
					}

					Point2f p2(avrX2D/groupSize, avrY2D/groupSize);
					Point3f p3(avrX3D/groupSize, avrY3D/groupSize, avrZ3D/groupSize);
					
					//currentBildData->features2D.push_back(p2);
					////currentBildData->features.push_back(p3);
					//summerizedFeatures.push_back(p3);
					
					PMDPoint newPMDPoint = PMDPoint(p3,p2);
					summerizedFeatures.push_back(newPMDPoint);
				}

				vector<PMDPoint> allFeatures; 
				for(int i=0;i<features.size();i++){
					int indexI = int(features[i].pt.y);
					int indexJ = int(features[i].pt.x);
					float x = currentBildData->threeDData[(indexI*204 + indexJ)*3];
					float y = currentBildData->threeDData[(indexI*204 + indexJ)*3 +1];
					float z = currentBildData->threeDData[(indexI*204 + indexJ)*3 +2];
					Point2f p2 = Point2f(indexJ, indexI);
					Point3f p3 = Point3f(x,y,z);
					PMDPoint newPMDPoint = PMDPoint(p3,p2);
					allFeatures.push_back(newPMDPoint);
				}

#ifdef RECOGNITION
				recognition->objectRecognition(summerizedFeatures);
#else

				/*************************************************
				 *
				 * Second Calibration to seprate different objects
				 *
				 *     assume now just on object
				 * TODO: for more objects expand
				 *
				 *************************************************/

				vector<vector<PMDPoint>> caliResult;
				//eps for my calibration
				float CALIEPS3D = 0.17;
				int minPts = 2;

				

				//Call calibration
				//if(obj->fixNodeCount>=3){
				//	Point3f center;
				//	obj->getMiddelPoint(center);
				//	DBSCANPMDPoint(caliResult, summerizedFeatures, center, CALIEPS3D, minPts);
				//} else {
					calibrationPMDPoint(caliResult, summerizedFeatures, CALIEPS3D);
				//}

//TODO: fix the problem of DBSCAN
				//if(summerizedFeatures.size()>0){
				//	DBSCANPMDPoint(caliResult, summerizedFeatures, 0.3, minPts);
				//}
				// Save the features into the first place of data buffer
				currentBildData->features.clear();
				findMaxPMDPointSet(caliResult, currentBildData->features);

				
				vector<vector<PMDPoint>> caliComResult;
				calibrationPMDPoint(caliComResult, allFeatures, CALIEPS3D);

				// Save the features into the first place of data buffer
				currentBildData->comFeatures.clear();
				findMaxPMDPointSet(caliComResult, currentBildData->comFeatures);

				/***************************************************
				 *
				 * Compare the historical features and the current features, in order to find the transformation of the object
				 *
				 * 1. Using FeatureAssociate to find the corresponding features in two frames
				 * 2. Using Unit Quartnion to find the rotation and traslation matrix of the object
				 * 3. Judge the transformation. Ignore the big noised frame
				 *
				 ***************************************************/
				//The loop of the historical data, in oder to remove the "bad" frames
				//cout<<"==================== Begin the loop of historical data ==========================="<<endl;

				
				// The current Features
				vector<PMDPoint> curFeatures = currentBildData->features;	
				// The historical Features
				vector<PMDPoint> hisFeatures = bildDataBuffer.front()->features;
				// The vector to save the points of SVC Association				
				vector<PMDPoint> oldResult, newResult;
	
				//Mat R = Mat(3,3,CV_32FC1);
				//Mat T = Mat(3,1,CV_32FC1);

#ifdef USINGICP
if(obj->fixNodeCount<=3){
// original
#endif
				cout<<"======= "<<isDataUsed<<" ======= "<<hisFeatures.size()<<" ======= "<<curFeatures.size()<<endl<<endl;

				if(hisFeatures.size()>0 && curFeatures.size()>0){
					float avrDis, disPE, sumP;
					bool isAssSuccess = featureAssociatePMD(hisFeatures, curFeatures, associaterate, oldResult, newResult, avrDis, disPE, sumP);
					//cout<<"Beforrrrrrrrrrrrrr: "<<oldResult.size()<<endl;
					//calibrationWithDistance(oldResult, newResult, 0.001);
					//cout<<"Afterrrrrrrrrrrrrr: "<<oldResult.size()<<endl;
					//bool isAssSuccess = featureAssociate(hisFeatures, curFeatures, associaterate, oldResult, newResult, oldIndexResult, newIndexResult, avrDis, disPE);
					// if the P is nan, restart the feature association with the smaller associate rate
					//if(_isnan(disPE)){
					//	associaterate = 0.02;
					//	featureAssociate(hisFeatures, curFeatures, associaterate, oldResult, newResult, oldIndexResult, newIndexResult, avrDis, disPE);
					//}
					//int assCount = 0;
					//do{
					//	featureAssociate(hisFeatures, curFeatures, associaterate, oldResult, newResult, oldIndexResult, newIndexResult, avrDis, disPE);
					//	assCount++;
					//}while(_isnan(disPE) && assCount<5);
					//if(assCount == 5) cout<<"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFeld!"<<endl;
	

					if(avrDis < 0.02){			
						associaterate = avrDis;
					} else {
						associaterate = 0.022;
					}
					//associaterate = 0.02;
					//calibrationWithDistance(oldResult, newResult);


					cout<<"The number of useful features is: "<<oldResult.size()<<endl;
#ifdef SVDTRACK
					SVDFindRAndT(oldResult, newResult, R, T);
					cout<<"The rotation matrix is: "<<R<<endl<<endl;
					cout<<"The translation matrix is: "<<T<<endl<<endl;
					obj->updateGraph(newResult, R, T);
					isDataUsed = true;
					cout<<"The loop Count is: "<<isDataUsed<<endl;
				}
				imshow("OpenCVCorrespondenz", testImg);					
#endif

						
#ifdef UQTRACK		
					vector<Point3f> old3DPointResult, new3DPointResult;
					for(int i=0;i<oldResult.size();i++){
						old3DPointResult.push_back(oldResult[i].coord);
						new3DPointResult.push_back(newResult[i].coord);
					}
					// get the rotated angle
					//float angle = UQFindRAndT(oldResult, newResult, R, T);
					float angle = UQFindRAndT(old3DPointResult, new3DPointResult, R, T);
					cout<<"The rotation matrix is: "<<R<<endl<<endl;
					cout<<"The translation matrix is: "<<T<<endl<<endl;

					

					// The second Limination
					// consider the angle of rotation 
					
					
#ifdef FRAME2
					vector<Point3f> cur3DPointFeatures;
					for(int i=0;i<curFeatures.size();i++){
						cur3DPointFeatures.push_back(curFeatures[i].coord);
					}
					if(isAssSuccess){
						cout<<"The sum of nonzero elements of Matrix P is: "<<sumP<<endl;

						obj->updateGraph(cur3DPointFeatures, R, T);
						RR = R*RR;
						// if there is minimal 1 feature jumped, remove the unused frame
						if(jumpedFeatures > 0){
							bildDataBuffer.erase(--(--bildDataBuffer.end()));
							jumpedFeatures = 0;
							memAvrDis = numeric_limits<float>::max();
							memSumP = -1;
						}
					} else {
						cout<<"The frame has big noise, and will be throw out!"<<endl;
						// if the noised frame has a smaller distance between the associated points
						if(sumP > memSumP || _isnan(disPE)){
							memIndex = frameIndex;
							memAvrDis = disPE;
							if(_isnan(disPE)){
								memSumP = -1;
							} else {
								memSumP = sumP;
							}

							R.copyTo(memR);
							T.copyTo(memT);
						
							//TODO: using all features to update object
							memNewResult = cur3DPointFeatures;
							// if this noised frame is not the first frame, that means, there is already a noised frame before this frame 
							if(jumpedFeatures > 0){
								// remove the frame before
								bildDataBuffer.erase(--(--bildDataBuffer.end()));
							}
						} else {
							bildDataBuffer.pop_back();
						}
						cout<<"Jumped! The jumped features: "<<jumpedFeatures<<endl;
						jumpedFeatures++;

						if(jumpedFeatures>MAXJUMPEDFEATURES){
							cout<<"The frame has noise, but the maximal number of the allowed jumped frames are arrived!"<<endl;
							cout<<"The frmae "<<memIndex<<" has been choice to get an approximation"<<endl;
						
							RR = memR*RR;
							RR.copyTo(obj->R);
							obj->updateGraph(memNewResult, memR, memT);

							jumpedFeatures = 0;
							memAvrDis = numeric_limits<float>::max();
							memSumP = -1;
						}

					}
#endif

#ifdef FRAME1
				//TODO: using the choosen strategy
				bool isBig = isBigNoised2(obj, newResult, R, T, 0.35, 0.008);
				if(!isBig || jumpedFeatures>=MAXJUMPEDFEATURES){
					if(!isBig){
						cout<<"The frame is not noised: "<<endl;
					} else {
						cout<<"The frame has noise, but the maximal number of the allowed jumped frames are arrived!"<<endl;
					}
					obj->updateGraph(newResult, R, T);
					RR = R*RR;
					jumpedFeatures = 0;
				} else {
					cout<<"The frame has big noise, and will be throw out!"<<endl;
					bildDataBuffer.pop_back();
					cout<<"Jumped! The jumped features: "<<jumpedFeatures<<endl;
					jumpedFeatures++;
				}
#endif

#ifdef FRAME3
					vector<Point3f> cur3DPointFeatures;
					for(int i=0;i<curFeatures.size();i++){
						cur3DPointFeatures.push_back(curFeatures[i].coord);
					}
					//if(frameIndex == 78) 
					//	cout<<endl;
					float corresRate = getCorresRate(obj, new3DPointResult, R, T, 0.005);
					if(corresRate > 0.35 && isAssSuccess){
						cout<<"The frame is not noised: "<<endl;
						RR = R*RR;
						RR.copyTo(obj->R);
						//TODO: using all features to update object
						obj->update(cur3DPointFeatures, R, T);
						//MAXJUMPEDFEATURES = 3;

						if(jumpedFeatures > 0){
							bildDataBuffer.erase(--(--bildDataBuffer.end()));
							jumpedFeatures = 0;
							memAvrDis = numeric_limits<float>::max();
							memDisPE = numeric_limits<float>::max();
							memSumP = -1;
						}
					} else {
						cout<<"The frame has big noise, and will be throw out!"<<endl;
						if(!isAssSuccess){
							avrDis = numeric_limits<float>::max()/10;
							disPE = numeric_limits<float>::max()/10;
							sumP = 0;
							//MAXJUMPEDFEATURES++;
							//cout<<"The Maximal JumpedFeatures are new defined!"<<MAXJUMPEDFEATURES<<endl;
						}
						// if the noised frame has a smaller distance between the associated points
						//if(avrDis < memAvrDis){
						//if(disPE < memDisPE){
						if(sumP > memSumP){
							memIndex = frameIndex;
							memAvrDis = avrDis;
							memDisPE = disPE;
							memSumP = sumP;

							memAngle = angle;
							R.copyTo(memR);
							T.copyTo(memT);
							
							//TODO: using all features to update object
							memNewResult = cur3DPointFeatures;

							// if this noised frame is not the first frame, that means, there is already a noised frame before this frame 
							if(jumpedFeatures > 0){
								// remove the frame before
								bildDataBuffer.erase(--(--bildDataBuffer.end()));
							}
						} else {
							bildDataBuffer.pop_back();
						}
						cout<<"Jumped! The jumped features: "<<jumpedFeatures<<endl;
						jumpedFeatures++;

						if(jumpedFeatures>MAXJUMPEDFEATURES){
							cout<<"The frame has noise, but the maximal number of the allowed jumped frames are arrived!"<<endl;
							cout<<"The frmae "<<memIndex<<" has been choice to get an approximation"<<endl;
							//cout<<"The selected Rotationmatrix and Translationmatrix are: "<<endl;
							//cout<<"memR = "<<memR<<endl;
							//cout<<"memT = "<<memT<<endl;
							//if(memAngle > 35 && obj->fixNodeCount > 1){
							//	cout<<"Even the maximal number of jumped frame is arrived, because of the too big rotated angle, the choosed frame will be still throw out!"<<endl;
							//	jumpedFeatures--;
							//} else {
							//cout<<"The current associate rate is: "<<associaterate<<endl;
							RR = memR*RR;
							RR.copyTo(obj->R);
							obj->update(memNewResult, memR, memT);


							jumpedFeatures = 0;
							memAvrDis = numeric_limits<float>::max();
							memDisPE = numeric_limits<float>::max();
							memSumP = -1;
							//MAXJUMPEDFEATURES = 3;
							//}
						}

					}
#endif

					//obj->updateGraph(newResult, R, T);
					//float traceR = obj->R.at<float>(0,0) + obj->R.at<float>(1,1) + obj->R.at<float>(2,2);
					//float angleR = acos((traceR-1)/2);
					////graph->nodeList[i]->color = int(fabs(angle)/90);
					//cout<<"The total rotated angle: "<<angleR*180/3.14<<endl;
					Vec3d euler1, euler2;
					calcEulerAngleFromR(obj->R, euler1, euler2);
					cout<<"The total rotated angle 1: "<<euler1[0]*180/3.14<<" , "<<euler1[1]*180/3.14<<" , "<<euler1[2]*180/3.14<<" , "<<endl;
					cout<<"The total rotated angle 2: "<<euler2[0]*180/3.14<<" , "<<euler2[1]*180/3.14<<" , "<<euler2[2]*180/3.14<<" , "<<endl;
				}
#ifdef USINGICP
} else {
// ICP
				//vector<PMDPoint> hisICPData, curICPData;
				//int boxSize = 1;
				//createBoundingPoints(curICPData, curFeatures, currentBildData, boxSize);
				//createBoundingPoints(hisICPData, hisFeatures, bildDataBuffer.front(), boxSize);
				//bool isICPSuccess = ICP(hisICPData, curICPData, R, T, oldResult, newResult);

				//bool isICPSuccess = ICP(hisFeatures, summerizedFeatures, R, T, oldResult, newResult);
				
				//Using the complete features
				vector<PMDPoint> hisComFeatures = bildDataBuffer.front()->comFeatures;
				bool isICPSuccess = ICP(hisComFeatures, allFeatures, R, T, oldResult, newResult);

				//currentBildData->features = newResult;
				// The current Features
				//curFeatures = newResult;	

				//if(isICPSuccess || jumpedFeatures>=MAXJUMPEDFEATURES){
				//	if(isICPSuccess){
				//		cout<<"The frame is not noised: "<<endl;
				//	} else {
				//		cout<<"The frame has noise, but the maximal number of the allowed jumped frames are arrived!"<<endl;
				//	}
				if(summerizedFeatures.size()>=5 || jumpedFeatures>=MAXJUMPEDFEATURES){
					vector<Point3f> ICPFeatures3D;
					vector<Point2f> ICPFeatures2D;
					//decPMDPointVector(newResult, ICPFeatures3D, ICPFeatures2D);
					decPMDPointVector(curFeatures, ICPFeatures3D, ICPFeatures2D);
					obj->updateGraph(ICPFeatures3D, R, T);	
					jumpedFeatures = 0;
				} else {
					cout<<"The frame has too less features!"<<endl;
					bildDataBuffer.pop_back();
					cout<<"Jumped! The jumped features: "<<jumpedFeatures<<endl;
					jumpedFeatures++;
				}
}
#endif



				/********************************************
				 *
				 * OpenCV Show Windows
				 *
				 ********************************************/
				//vector<Point2f> curFeatures2D = currentBildData->features2D;
				//vector<Point2f> hisFeatures2D = bildDataBuffer.front()->features2D;


				// set the new historical fram into the left OpenCV window 
				//transFloatTo3Char(bildDataBuffer[bufferIterator]->ampData, hisShowdata, balance, contrast);
				//Mat hisImg2 = Mat(size, CV_8UC3, hisShowdata);
				//hisImg2.copyTo(left);

				
				testImg = Mat(204, 204*2, CV_8UC3);

				Mat left = Mat(testImg, Range::all(), Range(0,204));
				Mat right = Mat(testImg, Range::all(), Range(204,204*2));

				hisMat.copyTo(left);
				curMat.copyTo(right);

				for(int i=0;i<hisFeatures.size();i++){
					// show the old features
					circle(left, hisFeatures[i].index, 2, Scalar(0,255,0,0), -1);
				}

				//for(int i=0;i<summerizedFeatures.size();i++){
				//	// show the old features
				//	circle(right, summerizedFeatures[i].index, 2, Scalar(0,0,255,0), -1);
				//}
				for(int i=0;i<curFeatures.size();i++){
					// show the old features
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

				Mat graphImg;
				curMat.copyTo(graphImg);
				//for(int k=0;k<caliResult.size();k++){
				//	int firstSize = caliResult[k].size();
				//	for(int i=0;i<firstSize;i++){
				//		circle(graphImg, caliResult[k][i].index, 2, Scalar(0,0,255), -1);
				//		for(int j=i;j<firstSize;j++){
				//			line(graphImg, caliResult[k][i].index, caliResult[k][j].index, Scalar(0, 255, 255, 0));
				//		}
				//	}
				//}
				for(int i=0;i<curFeatures.size();i++){
					circle(graphImg, curFeatures[i].index, 2, Scalar(0,0,255), -1);
					for(int j=i;j<curFeatures.size();j++){
						line(graphImg, curFeatures[i].index, curFeatures[j].index, Scalar(0, 255, 255, 0));
					}
				}
				imshow("OpenCVSummedFeatures", graphImg);

				//cout<<"==================== End the loop of historical data ==========================="<<endl;
// für Quaternion Rotation
#endif	
	
// für Recognition
#endif
				isDataUsed = true;
				//cout<<"The loop Count is: "<<isDataUsed<<endl;
#ifdef EVALUATION
				if(frameIndex == 145 || 
				   frameIndex == 175 ||
				   frameIndex == 214 ||
				   frameIndex == 238 ||
				   frameIndex == 253 ||
				   frameIndex == 275 ||
				   //frameIndex == 227 ||
				   frameIndex == 307){
					   stringstream ss;
					   ss<<frameIndex;
					   string evaPath = ss.str();
					   evaluation.saveCVBild(evaPath.data(), right);
					   evaPath.append("_1");
					   evaluation.saveCVBild(evaPath.data(), myFeatureDetector.drawMat);
				}

				vector<float> evaData;
				evaData.push_back(features.size());
				evaData.push_back(10);
				evaData.push_back(summerizedFeatures.size());
				evaData.push_back(summerizedFeatures.size()/10.0);
				evaluation.saveCVSData(evaData);
#endif
				
				LeaveCriticalSection (&calcCrs);
				//obj->createCompleteGraph(maxSet);

				// display 3D structur of the Object in an OpenGL Window
				PostMessage(pObjViewContext->hWnd, WM_PAINT, 0, 0);	
				//PostMessage(pKoordViewContext->hWnd, WM_PAINT, 0, 0);	

				char c = waitKey(100);
				if(c == 27) break;
				switch(c){
#ifdef EVALUATION
					case 's':
						evaluation.saveCVBild("MarkerSize", right);
						break;
#endif
					case 'c':
						obj->saveToVTKFile("Box3");
						break;
				}
#ifdef TEST
				switch(c){
					case 'p':
						if(isPause){
							cout<<"frame running!"<<endl;
							LeaveCriticalSection(&pauseCrs);
							isPause = false;
						} else {
							cout<<"frame pause!"<<endl;
							EnterCriticalSection(&pauseCrs);
							isPause = true;
						}
						break;
					case 'b':
						balance += 10;
						break;
					case 'v':
						balance -= 10;
						break;
					case 'n':
						contrast += 1;
						break;
					case 'm':
						contrast -= 1;
						break;
					case 't':
						detecParam +=2;
						break;
					case 'r':
						detecParam -=2;
						break;
					case 'o':
						//calculate energie
						energie = 0;
						maxValue = 0;
						for(int k = 0;k<204*204;k++){
							energie += data[k];
							if(filteData[k] > maxValue){
								maxValue = filteData[k];
							}
						}
						//energie = energie/(204*204);
						break;

					case '1':
						testP1 +=1;
						break;
					case 'q':
						testP1 -=1;
						break;
					case '2':
						testP2 +=1;
						break;
					case 'w':
						testP2 -=1;
						break;
					case '3':
						testP3 +=1;
						break;
					case 'e':
						testP3 -=1;
						break;
					case '6':
						eps +=0.5;
						break;
					case 'z':
						eps -=0.5;
						break;
				}
	
				//cout<<"The balance and contrast are: "<<balance<<"   "<<contrast<<endl;
				//cout<<"The parameter of detection is: "<<detecParam<<endl;
				//cout<<"The energie of the data is: "<<energie<<endl;
				//cout<<"The maximal value of the data is "<<maxValue<<endl;
				//cout<<"Test Parameters: "<<testP1<<"    "<<testP2<<"    "<<testP3<<endl;
				cout<<"Test Parameters eps: "<<eps<<endl;
				LeaveCriticalSection (&pauseCrs);
#endif
				//printf("main thread running\n"); 
				//LeaveCriticalSection (&pauseCrs);
				LeaveCriticalSection(&frameCrs);				
			}


			LeaveCriticalSection (&cvInitCrs);
		}//end cv and std namespace
		
		
		// release the memory block of the data		
		delete dFilter;
		
	}

	return nRetCode;
}

