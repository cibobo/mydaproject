// PMDTest.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"

#include "PMDTest.h"
#include "pmdsdk2.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/* Some arbitrary values for the integration times and the
   amplitude threshold. These may not be practical for all
   situations.
*/
#define SHORT_TIME 200
#define LONG_TIME 4000
#define AMPL_THRESHOLD 1300.0

// Das einzige Anwendungsobjekt

CWinApp theApp;

using namespace std;

void checkError (PMDHandle hnd, int code);
void hdrImage (PMDHandle hnd);
void saveDataToCVS(float *data, int cRows, int cCols);
template <typename T> bool saveData(char *pPath, T *pData, int iDataCount);


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// The handle is used to identify a connection 
	PMDHandle hnd;
	// Result code for PMDSDK2 calls 
	int res;

	// MFC initialisieren und drucken. Bei Fehlschlag Fehlermeldung aufrufen.
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: Den Fehlercode an Ihre Anforderungen anpassen.
		_tprintf(_T("Schwerwiegender Fehler bei der MFC-Initialisierung\n"));
		nRetCode = 1;
	}
	else
	{
		/* Open connection using the plugins camcube and
		camcubeproc. hnd is set if the call succeeds.
		The empty strings are parameters for the plugins */
		res = pmdOpen (&hnd, "plugin\\camcube0", "", "plugin\\camcubeproc0", "");

		// Check if the call succeeded 
		if (res != PMD_OK)
		{
			char err[256];
			/* The handle parameter in pmdGetLastError is 0
				because no connection was established */
			pmdGetLastError (0, err, 256);
			fprintf (stderr, "Could not connect: %s\n", err);


			return 1;
		}

		//hdrImage (hnd);

		// Close the connection 
		pmdClose (hnd);

		if (!CreateGLWindow("NeHe's OpenGL Framework",640,480,16,fullscreen)) 
		{ 
					char a;
		cin>>a;
			return 0;                           // Quit If Window Was Not Created 
		} 
	
		//float data[204*204];
		//for(int i=0;i<204*204;i++){
		//	data[i]=1;
		//}
		//cout<<sizeof(data)<<endl;
		//saveData(data, 204, 204);
		char a;
		cin>>a;

	}
	return nRetCode;
}

//int APIENTRY _tWinMain(HINSTANCE hInstance,
//                     HINSTANCE hPrevInstance,
//                     LPTSTR    lpCmdLine,
//                     int       nCmdShow)
//{
//	MSG msg;
//	BOOL done=false;
//	// Ask The User Which Screen Mode They Prefer 
//	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO) 
//	{ 
//		fullscreen=FALSE;                       // Windowed Mode 
//	} 
//	// Create Our OpenGL Window 
//	if (!CreateGLWindow("NeHe's OpenGL Framework",640,480,16,fullscreen)) 
//	{ 
//		return 0;                           // Quit If Window Was Not Created 
//	} 
//	while(!done)                                // Loop That Runs Until done=TRUE 
//	{ 
//		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))           // Is There A Message Waiting? 
//		{ 
//			if (msg.message==WM_QUIT)               // Have We Received A Quit Message? 
//			{ 
//				done=TRUE;                  // If So done=TRUE 
//			} 
//			else                            // If Not, Deal With Window Messages 
//			{ 
//				TranslateMessage(&msg);             // Translate The Message 
//				DispatchMessage(&msg);              // Dispatch The Message 
//			} 
//		} 
//		else                                // If There Are No Messages 
//		{ 
//			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene() 
//			if (active)                     // Program Active? 
//			{ 
//				if (keys[VK_ESCAPE])                // Was ESC Pressed? 
//				{ 
//					done=TRUE;              // ESC Signalled A Quit 
//				} 
//				else                        // Not Time To Quit, Update Screen 
//				{ 
//					DrawGLScene();              // Draw The Scene 
//					SwapBuffers(hDC);           // Swap Buffers (Double Buffering) 
//				} 
//			} 
//			if (keys[VK_F1])                    // Is F1 Being Pressed? 
//			{ 
//				keys[VK_F1]=FALSE;              // If So Make Key FALSE 
//				KillGLWindow();                 // Kill Our Current Window 
//				fullscreen=!fullscreen;             // Toggle Fullscreen / Windowed Mode 
//				// Recreate Our OpenGL Window 
//				if (!CreateGLWindow("NeHe's OpenGL Framework",640,480,16,fullscreen)) 
//				{ 
//					return 0;               // Quit If Window Was Not Created 
//				} 
//			} 
//		} 
//	} 
//	// Shutdown 
//    KillGLWindow();                             // Kill The Window 
//    return (msg.wParam);                            // Exit The Program 
//} 


