#include "header.h"

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

BOOL SetChunk(LPCSTR Name, PBYTE pChunk, DWORD dwChunkSize, PBYTE pBinBytes, DWORD dwBinSize) {
	DWORD length = dwChunkSize;
	DWORD crcSize = 4 + dwBinSize;
	PBYTE crcBuffer = HeapAlloc(GetProcessHeap(), 0, crcSize);

	if (!crcBuffer) {
		PRINT_ERROR("Could not get memory for CRC!");
		return FALSE;
	}

	pChunk[0] = (length >> 24) & 0xFF;
	pChunk[1] = (length >> 16) & 0xFF;
	pChunk[2] = (length >> 8) & 0xFF;
	pChunk[3] = length & 0xFF;

	memcpy(pChunk + 4, Name, 4);
	memcpy(pChunk + 8, pBinBytes, dwBinSize);
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

int main(int argc, char* argv[]) {

	OPTIONS opts;

	PrintBanner();
	if (!ParseArguments(argc, argv, &opts)) {
		if (opts.show_help) {
			PrintHelp(argv[0]);
		}
		return -1;
	}
	PrintArguments(&opts);


	PBYTE pPngStart = NULL;
	PBYTE pPngBytes = NULL;
	DWORD dwPngSize = 0;

	PBYTE pBinBytes = NULL;
	DWORD dwBinSize = 0;

	if (strlen(opts.chunk_name) != 4) {
		PRINT_ERROR("Name must be only 4 words!");
		return -1;
	}

	if (!ReadFileFromPath(opts.input_file, &pPngBytes, &dwPngSize)) {
		PRINT_ERROR("ReadFileFromPath while reading png!");
		return -1;
	}

	if (memcmp(pPngBytes, PNG_SIGNATURE, sizeof(PNG_SIGNATURE)) != 0) {
		PRINT_ERROR("Invalid png format!");
		return -1;
	}

	if (!ReadFileFromPath(opts.payload_file, &pBinBytes, &dwBinSize)) {
		PRINT_ERROR("ReadFileFromPath while reading bin!");
		return -1;
	}
	pPngStart = pPngBytes;
	pPngBytes += sizeof(PNG_SIGNATURE);

	printf("[*] Extracting chunks:\n");
	while (TRUE)
	{
		printf("\t- Chunk %.4s: ",(PCHAR)(pPngBytes + 4));
		if (memcmp(pPngBytes + 4, "IEND", 4) == 0) {
			printf("found!\n");
			break;
		}
		printf("skipped\n");
		pPngBytes += GetChunkSize(pPngBytes);
	}

	printf("\n[*] Injecting payload:\n");

	PBYTE pIEND = NULL;
	DWORD dwIENDSize = GetChunkSize(pPngBytes);

	PBYTE pPayloadChunk= NULL;
	PBYTE pOutput = NULL;
	DWORD dwPayloadChunkSize = dwBinSize + 12; // 12 is the new chunk header size 
	DWORD dwOutputSize = dwPayloadChunkSize + dwPngSize ;
	DWORD beforeIEND = (DWORD)(pPngBytes - pPngStart);

	pIEND = (PBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwIENDSize);
	if (pIEND == NULL) {
		PRINT_ERROR("Could not get memory for IEND!");
		return -1;
	}

	memcpy(pIEND, pPngBytes, dwIENDSize);

	pPayloadChunk = (PBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwPayloadChunkSize);
	if (pPayloadChunk == NULL) {
		PRINT_ERROR("Could not get memory for pPayloadChunk!");
		return -1;
	}

	pOutput = (PBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwOutputSize);
	if (pOutput == NULL) {
		PRINT_ERROR("Could not get memory for output!");
		return -1;
	}

	if (!SetChunk(opts.chunk_name, pPayloadChunk, dwPayloadChunkSize, pBinBytes, dwBinSize)) {
		PRINT_ERROR("SetChunk while making new chunk!");
		return -1;
	}

	memcpy(pOutput, pPngStart, dwPngSize - dwIENDSize);
	memcpy(pOutput + (dwPngSize - dwIENDSize), pPayloadChunk, dwPayloadChunkSize);
	memcpy(pOutput + (dwPngSize - dwIENDSize) + dwPayloadChunkSize, pIEND, dwIENDSize);

	printf("\t - Chunk %s: injected!\n", opts.chunk_name);

	if (!SaveFileToPath(opts.output_file, pOutput, dwOutputSize)) {
		PRINT_ERROR("SaveFileToPath while writing png!");
		return -1;
	}

	printf("\n[+] New image saved at \"%s\"\n\n", opts.output_file);

	return 0;

}