#include "../../lib/libuseful/src/test_utils.h"
#include "../cbnf.h"
#include "../scanner.h"
#include <string.h>
#include <stdlib.h>

DEFINE_TEST_FUNCTION	{
	while (1)	{
		token t;
		t.type = UNKNOWN;
		memset(t.string, 0, MAXTOKENLEN);
		get_token(in, &t);
		fprintf(out, "%s(%s) ", get_token_name(t.type), t.string);
		
		if(t.type == UNKNOWN)	{
			fprintf(stderr, "Syntax error: unknown token (%s) encountered\n", t.string);
			return 1;
		}
		
		if (t.type == EOT)	{
			break;
		}
	}
	
	fprintf(out,"\n");
	
	return 0;
}

int main(int argc, char** argv)	{
	RUN_TEST;
}