#include "stdafx.h"
#include "PMDCamIO.hpp"

#define AMPLITUDE
/* Some arbitrary values for the integration times and the
   amplitude threshold. These may not be practical for all
   situations.
*/
using namespace std;

//the path of the saved Data
const char *distanceSavePath = "data/distance/";
const char *amplitudeSavePath = "data/amplitude/";
//the pointer of the saved Data Array
float *ptrPMDData;
// Result code for PMDSDK2 calls 
int res;
PMDHandle hnd;

int frameCount = 0;

bool isDataSaved = false;


BOOL createPMDCon(){
	res = pmdOpen (&hnd, "plugin\\camcube0", "", "plugin\\camcubeproc0", "");

	// Check if the call succeeded 
	if (res != PMD_OK){
		char err[256];
		/* The handle parameter in pmdGetLastError is 0
			because no connection was established */
		pmdGetLastError (0, err, 256);
		fprintf (stderr, "Could not connect: %s\n", err);

		return FALSE;
	}
	//create the directory of the new data
	//createDefaultPMDDataDirectory();

	return TRUE;
}


void closePMDCon()
{
	pmdClose(hnd);
}

/* 
	We put error handling in an extra function to keep
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
void hdrImage ()
{
  int res;

  
  float amplitude[2][204*204];
  float distance[2][204*204];
  float intensity[204*204];

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


  res = pmdGetIntensities (hnd, intensity, sizeof (float) * 204 * 204);

  checkError (hnd, res);

  /* Check every pixel: If the amplitude is too low, use
     the long measurement, otherwise keep the short one */
  for (unsigned i = 0; i < 204 * 204; ++i)
    {
      if (amplitude[0][i] < AMPL_THRESHOLD)
        {
          amplitude[0][i] = amplitude[1][i];
          distance[0][i] = distance[1][i];
		  //float temp = distance[0][i] + distance[1][i];
		  //distance[0][i] = temp/2;
        }
    }

  /* amplitude[0] and distance[0] now contain the merged
     data */
  //saveNormalDataToFile("distance", frameCount, distance[0]);
  //saveNormalDataToFile("amplitude", frameCount, amplitude[0]);
  //saveNormalDataToFile("intensity", frameCount, intensity);
  frameCount++;
  ptrPMDData = distance[0];
  printf ("Selected distance from the middle: %f m\n",
          distance[0][204 * 102 + 102]);
}


void getAmpData(float *ampData){
	int res;

	res = pmdSetIntegrationTime (hnd, 0, LONG_TIME);
	checkError (hnd, res);

	res = pmdUpdate (hnd);
	checkError (hnd, res);

	/* space of 204*204 float */
	res = pmdGetAmplitudes (hnd, ampData,
							  sizeof (float) * 204 * 204);
	checkError (hnd, res);
}

void getIntData(float *intData){
	int res;

	res = pmdSetIntegrationTime (hnd, 0, LONG_TIME);
	checkError (hnd, res);

	res = pmdUpdate (hnd);
	checkError (hnd, res);

	/* space of 204*204 float */
	res = pmdGetIntensities (hnd, intData,
							  sizeof (float) * 204 * 204);
	checkError (hnd, res);
}


void getPMDData(float *disData, float *intData, float *ampData){
	int res;

	// array to save the data of long integration time
	float ampDataL[204*204];
	float disDataL[204*204];
	float intDataL[204*204];

	/* Take a picture with the short integration time */
	res = pmdSetIntegrationTime (hnd, 0, LONG_TIME);
	checkError (hnd, res);

	res = pmdUpdate (hnd);
	checkError (hnd, res);

	/* space of 204*204 float */
	res = pmdGetAmplitudes (hnd, ampData,
							  sizeof (float) * 204 * 204);
	checkError (hnd, res);

	res = pmdGetDistances (hnd, disData,
							 sizeof (float) * 204 * 204);
	checkError (hnd, res);

	res = pmdGetIntensities(hnd, intData,
							 sizeof (float) * 204 * 204);
	checkError (hnd, res);

	
	/* Take a picture with the long integration time */
	//res = pmdSetIntegrationTime (hnd, 0, LONG_TIME);
	//checkError (hnd, res);

	//res = pmdUpdate (hnd);
	//checkError (hnd, res);

	//res = pmdGetAmplitudes (hnd, ampDataL,
	//						  sizeof (float) * 204 * 204);
	//checkError (hnd, res);

	//res = pmdGetDistances (hnd, disDataL,
	//							  sizeof (float) * 204 * 204);
	//checkError (hnd, res);

	//res = pmdGetIntensities (hnd, intDataL, 
	//							  sizeof (float) * 204 * 204);
	//checkError (hnd, res);

	/* 
	 * Check every pixel: If the amplitude is too low, use
	 * the long measurement, otherwise keep the short one 
	 */
	//for (unsigned i = 0; i < 204 * 204; ++i)
	//{
	//  if (ampData[i] < AMPL_THRESHOLD)
	//	{
	//	  ampData[i] = ampDataL[i];
	//	  disData[i] = disDataL[i];
	//	  intData[i] = intDataL[i];
	//	  //float temp = distance[0][i] + distance[1][i];
	//	  //distance[0][i] = temp/2;
	//	}
	//}

	if(isDataSaved){
		saveNormalDataToFile("distance", frameCount, disData);
		saveNormalDataToFile("amplitude", frameCount, ampData);
		saveNormalDataToFile("intensity", frameCount, intData);
	}
	frameCount++;
}

