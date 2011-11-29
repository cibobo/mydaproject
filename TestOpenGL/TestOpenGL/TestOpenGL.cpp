// TestOpenGL.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include "TestOpenGL.h"


#define OFFLINE

#define TEST

#ifndef _MT 
#define _MT 
#endif 

const int DATA_SIZE = 204*204;

DWORD ThreadIndex = 1001;

using namespace std;

CRITICAL_SECTION crs;
CRITICAL_SECTION glInitCrs;
CRITICAL_SECTION cvInitCrs;
CRITICAL_SECTION frameCrs;
CRITICAL_SECTION calcCrs;

// global flag 
bool bDone = false;
// The handle for OpenGL Window
HWND openGLhnd;	
// The handle for Grayscale Window
HWND grayScalehnd;

//TODO: direct pointer define
// Array and the Pointer of Distance Data
//float dData[DATA_SIZE];
float *disData;// = dData;

// Array and the Pointer of Intensity Data
//float iData[DATA_SIZE];
float *intData;

// Array and the Pointer of Amplitude Data
//float aData[DATA_SIZE];
float *ampData;

DistanceFilter *dFilter;


// this function is called by a new thread 
void openGLThreadPorc( void *param ) 
{ 
	MSG msg;
	//BOOL done = FALSE;
	static OpenGLWinUI *pOpenGLWinUI = new OpenGLWinUI;

	TlsSetValue(ThreadIndex, pOpenGLWinUI);

	EnterCriticalSection (&glInitCrs);

	//if(!CreateOpenGLWindow("minimal", 0, 0, 800, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI)){
	if((openGLhnd=CreateOpenGLWindow("OpenGL Window", 0, 0, 900, 600, PFD_TYPE_RGBA, 0, pOpenGLWinUI))==NULL){
		exit(0);
	}

	cout<<"Thread OpenGL running"<<endl;

	LeaveCriticalSection (&glInitCrs);

	// call the display function and send the data direct with the pointer of array as parameter
	display(pOpenGLWinUI, disData, intData, ampData);

	//parameter to mark, whether the input process is in pause
	bool isPause = false;

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
		}
		display(pOpenGLWinUI, disData, intData, ampData);
			
	}

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

	EnterCriticalSection (&crs);

	arVideoCapStart();
	argMainLoop( NULL, keyEvent, mainLoop );

	LeaveCriticalSection (&crs);


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

	EnterCriticalSection (&cvInitCrs);
	setDefaultLoadPath("PeopleAndObj");

	//get the distance data for the first step
	loadNormalDataFromFile("distance", 3, disData);
	//init a distance filter
	dFilter = new DistanceFilter(disData);

	cout<<"Upgrading Distance Filter....."<<endl;
	for(int i=1;i<20;i++){
		loadNormalDataFromFile("distance", i, disData);
		dFilter->Upgrade(disData);
	}
	cout<<"Upgrading complete!"<<endl;
	LeaveCriticalSection (&cvInitCrs);

	for(int i=20;i<750;i++){
		EnterCriticalSection(&frameCrs);
		loadNormalDataFromFile("distance", i, disData);
		loadNormalDataFromFile("intensity", i, intData);
		loadNormalDataFromFile("amplitude", i, ampData);
		//openGLLoadData(disData, intData, ampData);	
		//updata the OpenGL Window
		PostMessage(openGLhnd, WM_PAINT, 0, 0);	
		//setARData(intData);
		LeaveCriticalSection(&frameCrs);
		Sleep(100);
	}

