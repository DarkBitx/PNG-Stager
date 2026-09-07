#include "utils.h"

BOOL ReadFileFromPath(LPSTR pePath, PBYTE* pBytes, PDWORD dwSize) {
	BOOL state = FALSE;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	PBYTE pBuffer = NULL;
	DWORD dwFSize = 0;
	DWORD dwBytesRead = 0;

	hFile = CreateFileA(pePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		PRINT_ERROR("CreateFileA");
		goto _END;
	}

	dwFSize = GetFileSize(hFile, NULL);
	if (dwFSize == INVALID_FILE_SIZE) {
		PRINT_ERROR("GetFileSize");
		goto _END;
	}

	pBuffer = (PBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwFSize);

	if (!ReadFile(hFile, pBuffer, dwFSize, &dwBytesRead, NULL) || dwBytesRead != dwFSize) {
		PRINT_ERROR("ReadFile");
		goto _END;
	}

	*pBytes = pBuffer;
	*dwSize = dwFSize;

	if (*pBytes != NULL && *dwSize != 0)
		state = TRUE;

_END:
	if (hFile)
		CloseHandle(hFile);

	return state;
}

BOOL SaveFileToPath(LPCSTR path, PBYTE pBytes, DWORD dwSize) {

	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD written = 0;

	hFile = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		PRINT_ERROR("CreateFileA");
		goto _END;
	}

	if (!WriteFile(hFile, pBytes, dwSize, &written, NULL)) {
		PRINT_ERROR("WriteFile");
		goto _END;
	}

_END:
	CloseHandle(hFile);
	return written == dwSize;
}

VOID PrintHeader(LPSTR Text, PBYTE pBytes, DWORD dwSize, DWORD Length, BOOL Break) {
	printf("%s", Text);
	for (DWORD i = 0; i < dwSize; i++) {
		if ((i % Length) == 0 && i != 0) {
			if (Break) {
				break;
			}
			printf("\n");
		}
		printf("%0.2X ", pBytes[i]);
	}
	printf("\n");
}
