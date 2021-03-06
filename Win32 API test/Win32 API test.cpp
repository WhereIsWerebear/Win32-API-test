// Win32 API test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "windows.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "stream_demultiplexer.h"
#include "Win32 API test.h"

using namespace std;

stream_demultiplexer<ostream> cout_log;
stream_demultiplexer<wostream> wcout_log;

int main()
{
	ofstream  coutLogFile("cout.txt");
	wofstream wcoutLogFile("wcout.txt");

	cout_log.add_stream(coutLogFile);
	wcout_log.add_stream(wcoutLogFile);

#define PRINT_TO_STDOUT
#ifdef PRINT_TO_STDOUT
	cout_log.add_stream(cout);
	wcout_log.add_stream(wcout);
#endif
	
	const DWORD PROCESS_ID = 15168;
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ, false, PROCESS_ID);

	cout_log << "Process ID: " << PROCESS_ID << endl;
	cout_log << "Process handle: " << hProcess << endl;

	const size_t   PAGE_SIZE           = 4 * 1024;
	const SIZE_T   BUFFER_SIZE         = PAGE_SIZE;
	uint8_t        buffer[BUFFER_SIZE] = {0};

// #define TEST_32_BIT_SPACE
#ifdef TEST_32_BIT_SPACE
	const size_t   NUM_BUFFERS_IN_BIT_SPACE = UINT_MAX / BUFFER_SIZE;
	const size_t   PROGRESS_MODULUS         = 0x1000;
#else
	const uint64_t NUM_BUFFERS_IN_BIT_SPACE = UINT64_MAX / BUFFER_SIZE;
	const size_t   PROGRESS_MODULUS         = 0x10000000;
#endif

	bool inMiddleOfReadableRange = false;
	uint64_t u64BeginReadableRangeAddress = UINT64_MAX;
	size_t numBytesReadInReadableRange = 0;
	for (uint64_t offset = 0; offset < NUM_BUFFERS_IN_BIT_SPACE; ++offset)
	{
		uint64_t u64BaseAddress = offset * BUFFER_SIZE;

		if (0 == u64BaseAddress % PROGRESS_MODULUS && 0 != offset)
		{
			cout_log << "Reached address " << fmtHex(u64BaseAddress) << endl;
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
			if (!inMiddleOfReadableRange)
			{
				inMiddleOfReadableRange = true;
				u64BeginReadableRangeAddress = u64BaseAddress;
				numBytesReadInReadableRange = 0;

				cout_log
					<< "Found the beginning of a readable range at address "
					<< fmtHex(u64BeginReadableRangeAddress) << endl;
			}

			numBytesReadInReadableRange += numBytesRead;
		}
		else
		{
			if (inMiddleOfReadableRange)
			{
				cout_log << "Read " << numBytesReadInReadableRange
					<< " bytes from address "
					<< fmtHex(u64BeginReadableRangeAddress)
					<< " to the address before "
					<< fmtHex(u64BaseAddress) << endl;

				inMiddleOfReadableRange = false;
				u64BeginReadableRangeAddress = UINT64_MAX;
				numBytesReadInReadableRange = 0;
			}
		}
	}

	cout_log << endl << "Finished" << endl;

	getchar();

#ifdef REDIRECT_STDOUT_TO_FILE
	fclose(stream);
#endif

	return 0;
}

string fmtHex(uint64_t n)
{
	stringstream ss;
	ss << hex << n;

	string str;
	ss >> str;

	if (8 < str.length())
	{
		str.insert(str.length() - 8, "'");
	}

	str.insert(0, "0x");

	return str;
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
