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

	const size_t   PAGE_SIZE           = 4 * 1024;
	const SIZE_T   BUFFER_SIZE         = PAGE_SIZE;
	uint8_t        buffer[BUFFER_SIZE] = {0};

// #define TEST_32_BIT_SPACE

#ifdef TEST_32_BIT_SPACE
	const size_t   NUM_BUFFERS_IN_BIT_SPACE = UINT_MAX / BUFFER_SIZE;
	const size_t   PROGRESS_DOT_MODULUS     = 1024;
#else
	const uint64_t NUM_BUFFERS_IN_BIT_SPACE = UINT64_MAX / BUFFER_SIZE;
	const size_t   PROGRESS_DOT_MODULUS     = 1024 * 1024;
#endif

	for (uint64_t offset = 0; offset < NUM_BUFFERS_IN_BIT_SPACE; ++offset)
	{
		if (0 == offset % PROGRESS_DOT_MODULUS)
		{
			cout << '.';
		}

		LPCVOID baseAddress = reinterpret_cast<LPCVOID>(offset * BUFFER_SIZE);

		SIZE_T numBytesRead = tryReadProcessMemory(
			hProcess,
			baseAddress,
			buffer,
			BUFFER_SIZE
		);

		if (0 < numBytesRead)
		{
			cout << endl << "Read " << numBytesRead << " bytes at address " << baseAddress << endl;
		}
	}

	cout << endl << "Finished" << endl;

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

	if (!readProcessMemorySucceeded && reportError)
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