void setIsDataSaved(bool isSaved){
	isDataSaved = isSaved;
}
	
//float* getPMDDataPointer(){
//	return ptrPMDData;
//}

//void calibration(){
//	{
//		using namespace cv;
//		using namespace std;
//
//		const int board_dt = 20;
//
//		int board_w = 6; // Board width in squares
//		int board_h = 8; // Board height 
//		int n_boards = 8; // Number of boards
//		int board_n = board_w * board_h;
//		Size board_sz = Size( board_w, board_h );
//		//CvCapture* capture = cvCreateCameraCapture( 0 );
//		//assert( capture );
//
//		namedWindow( "Calibration" );
//		// Allocate Sotrage
//		CvMat* image_points			= cvCreateMat( n_boards*board_n, 2, CV_32FC1 );
//		CvMat* object_points		= cvCreateMat( n_boards*board_n, 3, CV_32FC1 );
//		CvMat* point_counts			= cvCreateMat( n_boards, 1, CV_32SC1 );
//		//CvMat* intrinsic_matrix		= cvCreateMat( 3, 3, CV_32FC1 );
//		//CvMat* distortion_coeffs	= cvCreateMat( 5, 1, CV_32FC1 );
//		Mat intrinsic_matrix = Mat(3,3,CV_32FC1);
//		Mat distortion_coeffs = Mat(5,1,CV_32FC1);
//
//		vector<Point2f> corners;
//		int corner_count;
//		int successes = 0;
//		int step, frame = 0;
//
//		float floatData[204*204];
//		unsigned char data[204*204];
//
//		//setDefaultLoadPath("Markers1");
//		//loadNormalDataFromFile("intensity", frame, floatData);
//		getAmpData(floatData);
//
//		transAmplitudeToGrayValue(floatData, data);
//
//		Mat image = Mat(204, 204, CV_8UC1, data);
//
//		while( successes < n_boards ){
//			// Skip every board_dt frames to allow user to move chessboard
//			if( frame++ % board_dt == 0 ){
//				// Find chessboard corners:
//				bool found = findChessboardCorners(image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
//
//				// Get subpixel accuracy on those corners
//				if(found){
//					cornerSubPix(image, corners, cvSize( 11, 11 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
//				}
//
//				// Draw it
//				drawChessboardCorners( image, board_sz, Mat(corners), found );
//				imshow( "Calibration", image );
//
//				// If we got a good board, add it to our data
//				if( corners.size() == board_n ){
//					step = successes*board_n;
//					for( int i=step, j=0; j < board_n; ++i, ++j ){
//						CV_MAT_ELEM( *image_points, float, i, 0 ) = corners[j].x;
//						CV_MAT_ELEM( *image_points, float, i, 1 ) = corners[j].y;
//						CV_MAT_ELEM( *object_points, float, i, 0 ) = j/board_w;
//						CV_MAT_ELEM( *object_points, float, i, 1 ) = j%board_w;
//						CV_MAT_ELEM( *object_points, float, i, 2 ) = 0.0f;
//					}
//					CV_MAT_ELEM( *point_counts, int, successes, 0 ) = board_n;
//					successes++;
//					cout<<successes<<" successful Snapshots out of "<<n_boards<<" collected."<<endl;
//				}
//			} 
//
//			// Handle pause/unpause and ESC
//			int c = cvWaitKey( 15 );
//			if( c == 'p' ){
//				c = 0;
//				while( c != 'p' && c != 27 ){
//					c = cvWaitKey( 250 );
//				}
//			}
//			if( c == 27 )
//				exit(0);
//			//image = cvQueryFrame( capture ); // Get next image
//			
//			getAmpData(floatData);
//			transAmplitudeToGrayValue(floatData, image.data);
//			//image = Mat(204, 204, CV_8UC1, data);
//			
//		} // End collection while loop
//		
//		cout<<"\n\n *** Calbrating the camera now...\n";
//		
//		// Allocate matrices according to how many chessboards found
//		//CvMat* object_points2 = cvCreateMat( successes*board_n, 3, CV_32FC1 );
//		//CvMat* image_points2 = cvCreateMat( successes*board_n, 2, CV_32FC1 );
//		//CvMat* point_counts2 = cvCreateMat( successes, 1, CV_32SC1 );
//		//
//		//// Transfer the points into the correct size matrices
//		//for( int i = 0; i < successes*board_n; ++i ){
//		//	CV_MAT_ELEM( *image_points2, float, i, 0) = CV_MAT_ELEM( *image_points, float, i, 0 );
//		//	CV_MAT_ELEM( *image_points2, float, i, 1) = CV_MAT_ELEM( *image_points, float, i, 1 );
//		//	CV_MAT_ELEM( *object_points2, float, i, 0) = CV_MAT_ELEM( *object_points, float, i, 0 );
//		//	CV_MAT_ELEM( *object_points2, float, i, 1) = CV_MAT_ELEM( *object_points, float, i, 1 );
//		//	CV_MAT_ELEM( *object_points2, float, i, 2) = CV_MAT_ELEM( *object_points, float, i, 2 );
//		//}
//
//		vector<vector<Point3f>> object_points2;
//		vector<vector<Point2f>> image_points2;
//
//		for( int i = 0; i < successes; i++ ){
//			for(int j=0; j < board_n; j++){
//				image_points2[i][j].x = CV_MAT_ELEM( *image_points, float, i*board_n+j, 0 );
//				image_points2[i][j].y = CV_MAT_ELEM( *image_points, float, i*board_n+j, 1 );
//
//				object_points2[i][j].x = CV_MAT_ELEM( *object_points, float, i*board_n+j, 0 );
//				object_points2[i][j].y = CV_MAT_ELEM( *object_points, float, i*board_n+j, 1 );
//				object_points2[i][j].z = CV_MAT_ELEM( *object_points, float, i*board_n+j, 2 );
//			}
//		
//		}
//
//		//for( int i=0; i < successes; ++i ){
//		//	CV_MAT_ELEM( *point_counts2, int, i, 0 ) = CV_MAT_ELEM( *point_counts, int, i, 0 );
//		//}
//		cvReleaseMat( &object_points );
//		cvReleaseMat( &image_points );
//		cvReleaseMat( &point_counts );
//		
//		// At this point we have all the chessboard corners we need
//		// Initiliazie the intrinsic matrix such that the two focal lengths
//		// have a ratio of 1.0
//		//CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0;
//		//CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0;
//		intrinsic_matrix.at<float>(0,0) = 1.0;
//		intrinsic_matrix.at<float>(1,1) = 1.0;
//
//		vector<Mat> rvecs;
//		vector<Mat> tvecs;
//
//		// Calibrate the camera
//		calibrateCamera(object_points2, image_points2, image.size(), intrinsic_matrix, distortion_coeffs, rvecs, tvecs, CV_CALIB_FIX_ASPECT_RATIO); 
//
//		//Save values to file  
//		cout<<" *** Calibration Done!\n\n";  
//		cout<<"Storing Intrinsics.xml and Distortions.xml files...\n";  
//		cvSave(".\data\Intrinsics.xml",&intrinsic_matrix);  
//		cvSave(".\data\Distortion.xml",&distortion_coeffs);  
//		cout<<"Files saved.\n\n";    
//		cout<<"Starting corrected display...."; 
//
//
//		// Example of loading these matrices back in
//		Mat intrinsic = Mat(intrinsic_matrix);
//		Mat distortion = Mat(distortion_coeffs);
//
//		// Build the undistort map that we will use for all subsequent frames
//		//IplImage* mapx = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
//		//IplImage* mapy = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
//		//cvInitUndistortMap( intrinsic, distortion, mapx, mapy );
//
//		Size imgSize = image.size();
//		Mat mapx = Mat(imgSize, CV_32FC1);
//		Mat mapy = Mat(imgSize, CV_32FC1);
//		initUndistortRectifyMap(intrinsic, distortion, NULL, intrinsic, imgSize, CV_32FC1, mapx, mapy);
//
//		// Run the camera to the screen, now showing the raw and undistorted image
//		namedWindow( "Undistort" );
//
//		while( !image.empty() ){
//			//IplImage *t = cvCloneImage( image );
//			//cvShowImage( "Calibration", image ); // Show raw image
//			//cvRemap( t, image, mapx, mapy ); // undistort image
//			//cvReleaseImage( &t );
//			//cvShowImage( "Undistort", image ); // Show corrected image
//			imshow("Calibration", image);
//			Mat resultImg = Mat(imgSize, CV_8UC1, data);
//			remap(image, resultImg, mapx, mapy, INTER_LINEAR);
//			imshow("Undistort", resultImg);
//
//			// Handle pause/unpause and esc
//			int c = cvWaitKey( 15 );
//			if( c == 'p' ){
//				c = 0;
//				while( c != 'p' && c != 27 ){
//					c = cvWaitKey( 250 );
//				}
//			}
//			if( c == 27 )
//				break;
//			getAmpData(floatData);
//			transAmplitudeToGrayValue(floatData, image.data);
//			/*Mat image = Mat(204, 204, CV_8UC1, data);*/
//		}
//	}
//}

