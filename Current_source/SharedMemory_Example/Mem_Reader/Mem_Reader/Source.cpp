#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <array>
#pragma comment(lib,"user32.lib")

#include <iostream>
using namespace std;

#define BUF_SIZE 256
TCHAR szName[] = TEXT("TestMappingObject");

int _tmain()
{
	system("@echo off");
	system("color 27");
	system("title Mem_Reader");

	HANDLE hMapFile;
	LPCTSTR pBuf;
	
	// create a file mapping object
	// which is a memory block that can be manipulated by MapViewOfFile
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		szName
	);
	if (hMapFile == NULL)
	{
		_tprintf(TEXT("[:/] Could not open file mapping object (%d) .\n"),
			GetLastError());
		(void)_getch();
		return 1;
	}

	// map view of the file to be able to read/write to a location
	pBuf = (LPTSTR) MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		BUF_SIZE
	);


	if (pBuf == NULL)
	{
		_tprintf(TEXT("[:/] Could not map view of file (%d).\n"),
			GetLastError());
		CloseHandle(hMapFile);
		(void)_getch();
		return 1;
	}

	cout << "[--] Shared memory address " << pBuf << "\n";

	//declare empty variables to be reasigned with variables from shared memory
	std::array<int, 5> arr;
	float val;

	/*copy memory to byte data of array(destination) from the shared memory(source)
	with the size being the number of array elements multiplied by the size of each array element,
	dependent on its declare type*/
	CopyMemory(arr.data(), pBuf, arr.size() * sizeof(decltype(arr)::value_type));

	/*copy memory in to byte data (dereference) of single value from 100 bits into the memory location,
	with the size being the size of that value*/
	CopyMemory(&val, pBuf+100, sizeof(val));
	
	//for loop to iterate through each array element (C++11 standard)
	//constant dereference
	for (const int& i : arr)
		cout << i << ", "; cout << endl;

	//output float that has been extracted from shared memory
	cout << val << endl;

	//use wcout to be able to display text of type LPTSTR
	wcout << "[OK] Message recieved: " << *pBuf << "\n";

	(void)_getch();
}