#ifndef UTILS_H
#define UTILS_H

#include <Windows.h>
#include <stdio.h>

#include "utils.h"

#pragma warning(disable : 4996)

#define PRINT_ERROR(errName) printf("[!] Error at %s: %d\n", errName, GetLastError());

BOOL ReadFileFromPath(LPSTR pePath, PBYTE* pBytes, PDWORD dwSize);
BOOL SaveFileToPath(LPCSTR path, PBYTE pBytes, DWORD dwSize);
VOID PrintHeader(LPSTR Text, PBYTE pBytes, DWORD dwSize, DWORD Length, BOOL Break);

#endif // !UTILS_H