/*
 * The code copied from OpenCV Tutor 
 * http://dasl.mem.drexel.edu/~noahKuntz/openCVTut10.html
 */
void calibration(){
	int board_w = 6; // Board width in squares
	int board_h = 8; // Board height 
	int n_boards = 30; // Number of boards
	const int board_dt = 20;
	int board_n = board_w * board_h;
	CvSize board_sz = cvSize( board_w, board_h );

	cvNamedWindow( "Calibration" );
	// Allocate Sotrage
	CvMat* image_points		= cvCreateMat( n_boards*board_n, 2, CV_32FC1 );
	CvMat* object_points		= cvCreateMat( n_boards*board_n, 3, CV_32FC1 );
	CvMat* point_counts			= cvCreateMat( n_boards, 1, CV_32SC1 );
	CvMat* intrinsic_matrix		= cvCreateMat( 3, 3, CV_32FC1 );
	CvMat* distortion_coeffs	= cvCreateMat( 5, 1, CV_32FC1 );

	CvPoint2D32f* corners = new CvPoint2D32f[ board_n ];
	int corner_count;
	int successes = 0;
	int step, frame = 0;

	float floatData[204*204];
	unsigned char data[204*204];
#ifdef AMPLITUDE
	getAmpData(floatData);
	transAmplitudeToGrayValue(floatData, data);
#else
	getIntData(floatData);
	transIntensityToGrayValue(floatData, data);
#endif
	cv::Mat imageData = cv::Mat(204, 204, CV_8UC1, data);

	//IplImage *image = cvQueryFrame( capture );
	IplImage *image = &IplImage(imageData);
	IplImage *gray_image = cvCreateImage( cvGetSize( image ), 8, 1 );

	// Capture Corner views loop until we've got n_boards
	// succesful captures (all corners on the board are found)

	while( successes < n_boards ){
		// Skp every board_dt frames to allow user to move chessboard
		if( frame++ % board_dt == 0 ){
			// Find chessboard corners:
			int found = cvFindChessboardCorners( image, board_sz, corners,
				&corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );

			// Get subpixel accuracy on those corners
			//cvCvtColor( image, gray_image, CV_BGR2GRAY );
			cvFindCornerSubPix( image, corners, corner_count, cvSize( 11, 11 ), 
				cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

			// Draw it
			cvDrawChessboardCorners( image, board_sz, corners, corner_count, found );
			cvShowImage( "Calibration", image );

			// If we got a good board, add it to our data
			if( corner_count == board_n ){
				step = successes*board_n;
				for( int i=step, j=0; j < board_n; ++i, ++j ){
					CV_MAT_ELEM( *image_points, float, i, 0 ) = corners[j].x;
					CV_MAT_ELEM( *image_points, float, i, 1 ) = corners[j].y;
					CV_MAT_ELEM( *object_points, float, i, 0 ) = j/board_w;
					CV_MAT_ELEM( *object_points, float, i, 1 ) = j%board_w;
					CV_MAT_ELEM( *object_points, float, i, 2 ) = 0.0f;
				}
				CV_MAT_ELEM( *point_counts, int, successes, 0 ) = board_n;
				successes++;
				cout<<successes<<" successful Snapshots out of "<<n_boards<<" collected."<<endl;
			}
		} 

		// Handle pause/unpause and ESC
		int c = cvWaitKey( 15 );
		if( c == 'p' ){
			c = 0;
			while( c != 'p' && c != 27 ){
				c = cvWaitKey( 250 );
			}
		}
		if( c == 27 )
			exit(0);
		//image = cvQueryFrame( capture ); // Get next image
#ifdef AMPLITUDE
		getAmpData(floatData);
		transAmplitudeToGrayValue(floatData, (unsigned char*)image->imageData);
#else
		getIntData(floatData);
		transIntensityToGrayValue(floatData, (unsigned char*)image->imageData);
#endif
	} // End collection while loop

	// Allocate matrices according to how many chessboards found
	CvMat* object_points2 = cvCreateMat( successes*board_n, 3, CV_32FC1 );
	CvMat* image_points2 = cvCreateMat( successes*board_n, 2, CV_32FC1 );
	CvMat* point_counts2 = cvCreateMat( successes, 1, CV_32SC1 );
	
	// Transfer the points into the correct size matrices
	for( int i = 0; i < successes*board_n; ++i ){
		CV_MAT_ELEM( *image_points2, float, i, 0) = CV_MAT_ELEM( *image_points, float, i, 0 );
		CV_MAT_ELEM( *image_points2, float, i, 1) = CV_MAT_ELEM( *image_points, float, i, 1 );
		CV_MAT_ELEM( *object_points2, float, i, 0) = CV_MAT_ELEM( *object_points, float, i, 0 );
		CV_MAT_ELEM( *object_points2, float, i, 1) = CV_MAT_ELEM( *object_points, float, i, 1 );
		CV_MAT_ELEM( *object_points2, float, i, 2) = CV_MAT_ELEM( *object_points, float, i, 2 );
	}

	for( int i=0; i < successes; ++i ){
		CV_MAT_ELEM( *point_counts2, int, i, 0 ) = CV_MAT_ELEM( *point_counts, int, i, 0 );
	}
	cvReleaseMat( &object_points );
	cvReleaseMat( &image_points );
	cvReleaseMat( &point_counts );

	// At this point we have all the chessboard corners we need
	// Initiliazie the intrinsic matrix such that the two focal lengths
	// have a ratio of 1.0

	CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0;
	CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0;

	// Calibrate the camera
	cvCalibrateCamera2( object_points2, image_points2, point_counts2, cvGetSize( image ), 
		intrinsic_matrix, distortion_coeffs, NULL, NULL, CV_CALIB_FIX_ASPECT_RATIO ); 

	// Save the intrinsics and distortions
	cvSave( "data/Intrinsics.xml", intrinsic_matrix );
	cvSave( "data/Distortion.xml", distortion_coeffs );

	// Example of loading these matrices back in
	CvMat *intrinsic = (CvMat*)cvLoad( "data/Intrinsics.xml" );
	CvMat *distortion = (CvMat*)cvLoad( "data/Distortion.xml" );

	// Build the undistort map that we will use for all subsequent frames
	IplImage* mapx = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
	IplImage* mapy = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
	cvInitUndistortMap( intrinsic, distortion, mapx, mapy );

	// Run the camera to the screen, now showing the raw and undistorted image
	cvNamedWindow( "Undistort" );

	while( image ){
		IplImage *t = cvCloneImage( image );
		cvShowImage( "Calibration", image ); // Show raw image
		cvRemap( t, image, mapx, mapy ); // undistort image
		cvReleaseImage( &t );
		cvShowImage( "Undistort", image ); // Show corrected image

		// Handle pause/unpause and esc
		int c = cvWaitKey( 15 );
		if( c == 'p' ){
			c = 0;
			while( c != 'p' && c != 27 ){
				c = cvWaitKey( 250 );
			}
		}
		if( c == 27 )
			break;
#ifdef AMPLITUDE
		getAmpData(floatData);
		transAmplitudeToGrayValue(floatData, (unsigned char*)image->imageData);
#else
		getIntData(floatData);
		transIntensityToGrayValue(floatData, (unsigned char*)image->imageData);
#endif
	}
}