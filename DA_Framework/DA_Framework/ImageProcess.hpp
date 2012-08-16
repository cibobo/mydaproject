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
	 * Translate the float data array to Matrix in OpenCV
	 **/
	void static transFloatToMat(float *src, Mat &dst, float balance, float contrast);
};