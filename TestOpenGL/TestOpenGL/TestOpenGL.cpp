// TestOpenGL.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "TestOpenGL.h"


#define OFFLINE

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

CRITICAL_SECTION frameCrs;
CRITICAL_SECTION calcCrs;

// global flag 
bool bDone = false;
MSG msg;
bool isPause = false;

// The Context for 3D data Window
//HWND openGLhnd;	
OpenGLContext *p3DDataViewContext;
// The Context for Objekt structur Window
//HWND objGLhnd;
OpenGLContext *pObjViewContext;


// The Buffer for BildDatas, which is saving the newest BildData at first place and the oldest at the last place.
// The maximal length of the Buffer is defined with MAXBUFFERSIZE
vector<BildData*> bildDataBuffer;
int MAXBUFFERSIZE = 2;

// framerate
int FRAMERATE = 150;

//int HISFRAMEINDEX = 3;

DistanceFilter *dFilter;

Graph *obj;



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
	//EnterCriticalSection(&frameCrs);
	display(pOpenGLWinUI, bildDataBuffer[0]);
	//LeaveCriticalSection(&frameCrs);

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
					LeaveCriticalSection(&frameCrs);
					isPause = false;
				} else {
					cout<<"frame pause!"<<endl;
					EnterCriticalSection(&frameCrs);
					isPause = true;
				}
			} else{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				//display(pOpenGLWinUI, disData, intData, ampData);
			}
		} else {
			display(pOpenGLWinUI, bildDataBuffer[0]);
			SwapBuffers(p3DDataViewContext->hDC);
		}
		//display(pOpenGLWinUI, disData, intData, ampData);
		//EnterCriticalSection(&frameCrs);
		//display(pOpenGLWinUI, bildDataBuffer[0]);
		//LeaveCriticalSection(&frameCrs);
			
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
	setDefaultLoadPath("TestRotation");

	//get the distance data for the first step
	//loadNormalDataFromFile("distance", 3, bildData->disData);
	BildData *temp = new BildData();
	loadAllDataFromFile(3, temp);
	//init a distance filter
	dFilter = new DistanceFilter(temp->disData);

	cout<<"Upgrading Distance Filter and Save Data into Buffer....."<<endl;
	for(int i=1;i<20;i++){
		//loadNormalDataFromFile("distance", i, bildData->disData);
		BildData *temp = new BildData();
		loadAllDataFromFile(i, temp);
		if(bildDataBuffer.size()>=MAXBUFFERSIZE){
			bildDataBuffer.pop_back();
		}
		bildDataBuffer.insert(bildDataBuffer.begin(), temp);

		dFilter->Upgrade(temp->disData);
	}
	cout<<"Upgrading complete!"<<endl;
	
	LeaveCriticalSection (&cvInitCrs);
	LeaveCriticalSection (&glInitCrs);

	for(int i=20;i<750;i++){
		EnterCriticalSection(&frameCrs);
		//loadNormalDataFromFile("distance", i, bildData->disData);
		//loadNormalDataFromFile("intensity", i, bildData->intData);
		//loadNormalDataFromFile("amplitude", i, bildData->ampData);

		BildData *temp = new BildData();
		loadAllDataFromFile(i, temp);

		// Pop the last element of the Buffer, and the destructor will be called automatically 
		bildDataBuffer.pop_back();
		bildDataBuffer.insert(bildDataBuffer.begin(), temp);

		//updata the OpenGL Window
		//PostMessage(openGLhnd, WM_PAINT, 0, 0);	
		PostMessage(p3DDataViewContext->hWnd, WM_PAINT, 0, 0);	
		//setARData(intData);
		LeaveCriticalSection(&frameCrs);
		Sleep(FRAMERATE);
	}

#else
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);
	createDefaultPMDDataDirectory("TestRotation");
	setIsDataSaved(true);
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
	dFilter = new DistanceFilter(temp->disData);

	cout<<"Upgrading Distance Filter and Save Data into Buffer....."<<endl;
	for(int i=0;i<20;i++){
		BildData *temp = new BildData();
		getPMDData(temp);
		if(bildDataBuffer.size()>=MAXBUFFERSIZE){
			bildDataBuffer.pop_back();
		}
		bildDataBuffer.insert(bildDataBuffer.begin(), temp);

		dFilter->Upgrade(temp->disData);
	}
	cout<<"Upgrading complete!"<<endl;

	LeaveCriticalSection (&glInitCrs);
	LeaveCriticalSection (&cvInitCrs);

	while(!bDone){
		EnterCriticalSection(&frameCrs);

		//setARData(intData);
		try {
			BildData *temp = new BildData();
			getPMDData(temp);

			bildDataBuffer.pop_back();
			bildDataBuffer.insert(bildDataBuffer.begin(), temp);
		} catch (CMemoryException * e){
			AfxMessageBox("Irgendwas ist schiefgegangen!");
		}

		LeaveCriticalSection(&frameCrs);
		Sleep(FRAMERATE);
	}
	closePMDCon();
