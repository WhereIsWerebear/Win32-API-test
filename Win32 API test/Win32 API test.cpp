// Win32 API test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "windows.h"

#include <iostream>

using namespace std;

int main()
{
	const DWORD PROCESS_ID = 11176;
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, PROCESS_ID);
	std::cout << hProcess << std::endl;

	const SIZE_T BUFFER_SIZE = 1;
	char buffer[BUFFER_SIZE];
	SIZE_T numBytesRead = 0;

	const LPCVOID BASE_ADDRESS = 0;
	BOOL readProcessMemorySucceeded = ReadProcessMemory(
		hProcess,
		BASE_ADDRESS,
		buffer,
		BUFFER_SIZE,
		&numBytesRead);

	cout << readProcessMemorySucceeded << endl;
	cout << numBytesRead << endl;

	DWORD error = GetLastError();

	cout << error << endl;

	if (!readProcessMemorySucceeded)
	{
		const SIZE_T ERROR_MESSAGE_SIZE =1024;
		TCHAR message[ERROR_MESSAGE_SIZE];

		DWORD messageSize = FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM,
			nullptr,
			error,
			0,
			message,
			ERROR_MESSAGE_SIZE,
			nullptr);

		cout << messageSize << endl;

		if (0 == messageSize)
		{
			DWORD formatMessageError = GetLastError();
			cout << formatMessageError << endl;
		}

		wcout << message << endl;
		getchar();
	}

	getchar();

    return 0;
}

