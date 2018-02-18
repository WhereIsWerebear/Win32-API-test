// Win32 API test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "windows.h"

#include <iostream>
#include "Win32 API test.h"

using namespace std;

int main()
{
	const DWORD PROCESS_ID = 11176;
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, PROCESS_ID);

	cout << "Process ID: " << PROCESS_ID << endl;
	cout << "Process handle: " << hProcess << endl;

	const SIZE_T BUFFER_SIZE = 4;
	uint8_t buffer[BUFFER_SIZE] = {0};

	const size_t PAGE_SIZE = 4 * 1024;
	const size_t NUM_PAGES_IN_32_BIT_ADDRESS_SPACE = 1024 * 1024;
	for (size_t pageNum = 0; pageNum < NUM_PAGES_IN_32_BIT_ADDRESS_SPACE; ++pageNum)
	{
		if (0 == pageNum % 1024)
		{
			cout << '.';
		}

		LPCVOID baseAddress = reinterpret_cast<LPCVOID>(pageNum * PAGE_SIZE);

		SIZE_T numBytesRead = tryReadProcessMemory(
			hProcess,
			baseAddress,
			buffer,
			BUFFER_SIZE
		);

		if (0 < numBytesRead)
		{
			cout << endl << "We did it!" << endl;
			cout << "Base address: " << baseAddress << endl;
		}
	}

	getchar();

	return 0;
}

SIZE_T tryReadProcessMemory(HANDLE hProcess, LPCVOID baseAddress, LPVOID buffer, SIZE_T bufferSize, bool reportError)
{
	SIZE_T numBytesRead = 0;
	BOOL readProcessMemorySucceeded = ReadProcessMemory(
		hProcess,
		baseAddress,
		buffer,
		bufferSize,
		&numBytesRead);

	if (readProcessMemorySucceeded)
	{
		cout << "Read " << numBytesRead << "bytes" << endl;
	}
	else if (reportError)
	{
		reportWinError(GetLastError());
	}

	return numBytesRead;
}

void reportWinError(DWORD error)
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

	if (0 < messageSize)
	{
		cout << "Error code: " << error << endl;
		wcout << L"Error message:\n" << message << endl;
	}
	else
	{
		DWORD formatMessageError = GetLastError();
		cout << "FormatMessageW() failed with error " << formatMessageError << endl;
	}
}
