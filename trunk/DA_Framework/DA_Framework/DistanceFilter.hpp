#include <math.h>
#include <iostream>
#include <cv.h>

#include "BildData.hpp"

/**
 * Construct a Filter for the distance, which will seperate the data in several layers.
 */

class DistanceFilter {
public:
	/*
	 * To save the distance data of envieronment
	 */
	float *origArray;
	/*
	 * The number of frames, which would be used for creating the Distance Filter
	 */
	int creatingFrames;
	/*
	 * The minimal recogenized distance between the envieronment and the object
	 */
	float eps;
	/*
	 * The different rate of the distance data
	 *   r = #(different pixes)/#(all pixes)
	 */
	float diffRate;


	DistanceFilter();
	DistanceFilter(float *dis);
	DistanceFilter(float *dis, float epsilon, float rate);

	DistanceFilter(BildData *bildData);
	DistanceFilter(BildData *bildData, float epsilon, float rate);
	~DistanceFilter();

	void Upgrade(float *dis);
	void Upgrade(BildData *bildData);

	/*
	 * Seperate the data with the input distance data.
	 * The data from src will be keep, which has the different distance from the beginning.
	 * That's mean, just the data, that doesn't belongs to environment, will be hold in dst
	 */
	bool Filte(float *dis, float *src, float *dst);

	bool DistanceFilter::Filte(BildData *bildData, float *dst);
};