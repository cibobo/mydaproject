#include <math.h>
#include <iostream>

/**
 * Construct a Filter for the distance, which will seperate the data in several layers.
 */

class DistanceFilter {

private:
	/*
	 * To save the distance data of envieronment
	 */
	float *origArray;
	/*
	 * The minimal recogenized distance between the envieronment and the object
	 */
	float eps;
	/*
	 * The different rate of the distance data
	 *   r = #(different pixes)/#(all pixes)
	 */
	float diffRate;

public:
	DistanceFilter();
	DistanceFilter(float *dis);
	DistanceFilter(float *dis, float epsilon, float rate);
	~DistanceFilter();

	void Upgrade(float *dis);

	/*
	 * Seperate the data with the input distance data.
	 * The data from src will be keep, which has the different distance from the beginning.
	 * That's mean, just the data, that doesn't belongs to environment, will be hold in dst
	 */
	bool Filte(float *dis, float *src, float *dst);
};