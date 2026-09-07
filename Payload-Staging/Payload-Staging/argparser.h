#ifndef ARGPARSE_H
#define ARGPARSE_H

#include <Windows.h>
#include <stdio.h>

#pragma warning(disable : 4996)

typedef enum _ENCRYPTION_ALGO {
    ALGO_NONE = 0,
    ALGO_XOR,
    ALGO_AES,
    ALGO_CHACHA20,
    ALGO_UNKNOWN
}ENCRYPTION_ALGO, * PENCRYPTION_ALGO;

typedef struct _OPTIONS {
    PCHAR input_file;
    PCHAR payload_file;
    PCHAR output_file;
    PCHAR chunk_name;
    BOOL show_help;
} OPTIONS, *POPTIONS;

#define PRINT_ERROR(x) printf("error: %s\n",x);

VOID PrintBanner();
VOID PrintHelp(PCHAR progname);
VOID PrintArguments(POPTIONS opts);
BOOL ParseArguments(INT argc, PCHAR argv[], POPTIONS opts);

#endif // !ARGPARSE_H
