#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <array>

using namespace std;

#define BUF_SIZE 256
TCHAR szName[] = TEXT("TestMappingObject");
TCHAR szMsg[] = TEXT("1024");
//byte szMsg[];
//int integer = 12;

int _tmain()
{	
	system("@echo off");
	system("color 37");
	system("title Mem_Writer");

	HANDLE hMapFile;
	LPCTSTR pBuf;
	
	// create a file mapping object
	// which is a memory block that can be manipulated by MapViewOfFile
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,		// use paging file
		NULL,						// default security
		PAGE_READWRITE,				// read/write access
		0,							// maximum object size (high-order DWORD)
		BUF_SIZE,					// maxmimum object size (low-order DWORD)
		szName						// name of mapping object
	);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d).\n")
			,GetLastError());
		return 1;
	}
	
	// map view of the file to be able to read/write to a location
	pBuf = (LPTSTR) MapViewOfFile(
		hMapFile,					// handle to map object
		FILE_MAP_ALL_ACCESS,		// read/write permission
		0,
		0,
		BUF_SIZE
	);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("Could not create file mapping object (%d) .\n"),
			GetLastError());
		CloseHandle(hMapFile);
		return 1;
	}

	//copy and store memory block in an address
	//unmap the file view and close the handle at the end
	/*if (pBuf != NULL)
	{
		CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
		cout << "[OK] Variable stored commited to shared memory.\n";
		cout << "[??] Press any key to unpage memory file.";
		(void)_getch();
		UnmapViewOfFile(pBuf);
		CloseHandle(hMapFile);
	}*/

	if (pBuf != NULL)
	{	
		//assign values to be copied to shared memory
		//assign array of size 5, of type int with the variable name of arr
		std::array<int, 5> arr{ 1, 2, 3, 42, 5 };
		float val = 3.1456;

		/*copy variables of array(source) to shared memory location(destination),
		with the size being the number of array elements multiplied by the size of each array element,
		dependent on the value type declared*/
		CopyMemory((PVOID)pBuf, arr.data(), arr.size() * sizeof(decltype(arr)::value_type));
		CopyMemory((PVOID)(pBuf+100), &val, sizeof(val));

		cout << "[OK] Variable stored commited to shared memory.\n";
		cout << "[??] Press any key to unpage memory file.";
		(void)_getch();
		UnmapViewOfFile(pBuf);
		CloseHandle(hMapFile);
	}
}