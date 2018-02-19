// Win32 API test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "windows.h"

#include <fstream>
#include <iostream>

#include "stream_demultiplexer.h"
#include "Win32 API test.h"

using namespace std;

stream_demultiplexer<ostream> cout_log;
stream_demultiplexer<wostream> wcout_log;

int main()
{
	ofstream coutLogFile("cout.txt");
	cout_log.add_stream(coutLogFile);
	cout_log.add_stream(cout);

	wofstream wcoutLogFile("wcout.txt");
	wcout_log.add_stream(wcoutLogFile);
	wcout_log.add_stream(wcout);
	
	const DWORD PROCESS_ID = 11176;
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, PROCESS_ID);

	cout_log << "Process ID: " << PROCESS_ID << endl;
	cout_log << "Process handle: " << hProcess << endl;

	const size_t   PAGE_SIZE           = 4 * 1024;
	const SIZE_T   BUFFER_SIZE         = PAGE_SIZE;
	uint8_t        buffer[BUFFER_SIZE] = {0};

// #define TEST_32_BIT_SPACE

#ifdef TEST_32_BIT_SPACE
	const size_t   NUM_BUFFERS_IN_BIT_SPACE = UINT_MAX / BUFFER_SIZE;
	const size_t   PROGRESS_MODULUS         = 1024;
#else
	const uint64_t NUM_BUFFERS_IN_BIT_SPACE = UINT64_MAX / BUFFER_SIZE;
	const size_t   PROGRESS_MODULUS         = 1024 * 1024;
#endif

	for (uint64_t offset = 0; offset < NUM_BUFFERS_IN_BIT_SPACE; ++offset)
	{
		uint64_t u64BaseAddress = offset * BUFFER_SIZE;

		if (0 == offset % PROGRESS_MODULUS)
		{
			cout_log << "Reached address " << hex << u64BaseAddress << endl;
			fflush(stdout);
		}

		SIZE_T numBytesRead = tryReadProcessMemory(
			hProcess,
			reinterpret_cast<LPCVOID>(u64BaseAddress),
			buffer,
			BUFFER_SIZE
		);

		if (0 < numBytesRead)
		{
			cout_log << endl << "Read " << numBytesRead << " bytes at address " << hex << u64BaseAddress << endl;
		}
	}

	cout_log << endl << "Finished" << endl;

	getchar();

#ifdef REDIRECT_STDOUT_TO_FILE
	fclose(stream);
#endif

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
		cout_log  << "Error code: " << error << endl;
		wcout_log << L"Error message:\n" << message << endl;
	}
	else
	{
		DWORD formatMessageError = GetLastError();
		cout_log << "FormatMessageW() failed with error " << formatMessageError << endl;
	}
}