#else
	EnterCriticalSection (&glInitCrs);
	EnterCriticalSection (&cvInitCrs);
	//createDefaultPMDDataDirectory("PeopleAndObj");
	//setIsDataSaved(true);
	cout<<"PMD Camera Connecting..."<<endl;
	if(!createPMDCon()){
		exit(1);
		//cout<<"input thread running!"<<endl;
		
	}
	//Sleep(3000);
	//cout<<"input thread running!!!!"<<endl;


	//calibration();

	// init distance filter
	getPMDData(disData, intData, ampData);
	dFilter = new DistanceFilter(disData);

	cout<<"Upgrading Distance Filter....."<<endl;
	for(int i=0;i<20;i++){
		getPMDData(disData, intData, ampData);
		dFilter->Upgrade(disData);
	}
	cout<<"Upgrading complete!"<<endl;

	LeaveCriticalSection (&glInitCrs);
	LeaveCriticalSection (&cvInitCrs);

	while(!bDone){
		EnterCriticalSection(&frameCrs);
		getPMDData(disData, intData, ampData);
		//setARData(intData);
		//disData = getPMDDataPointer();
		
		//openGLLoadData(disData, intData, ampData);
		
		//PostMessage(openGLhnd, WM_PAINT, 0, 0);
		//cout<<"Input Process with PMD Cam is running!"<<endl;
		LeaveCriticalSection(&frameCrs);
		Sleep(100);
	}
	closePMDCon();
