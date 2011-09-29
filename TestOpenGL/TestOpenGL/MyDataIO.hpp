#ifndef _MYDATAIO_
#define _MYDATAIO_


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/*
*/
template <typename T> bool saveData(char *pPath, T *pData, int iDataCount)
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
template <typename T> bool loadData(char *pPath, T *pData, int iDataCount)
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