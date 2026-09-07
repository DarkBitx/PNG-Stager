#include "argparser.h"

VOID PrintBanner() {
    printf(
        "\n"
        "====================================\n"
        "        PNG Payload Stager\n"
        "              DarkBit\n"
        "====================================\n\n"
    );
}

VOID PrintHelp(PCHAR progname) {
    printf("Usage: %s [options]\n", progname);
    printf("Options:\n");
    printf("  -f, --file    <file>       Input .png file (required)\n");
    printf("  -p, --payload <file>       Input .bin file (required)\n");
    printf("  -o, --outout  <file>       Output file (default: staged.png)\n");
    printf("  -n, --name    <name>       Chunk Name  (default: dBIT)\n");
    printf("  -h, --help                 Show this help message\n");
}

VOID PrintArguments(POPTIONS opts) {
    printf("[i] Input: %s\n", opts->input_file);
    printf("[i] Payload: %s\n", opts->payload_file);
    printf("[i] Output: %s\n", opts->output_file);
    printf("[i] Chunk Name: %s\n\n", opts->chunk_name);
}


VOID InitOptions(POPTIONS opts) {
    memset(opts, 0, sizeof(OPTIONS));
    opts->input_file = NULL;
    opts->payload_file = NULL;
    opts->output_file = "staged.png";
    opts->chunk_name = "dBIT";
    opts->show_help = TRUE;
}

BOOL ParseArguments(INT argc, PCHAR argv[], POPTIONS opts) {
    InitOptions(opts);

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            return FALSE;
        }

        else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--input") == 0) {
            if (i + 1 >= argc) {
                PRINT_ERROR("missing input file");
                return FALSE;
            }
            opts->input_file = argv[++i];
        }

        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--payload") == 0) {
            if (i + 1 >= argc) {
                PRINT_ERROR("missing input file");
                return FALSE;
            }
            opts->payload_file = argv[++i];
        }

        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 >= argc) {
                PRINT_ERROR("missing output file");
                return FALSE;
            }
            opts->output_file = argv[++i];
        }

        else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0) {
            if (i + 1 >= argc) {
                PRINT_ERROR("missing output file");
                return FALSE;
            }
            opts->chunk_name = argv[++i];
        }

        else {
            CHAR err[256];
            sprintf(err, "unknown option: %s", argv[i]);
            PRINT_ERROR(err);
            return FALSE;
        }
    }

    if (opts->input_file == NULL || opts->payload_file == NULL) {
        PRINT_ERROR("missing input or payload file");
        return FALSE;
    }

    opts->show_help = FALSE;
    return TRUE;
}