#endif
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	//just for the situation, that more than one threads are calling the same data at the same time
	InitializeCriticalSection (&crs);
	InitializeCriticalSection (&glInitCrs);
	InitializeCriticalSection (&cvInitCrs);
	InitializeCriticalSection (&frameCrs);

	// MFC initialisieren und drucken. Bei Fehlschlag Fehlermeldung aufrufen.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: Den Fehlercode an Ihre Anforderungen anpassen.
		_tprintf(_T("Schwerwiegender Fehler bei der MFC-Initialisierung\n"));
		nRetCode = 1;
	}
	else
	{
		// create a new array for distance data
		disData = new float[DATA_SIZE];

		// create a new array for intensity data
		intData = new float[DATA_SIZE];

		// create a new array for amplitude data
		ampData = new float[DATA_SIZE];

		//Start Input Thread
		if(_beginthread (inputThreadProc, 0, NULL) == -1){
			cout<<"Failed to create input thread"<<endl;
		}

		 //Start OpenGL Window Thread 
		//if(_beginthread (openGLThreadPorc, 0, NULL)==-1){
		//	cout<<"Failed to create openGL thread"<<endl;
		//}

		// Start ARToolKit Window Thread 
		//if(_beginthread (arToolKitThreadProc, 0, NULL)==-1){
		//	cout<<"Failed to create ARToolKit thread"<<endl;
		//}

		//Main Thread
		{
			using namespace cv; 
			namedWindow("OpenCVGrayScale", CV_WINDOW_AUTOSIZE);
			namedWindow("OpenCVRGBTest", CV_WINDOW_AUTOSIZE);
			namedWindow("OpenCVRGBResult", CV_WINDOW_AUTOSIZE);
			Size size = Size(204, 204);
			Mat img, showimg, testimg;
			bool isPause = false;
			int balance = 200;
			float contrast = 10;


			double energie = 0;
			float maxValue = 0;

			int MINFEATURECOUNT = 7;
			int MAXFEATURECOUNT = 11;

			float MINSTANDARDENERGY = 300000.0;
			float MAXSTANDARDENERGY = 450000.0;

			float MINCONTRAST = 2;
			float MAXCONTRAST = 18;

			float MINRESPONSETHRESHOLD = 40;
			float MAXRESPONSETHRESHOLD = 130;

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
			std::vector<KeyPoint> sumFeatures;

			//The KeyPoint for the last frame
			std::vector<KeyPoint> hisFeatures;
			unsigned char hisShowdata[41616*3];
			
			while (!bDone) 
			{ 	
				//call the calculate function after the init of PMD Camera
				EnterCriticalSection (&cvInitCrs);
				
				// create the data for a RGB image
				unsigned char showdata[41616*3];
				

				transFloatTo3Char(ampData, showdata, balance, contrast);
				transFloatTo3Char(ampData, hisShowdata, balance, contrast);

				// set the data to the RGB image
				showimg = Mat(size, CV_8UC3, showdata);
				//testimg = Mat(size, CV_8UC3, showdata2);
				testimg = Mat(204, 204*2, CV_8UC3);
				Mat left = Mat(testimg, Range::all(), Range(0,204));
				Mat right = Mat(testimg, Range::all(), Range(204,204*2));
				showimg.copyTo(left);
				showimg.copyTo(right);
				


				// create the array to save the filted data
				float filteData[204*204];
				bool isDiff = dFilter->Filte(disData, ampData, filteData);


				
				unsigned char data[41616];

#ifdef TEST
				EnterCriticalSection(&frameCrs);
				// translate the filted data to a grayscale image 
				transFloatToChar(filteData, data, balance, contrast);
				img = Mat(size, CV_8UC1, data);
				
				std::vector<KeyPoint> features;
				//set the parameter of the STAR Detector
				StarDetector detector = StarDetector(MAXSIZE, detecParam, LINETHRESHOLDPROJECTED, LINETHRESHOLDBINARIZED, SUPPRESSNONMAXSIZE);
				detector(img, features);
				
				//draw features
				int vectorSize = features.size();
				cout<<"find "<<vectorSize<<" features!"<<endl;
				for(int i=0;i<vectorSize;i++){
					circle(showimg, features[i].pt, 1, Scalar(0,0,255,0), -1); 
					//sumFeatures.insert(sumFeatures.begin()+i, features[i]);
				}

				//summarize the near features
				std::vector<KeyPoint> sumFeatures = features;
				//The loop of all features
				for(int i=0;i<sumFeatures.size();i++){
					//The loop from current features to the others, which is behind the current feature
					for(int j=i+1;j<sumFeatures.size();j++){
						//calculate the distance between two features
						float xDis = sumFeatures[i].pt.x - sumFeatures[j].pt.x;
						float yDis = sumFeatures[i].pt.y - sumFeatures[j].pt.y;
						//if they are too close
						if(fabs(xDis)<eps && fabs(yDis)<eps){
							//reset the position of the ith feature
							sumFeatures[i].pt.x = 0.5*(sumFeatures[i].pt.x + sumFeatures[j].pt.x);
							sumFeatures[i].pt.y = 0.5*(sumFeatures[i].pt.y + sumFeatures[j].pt.y);
							//delete the feature at jth position
							sumFeatures.erase(sumFeatures.begin() + j);
						}
					}
					circle(testimg, sumFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
				}
				cout<<"After Summerize get "<<sumFeatures.size()<<" features!"<<endl;

				//kmeans(

				//draw
				imshow("OpenCVGrayScale", img);
				imshow("OpenCVRGBResult", showimg);
				imshow("OpenCVRGBTest", testimg);

#else
				int safeCount = 0;

				while(safeCount < MAXLOOPS){
					safeCount ++;
					//Sleep(500);
					// translate the filted data to a grayscale image 
					transFloatToChar(filteData, data, balance, contrast);
					img = Mat(size, CV_8UC1, data);
					
					std::vector<KeyPoint> features;
					//set the parameter of the STAR Detector
					StarDetector detector = StarDetector(MAXSIZE, detecParam, LINETHRESHOLDPROJECTED, LINETHRESHOLDBINARIZED, SUPPRESSNONMAXSIZE);
					detector(img, features);
					
					//draw features
					//int vectorSize = features.size();
					cout<<"find "<<features.size()<<" features!"<<endl;
					for(int i=0;i<features.size();i++){
						circle(showimg, features[i].pt, 1, Scalar(0,0,255,0), -1); 
						//sumFeatures.insert(sumFeatures.begin()+i, features[i]);
					}

					/***********************************
					 *
					 * Summarize the near features
					 * 
					 * Because the last parameter of STAR detector SUPPRESSNONMAXSIZE set as 1, more features will be found as expected.
					 * The close features will be set as one feature
					 *
					 ***********************************/
					sumFeatures.clear();
					sumFeatures = features;
					//The loop of all features
					for(int i=0;i<sumFeatures.size();i++){
						//The loop from current features to the others, which is behind the current feature
						for(int j=i+1;j<sumFeatures.size();j++){
							//calculate the distance between two features
							float xDis = fabs(sumFeatures[i].pt.x - sumFeatures[j].pt.x);
							float yDis = fabs(sumFeatures[i].pt.y - sumFeatures[j].pt.y);
							//if they are too close
							if(xDis<eps && yDis<eps){
								//delete the feature at jth position
								sumFeatures.erase(sumFeatures.begin() + j);
							}
						}
						//circle(testimg, sumFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
						circle(left, sumFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
						circle(right, sumFeatures[i].pt, 1, Scalar(0,0,255,0), -1);

						//translate vector from left to right
						Point2f trans(204, 0);
						line(testimg, sumFeatures[i].pt, sumFeatures[i].pt+trans, Scalar(255, 255, 0, 0));
					}
					cout<<"After Summerize get "<<sumFeatures.size()<<" features!"<<endl;
					int vectorSize = sumFeatures.size();

					//draw historical features
					//if(hisFeatures.size()>0){
					//	for(int i=0;i<hisFeatures.size();i++){
					//		circle(testimg, hisFeatures[i].pt, 1, Scalar(0,255,0,0), -1);
					//	}
					//}

					//draw
					imshow("OpenCVGrayScale", img);
					imshow("OpenCVRGBResult", showimg);
					imshow("OpenCVRGBTest", testimg);

					
					/**********************************
					 *
					 * The Distance Filter
					 *
					 *********************************/
					if(!isDiff) break;


					/*********************************
					 * 
					 * The algorithm for the Brightness and RESPONSETHRESHOLD
					 *
					 ********************************/
					//if lesser than 7 features have been found
					if(vectorSize < MINFEATURECOUNT){
						cout<<"Case 1111111111111111111111111111111"<<endl;

						//calculate the Energy of the frame
						energie = 0;
						for(int k = 0;k<204*204;k++){
							energie += data[k];
						}
						cout<<"The energy of the data is: "<<energie<<endl;
						//if(energie < 50000) break;

						//compare the energy with the standard energy
						if(energie < MINSTANDARDENERGY){
							//if the frame is too dark
							//float eFactor = energie/MINSTANDARDENERGY;
							contrast -= 0.5;
							//cout<<"Too Dark!! The energy factor is: "<<eFactor<<endl;
							cout<<"Too Dark! The current contrast is "<<contrast<<endl;
							if(contrast<MINCONTRAST) {
								cout<<"Algo fehld, break!"<<endl;
								contrast = MINCONTRAST;
								break;
							}
						} else if(energie > MAXSTANDARDENERGY){
							//if the frame is too bright
							//float eFactor = energie/MAXSTANDARDENERGY;
							//contrast *= eFactor;
							//cout<<"Too Bright!! The energy factor is: "<<eFactor<<endl;
							
							contrast += 0.5;
							cout<<"Too Bright! The current contrast is "<<contrast<<endl;
							if(contrast>MAXCONTRAST) {
								cout<<"Algo fehld, break!"<<endl;
								contrast = MAXCONTRAST;
								break;
							}
						} else {
							//the energy is acceptable, but still can not find enough features. So change the parameter of STAR Detector
							detecParam -= 5;
							cout<<"Brightness is OK!! The current detecParam is "<<detecParam<<endl;
							if(detecParam < MINRESPONSETHRESHOLD){
								break;
							}
						}						
					} else if(vectorSize > MAXFEATURECOUNT){
						cout<<"Case 222222222222222222222222222"<<endl;
						if(detecParam > MAXRESPONSETHRESHOLD){
							//if the detecParam is too big
							break;
						} else {
							//else increase the detecParam
							detecParam += 4;
						}
						cout<<"Too Many Features!! The current detecParam is "<<detecParam<<endl;
					} else {
						cout<<"Case 333333333333333333333333333"<<endl;
						break;
					}
				}

				//save the detected features into the vector of historical features
				hisFeatures.clear();
				hisFeatures = sumFeatures;

#endif
				char c = waitKey(100);
				if(c == 27) break;
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
#ifdef TEST
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
		}
		
		
		// release the memory block of the data
		delete [] disData;
		delete [] intData;
		delete [] ampData;
		
		delete dFilter;
		
	}

	return nRetCode;
}

