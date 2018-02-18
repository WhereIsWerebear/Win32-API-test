// Win32 API test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "windows.h"

#include <iostream>

using namespace std;

int main()
{
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, 7208);
	std::cout << hProcess << std::endl;

	const SIZE_T BUFFER_SIZE = 128;
	char buffer[BUFFER_SIZE];
	SIZE_T numBytesRead = 0;

	BOOL readProcessMemorySucceeded = ReadProcessMemory(
		hProcess,
		reinterpret_cast<LPVOID>(static_cast<UINT_PTR>(0x101038b0)),
		buffer,
		BUFFER_SIZE,
		&numBytesRead);

	cout << readProcessMemorySucceeded << endl;
	cout << numBytesRead << endl;

	DWORD error = GetLastError();

	cout << error << endl;

#if 1
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
#endif

	getchar();

    return 0;
}

