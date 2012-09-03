#ifndef _MYDATAIO_
#define _MYDATAIO_

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include <highgui.h>

#include "BildData.hpp"

bool isDirectoryExist(const char *directory);

bool createDirectory(const char *directory);

void createDefaultPMDDataDirectory(const char *subPath);

void saveAllDataToFile(int frameCount, BildData *bildData);

void saveNormalDataToFile(const char *type, int frameCount, float *data);

void saveNormalDataToPNG(const char *subPath, int frameCount, Mat data);

void loadAllDataFromFile(int frameCount, BildData *bildData);

void loadNormalDataFromFile(const char *type, int frameCount, float *data);

void setDefaultLoadPath(const char* subPath);

unsigned char * loadNormalDataForAR(int frameCount);

/*
*/
template <typename T> bool saveData(const char *pPath, T *pData, int iDataCount)
{
	FILE *pFile;
	pFile = fopen(pPath, "wb");
	if(pFile != NULL)
	{
		fwrite((void*)pData, sizeof(T)*iDataCount, 1, pFile);
		fclose(pFile);
		return true;
	}
	return false;
}

/*
*/
template <typename T> bool loadData(const char *pPath, T *pData, int iDataCount)
{
	FILE *pFile;
	pFile = fopen(pPath, "rb");
	if(pFile != NULL)
	{
		fread((void*)pData, sizeof(T), iDataCount, pFile);
		fclose(pFile);
		return true;
	}
	return false;
}

#endif