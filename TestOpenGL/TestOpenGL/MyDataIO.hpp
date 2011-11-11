#ifndef _MYDATAIO_
#define _MYDATAIO_


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

bool isDirectoryExist(const char *directory);

bool createDirectory(const char *directory);

void createDefaultPMDDataDirectory(char *subPath);

void saveNormalDataToFile(const char *type, int frameCount, float *data);

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