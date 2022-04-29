// rwyRemaining_toUnity.cpp : Defines the entry point for the application.
//

#include "rwyRemaining_toUnity.h"

#define BUF_SIZE 256
TCHAR szName[] = TEXT("rwyRemaining_toUnity");

extern "C" __declspec(dllexport) int rwyRemaining_toUnity()
{
	HANDLE hMapFile;
	LPCTSTR pBuf;

	// create a file mapping obkect
	// which is a memory block that can be manipulated by MapViewOfFile
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		szName
	);
	if (hMapFile == NULL)
	{
		return 0;
	}
	pBuf = (LPTSTR)MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		BUF_SIZE
	);
	if (pBuf == NULL)
	{
		return 0;
	}
	//copy value from stored memory to variable
	int val;
	CopyMemory(&val, pBuf, sizeof(val));
	return val*10;
}