/* We put error handling in an extra function to keep
   the interesting code short
*/
void checkError (PMDHandle hnd, int code)
{
  /* Only do something if the result code is not PMD_OK */
  if (code != PMD_OK)
    {
      char err[256];

      /* Get an error description from the PMDSDK2 */
      pmdGetLastError (hnd, err, 256);
      /* Display the error message */
      fprintf (stderr, "Error: %s\n", err);
      /* Close the connection */
      pmdClose (hnd);
      /* For our example programs, we just quit */
      exit (1);
    }
}

/* Take two pictures and use the better pixels from both */
void hdrImage (PMDHandle hnd)
{
  int res;
  float amplitude[2][204*204];
  float distance[2][204*204];
  
  /* Take a picture with the short integration time */
  res = pmdSetIntegrationTime (hnd, 0, SHORT_TIME);
  checkError (hnd, res);

  res = pmdUpdate (hnd);
  checkError (hnd, res);

  /* space of 204*204 float */
  res = pmdGetAmplitudes (hnd, amplitude[0],
                          sizeof (float) * 204 * 204);
  checkError (hnd, res);

  res = pmdGetDistances (hnd, distance[0],
                         sizeof (float) * 204 * 204);
  checkError (hnd, res);

  /* Take a picture with the long integration time */
  res = pmdSetIntegrationTime (hnd, 0, LONG_TIME);
  checkError (hnd, res);

  res = pmdUpdate (hnd);
  checkError (hnd, res);

  res = pmdGetAmplitudes (hnd, amplitude[1],
                          sizeof (float) * 204 * 204);
  checkError (hnd, res);

  res = pmdGetDistances (hnd, distance[1],
                              sizeof (float) * 204 * 204);
  checkError (hnd, res);

  printf ("A distance from the middle (short): %f m (a = %f)\n",
          distance[0][204 * 102 + 102], amplitude[0][204 * 102 + 102]);

  printf ("A distance from the middle (long): %f m (a = %f)\n",
          distance[1][204 * 102 + 102], amplitude[1][204 * 102 + 102]);

  /* Check every pixel: If the amplitude is too low, use
     the long measurement, otherwise keep the short one */
  for (unsigned i = 0; i < 204 * 204; ++i)
    {
      if (amplitude[0][i] < AMPL_THRESHOLD)
        {
          amplitude[0][i] = amplitude[1][i];
          distance[0][i] = distance[1][i];
        }
    }

  /* amplitude[0] and distance[0] now contain the merged
     data */
  char a='a';
  saveData<float>(&a, distance[0], 204*204);

  printf ("Selected distance from the middle: %f m\n",
          distance[0][204 * 102 + 102]);
}

void saveDataToCVS(float *data, int cRows, int cCols){
	char fileName[20] = "data.cvs";
	ofstream dataFile(fileName, ios::out);
	if(!dataFile){
		cerr<<"Error in creating file!"<<endl;
		exit(1);
	}
	
	int size = cRows*cCols;
	cout<<"the size of data is "<<size<<endl;
	//dataFile.open();
	for(int i=0;i<size;i++){
		dataFile<<data[i];
		if(i%cCols==(cCols-1)){
			dataFile<<endl;
		} else {
			dataFile<<',';
		}
	}
	//dataFile.close();

}

template <typename T> bool saveData(char *pPath, T *pData, int iDataCount)
{
	FILE *pFile;

	pFile = fopen("data", "wb");

	if(pFile != NULL)
	{
		fwrite((void*)pData, sizeof(T)*iDataCount, 1, pFile);

		fclose(pFile);

		return true;
	}

	return false;
}