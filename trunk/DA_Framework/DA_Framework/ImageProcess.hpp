#include <cv.h>

#include "BildData.hpp"

class ImageProcess{
public:
   /**
	* Gaussian Filter
	* Using 1 dimentional Gaussian Filter to filter the depth daten of the 3D Daten
	**/
	void static filterDepthDate(float threeDData[], double sigma);

	/**
	 * Translate the float data array to Matrix with 1 Channel in OpenCV
	 **/
	void static transFloatToMat(float *src, Mat &dst, float balance, float contrast);
	/**
	 * Translate the float data array to Matrix with 3 Channels in OpenCV
	 **/
	void static transFloatToMat3(float *src, Mat &dst, float balance, float contrast);

	void static transFloatToChar(float *src, unsigned char *dst, float balance, float contrast);
};