#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>

#define MAXTOKENLEN	512				// TODO: Ultimately this should be dynamic
									//		there really shouldn't be any limit
									//		on token size, plus this is a buffer
									//		overrun just waiting to happen


typedef struct token_info_tag	{
	int type;						// Tokens types are defined externally
	const char* name;
	const char* literal;
} token_info;

typedef struct token_tag	{
	int type;
	int source_line;
	int source_column;
	char string[MAXTOKENLEN];
} token;

extern token_info tokens[];			// Define this where you define your tokens

void get_token(FILE* fp, token* tok);
const char* get_token_name(int tok);

#endif // SCANNER_H