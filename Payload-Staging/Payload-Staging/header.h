#ifndef HEADER_H
#define HEADER_H

#include <Windows.h>
#include <stdio.h>

#include "argparser.h"
#include "utils.h"

typedef struct {
	char* input_file;
	char* output_file;
	char* algorithm;
	int show_help;
} Options;

static const BYTE PNG_SIGNATURE[8] =
{
	0x89,0x50,0x4E,0x47,
	0x0D,0x0A,0x1A,0x0A
};

#endif // !HEADER_H