#endif
}


void objWindowThreadPorc(void *param ){
	static OpenGLWinUI *pObjGLWinUI = new OpenGLWinUI;
	pObjViewContext = new OpenGLContext;

	TlsSetValue(ThreadIndex, pObjGLWinUI);

	

	if(!CreateOpenGLWindow("Object Window", 0, 0, 1000, 1000, PFD_TYPE_RGBA, 0, pObjGLWinUI, pObjViewContext)){
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
					LeaveCriticalSection(&frameCrs);
					isPause = false;
				} else {
					cout<<"frame pause!"<<endl;
					EnterCriticalSection(&frameCrs);
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



int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	//just for the situation, that more than one threads are calling the same data at the same time
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

			namedWindow("OpenCVGrayScale", CV_WINDOW_AUTOSIZE);
			namedWindow("OpenCVRGBTest", CV_WINDOW_AUTOSIZE);
			namedWindow("OpenCVRGBResult", CV_WINDOW_AUTOSIZE);
			namedWindow("OpenCVRGBGraph", CV_WINDOW_AUTOSIZE);
			Size size = Size(204, 204);
			Mat img, featuresImg, testimg;
			//bool isPause = false;
			int balance = 200;
			float contrast = 10;


			double energie = 0;
			float maxValue = 0;



			//parameter for STAR Detector
			int MAXSIZE = 8;
			//int RESPONSETHRESHOLD = 70;
			int detecParam = 70;
			int LINETHRESHOLDPROJECTED = 5;
			int LINETHRESHOLDBINARIZED = 6;
			int SUPPRESSNONMAXSIZE = 1;

			//the maximal loop times
			int MAXLOOPS = 30;
			

			//Test
			int testP1 = 5;
			int testP2 = 6;
			int testP3 = 1;

			float eps = 5;

			//The summerized KeyPoint fot the current frame
			vector<KeyPoint> sumFeatures;

			unsigned char hisShowdata[41616*3];

			obj = new Graph();
			
			while (!bDone) 
			{ 	
				if(isPause){
					continue;
				}
				//call the calculate function after the init of PMD Camera
				EnterCriticalSection (&cvInitCrs);
				EnterCriticalSection (&calcCrs);

				//get the current Bild Data
				BildData *currentBildData = bildDataBuffer[0];
				
				// create the data for a RGB image
				unsigned char showdata[41616*3];
				

				transFloatTo3Char(currentBildData->ampData, showdata, balance, contrast);
				transFloatTo3Char(bildDataBuffer[bildDataBuffer.size()-1]->ampData, hisShowdata, balance, contrast);

				// set the data to the RGB image
				featuresImg = Mat(size, CV_8UC3, showdata);
				Mat hisImg = Mat(size, CV_8UC3, hisShowdata);
				testimg = Mat(204, 204*2, CV_8UC3);

				Mat left = Mat(testimg, Range::all(), Range(0,204));
				Mat right = Mat(testimg, Range::all(), Range(204,204*2));

				hisImg.copyTo(left);
				featuresImg.copyTo(right);
				

				// create the array to save the filted data
				float filteData[204*204];
				bool isDiff = dFilter->Filte(currentBildData->disData, currentBildData->ampData, filteData);


				//if(!isDiff) continue;


				vector<KeyPoint> features;
				
				unsigned char data[41616];

#ifdef TEST
				EnterCriticalSection(&frameCrs);
				// translate the filted data to a grayscale image 
				transFloatToChar(filteData, data, balance, contrast);
				img = Mat(size, CV_8UC1, data);
				
				vector<KeyPoint> features;
				//set the parameter of the STAR Detector
				StarDetector detector = StarDetector(MAXSIZE, detecParam, LINETHRESHOLDPROJECTED, LINETHRESHOLDBINARIZED, SUPPRESSNONMAXSIZE);
				detector(img, features);
				
				//draw features
				int vectorSize = features.size();
				cout<<"find "<<vectorSize<<" features!"<<endl;
				for(int i=0;i<vectorSize;i++){
					circle(featuresImg, features[i].pt, 1, Scalar(0,0,255,0), -1); 
					//sumFeatures.insert(sumFeatures.begin()+i, features[i]);
				}

				//summarize the near features
				vector<KeyPoint> sumFeatures = features;
				//The loop of all features
				//for(int i=0;i<sumFeatures.size();i++){
				//	//The loop from current features to the others, which is behind the current feature
				//	for(int j=i+1;j<sumFeatures.size();j++){
				//		//calculate the distance between two features
				//		float xDis = sumFeatures[i].pt.x - sumFeatures[j].pt.x;
				//		float yDis = sumFeatures[i].pt.y - sumFeatures[j].pt.y;
				//		//if they are too close
				//		if((xDis*xDis + yDis*yDis)<eps*eps){
				//			//reset the position of the ith feature
				//			sumFeatures[i].pt.x = 0.5*(sumFeatures[i].pt.x + sumFeatures[j].pt.x);
				//			sumFeatures[i].pt.y = 0.5*(sumFeatures[i].pt.y + sumFeatures[j].pt.y);
				//			//delete the feature at jth position
				//			sumFeatures.erase(sumFeatures.begin() + j);
				//		}
				//	}
				//	circle(left, sumFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
				//	circle(right, sumFeatures[i].pt, 1, Scalar(0,0,255,0), -1);

				//	//translate vector from left to right
				//	Point2f trans(204, 0);
				//	line(testimg, sumFeatures[i].pt, sumFeatures[i].pt+trans, Scalar(255, 255, 0, 0));
				//}
				//cout<<"After Summerize get "<<sumFeatures.size()<<" features!"<<endl;

				vector<vector<KeyPoint>> groupFeatures;
					vector<int> pointer;
					for(int i=0;i<sumFeatures.size();i++){
						pointer.push_back(-1);
					}
					for(int i=0;i<sumFeatures.size();i++){
						vector<KeyPoint> temp;
						int index;
						//no pointer defined
						if(pointer.at(i) == -1){
							temp.push_back(sumFeatures[i]);
							index = groupFeatures.size();
							for(int j=0;j<sumFeatures.size();j++){
								//calculate the distance between two features
								float xDis = fabs(sumFeatures[i].pt.x - sumFeatures[j].pt.x);
								float yDis = fabs(sumFeatures[i].pt.y - sumFeatures[j].pt.y);
								//if they are too close
								//if(xDis<eps && yDis<eps){
								if((xDis*xDis + yDis*yDis)<eps*eps){
									if(pointer.at(j) != index){
										temp.push_back(sumFeatures[j]);
										pointer.at(j) = index;
									}
								}
							}
							groupFeatures.push_back(temp);
						} else {
							index = pointer.at(i);
							temp = groupFeatures.at(index);
							for(int j=0;j<sumFeatures.size();j++){
								//calculate the distance between two features
								float xDis = fabs(sumFeatures[i].pt.x - sumFeatures[j].pt.x);
								float yDis = fabs(sumFeatures[i].pt.y - sumFeatures[j].pt.y);
								//if they are too close
								//if(xDis<eps && yDis<eps){
								if((xDis*xDis + yDis*yDis)<eps*eps){
									if(pointer.at(j) != index){
										temp.push_back(sumFeatures[j]);
										pointer.at(j) = index;
									}
								}
							}
							groupFeatures.at(index) = temp;
						}
					}
					cout<<"After Summerize get "<<groupFeatures.size()<<" features!"<<endl;

					for(int i=0;i<groupFeatures.size();i++){
						float avrX = 0;
						float avrY = 0;
						int size = groupFeatures[i].size();
						for(int j=0;j<size;j++){
							avrX += groupFeatures[i][j].pt.x;
							avrY += groupFeatures[i][j].pt.y;
						}

						Point2f p(avrX/size, avrY/size);
						circle(left, p, 1, Scalar(0,255,0,0), -1);
						circle(right, p, 1, Scalar(0,0,255,0), -1);

						//translate vector from left to right
						Point2f trans(204, 0);
						line(testimg, p, p+trans, Scalar(255, 255, 0, 0));
					}

				//Mat sampleLables = Mat(sumFeatures.size(), 1, CV_32FC1);
				//TermCriteria criteria;
				//criteria.epsilon = 0.01;
				//kmeans(sumFeatures, 3, sampleLables, criteria, 2, KMEANS_RANDOM_CENTERS); 

				//draw
				imshow("OpenCVGrayScale", img);
				imshow("OpenCVRGBResult", featuresImg);
				imshow("OpenCVRGBTest", testimg);

#else
				int safeCount = 0;
				vector<vector<KeyPoint>> groupFeatures;

				/****************************************************************
				 *
				 * The Loop for the Features' Detection. The result is just a between solution
				 *
				 ***************************************************************/
				while(safeCount < MAXLOOPS){
					safeCount ++;
					//Sleep(500);
					// translate the filted data to a grayscale image 
					transFloatToChar(filteData, data, balance, contrast);
					img = Mat(size, CV_8UC1, data);
					
					features.clear();
					//set the parameter of the STAR Detector
					StarDetector detector = StarDetector(MAXSIZE, detecParam, LINETHRESHOLDPROJECTED, LINETHRESHOLDBINARIZED, SUPPRESSNONMAXSIZE);
					detector(img, features);
					
					//draw features
					int vectorSize = features.size();
					cout<<"find "<<features.size()<<" features!"<<endl;
					for(int i=0;i<features.size();i++){
						circle(featuresImg, features[i].pt, 1, Scalar(0,0,255,0), -1); 
					}
							
					//draw historical features
					//if(hisFeatures.size()>0){
					//	for(int i=0;i<hisFeatures.size();i++){
					//		circle(testimg, hisFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
					//	}
					//}

					//draw
					imshow("OpenCVGrayScale", img);
					imshow("OpenCVRGBResult", featuresImg);
					
					/**********************************
					 *
					 * The Distance Filter
					 *
					 *********************************/
					if(!isDiff) break;

					if(!brightnessControll(vectorSize, contrast, detecParam, data)){
						break;
					}
					
				} // Ende of the Loop for the Features' Detection
				
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
				//	//circle(testimg, sumFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
				//	circle(left, sumFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
				//	circle(right, sumFeatures[i].pt, 1, Scalar(0,0,255,0), -1);

				//	//translate vector from left to right
				//	Point2f trans(204, 0);
				//	line(testimg, sumFeatures[i].pt, sumFeatures[i].pt+trans, Scalar(255, 255, 0, 0));
				//}
				//cout<<"After Summerize get "<<sumFeatures.size()<<" features!"<<endl;
				//int vectorSize = sumFeatures.size();

				/**************************************************
				 *
				 * A new Algorithm to make sure, that every marker will be as a just one KeyPoint recognized
				 *
				 * 1. Spread the features to different set, which are close to each others. The parameter eps set the size of the area
				 * 2. Calculate the middel point of each set and show them
				 *************************************************/
				// to save the sets and the elements of set
				//vector<vector<KeyPoint>> groupFeatures;
				// to save the set Nr. for each feature
				vector<int> pointer;
				// the beginning set Nr. is -1 
				for(int i=0;i<features.size();i++){
					pointer.push_back(-1);
				}
				for(int i=0;i<features.size();i++){
					vector<KeyPoint> temp;
					int index;
					// if the set Nr. for the current feature is not be set
					if(pointer.at(i) == -1){
						// create a new set, and add the current feature to it
						temp.push_back(features[i]);
						// get the set Nr.
						index = groupFeatures.size();
						for(int j=0;j<features.size();j++){
							//calculate the distance between two features
							float xDis = fabs(features[i].pt.x - features[j].pt.x);
							float yDis = fabs(features[i].pt.y - features[j].pt.y);
							//if they are too close
							if((xDis*xDis + yDis*yDis)<eps*eps){
								// if the feature j is not include in this set
								if(pointer.at(j) != index){
									// add the feature j into the set of close features for feature i 
									temp.push_back(features[j]);
									// set the set Nr. for feature j
									pointer.at(j) = index;
								}
							}
						}
						// add the new set to the groupFeatures
						groupFeatures.push_back(temp);
					// if the set Nr. for the current feature has been already set 
					} else {
						// get the set Nr.
						index = pointer.at(i);
						// get the set
						temp = groupFeatures.at(index);
						for(int j=0;j<features.size();j++){
							//calculate the distance between two features
							float xDis = fabs(features[i].pt.x - features[j].pt.x);
							float yDis = fabs(features[i].pt.y - features[j].pt.y);
							//if they are too close
							//if(xDis<eps && yDis<eps){
							if((xDis*xDis + yDis*yDis)<eps*eps){
								// if the feature j is not include in this set
								if(pointer.at(j) != index){
									temp.push_back(features[j]);
									pointer.at(j) = index;
								}
							}
						}
						// reset the set of groupFeatures 
						groupFeatures.at(index) = temp;
					}
				}
				cout<<"After Summerize get "<<groupFeatures.size()<<" features!"<<endl;

				// Save the features into the first place of data buffer
				currentBildData->features.clear();
				for(int i=0;i<groupFeatures.size();i++){
					float avrX = 0;
					float avrY = 0;
					// calculate the average depth data of the features
					float avrZ = 0;
					int size = groupFeatures[i].size();
					for(int j=0;j<size;j++){
						float x = groupFeatures[i][j].pt.x;
						float y = groupFeatures[i][j].pt.y;
						avrX += x;
						avrY += y;
						avrZ += currentBildData->threeDData[((int)x*204+(int)y)*3+2];
					}

					Point2f p2(avrX/size, avrY/size);
					Point3f p3(avrX/size, avrY/size, avrZ/size);
					
					currentBildData->features.push_back(p3);
					
					// show the current features
					circle(right, p2, 1, Scalar(0,0,255,0), -1);

					//translate vector from left to right
					//Point2f trans(204, 0);
					//line(testimg, p, p+trans, Scalar(255, 255, 0, 0));
				}

				//save the detected features into the vector of historical features
				vector<Point3f> hisFeatures = bildDataBuffer[bildDataBuffer.size()-1]->features;
				for(int i=0;i<hisFeatures.size();i++){
					// show the old features
					circle(left, point3To2(hisFeatures[i]), 2, Scalar(0,255,0,0), -1);
				}
				

				// feature Tracking with Singular Value Decomposition
				//vector<int> oldResult, newResult;
				//vector<Point2f> curFeatures = currentBildData->features;
				//if(hisFeatures.size()>0&&curFeatures.size()>0){
				//	featureAssociate(hisFeatures, curFeatures, 15, oldResult, newResult);
				//	for(int i=0;i<oldResult.size();i++){
				//		Point2f trans(204,0);
				//		line(testimg, hisFeatures[oldResult[i]], curFeatures[newResult[i]]+trans, Scalar(255,255,0,0));
				//	}
				//	cout<<"The number of useful features is: "<<oldResult.size()<<endl;
				//}

				// call calibration
				vector<vector<Point3f>> caliResult;
				calibration(caliResult, currentBildData->features, 18);
				//calibration(caliResult, newResult, 18);

				Mat graphImg = Mat(size, CV_8UC3, showdata);
				for(int k=0;k<caliResult.size();k++){
					int firstSize = caliResult[k].size();
					for(int i=0;i<firstSize;i++){
						for(int j=0;j<firstSize;j++){
							line(graphImg, point3To2(caliResult[k][i]), point3To2(caliResult[k][j]), Scalar(0, 255, 255, 0));
						}
					}
				}
				imshow("OpenCVRGBGraph", graphImg);

				vector<Point3f> maxSet;
				findMaxPointsSet(caliResult, maxSet);
				
				vector<Point3f> oldResult, newResult;
				vector<Point3f> curFeatures = currentBildData->features;
				Mat R = Mat(3,3,CV_32FC1);
				Mat T = Mat(3,1,CV_32FC1);
				if(hisFeatures.size()>0&&curFeatures.size()>0){
					featureAssociate2(hisFeatures, curFeatures, 15, oldResult, newResult);

					findRAndT(oldResult, newResult, R, T);
					cout<<"The rotation matrix is: "<<R<<endl<<endl;
					cout<<"The translation matrix is: "<<T<<endl<<endl;
					obj->updateGraph(newResult, R, T);

					for(int i=0;i<oldResult.size();i++){
						Point2f trans(204,0);
						line(testimg, point3To2(oldResult[i]), point3To2(newResult[i])+trans, Scalar(255,255,0,0));
					}
					cout<<"The number of useful features is: "<<oldResult.size()<<endl;
				}

				imshow("OpenCVRGBTest", testimg);

				

				//obj->updateGraph(maxSet);
				

				LeaveCriticalSection (&calcCrs);
				//obj->createCompleteGraph(maxSet);

				// display 3D structur of the Object in an OpenGL Window
				PostMessage(pObjViewContext->hWnd, WM_PAINT, 0, 0);	


#endif
				char c = waitKey(100);
				if(c == 27) break;
#ifdef TEST
				switch(c){
					case 'p':
						if(isPause){
							cout<<"frame running!"<<endl;
							LeaveCriticalSection(&frameCrs);
							isPause = false;
						} else {
							cout<<"frame pause!"<<endl;
							EnterCriticalSection(&frameCrs);
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
				LeaveCriticalSection (&frameCrs);
#endif
				//printf("main thread running\n"); 
				//LeaveCriticalSection (&frameCrs);
				
			}


			LeaveCriticalSection (&cvInitCrs);
			destroyWindow("OpenCVWindow");
		}//end cv and std namespace
		
		
		// release the memory block of the data		
		delete dFilter;
		
	}

	return nRetCode;
}

