#include "stdafx.h"
#include "Parameters.hpp"

Parameters::Parameters(){
	MINFEATURECOUNT = 42;
	MAXFEATURECOUNT = 65;
	MAXDETECTLOOPS = 30;
	balance = 200;
	contrast = 10;

	MAXSIZE = 8;
	responseThreshold = 70;
	LINETHRESHOLDPROJECTED = 5;
	LINETHRESHOLDBINARIZED = 6;
	SUPPRESSNONMAXSIZE = 1;
}