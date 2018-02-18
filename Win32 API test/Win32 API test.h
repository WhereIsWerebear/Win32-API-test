#pragma once

SIZE_T tryReadProcessMemory(HANDLE hProcess, LPCVOID baseAddress, LPVOID buffer, SIZE_T bufferSize, bool reportError = false);

void reportWinError(DWORD error);
