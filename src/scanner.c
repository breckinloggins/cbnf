#include "cbnf.h"
#include "scanner.h"
#include <string.h>
#include <stdlib.h>


typedef enum scan_state_tag	{
	IN_WHITESPACE,
	IN_NON_WHITESPACE,
	IN_COMMENT,
	IN_COMMENT_BLOCK,
	IN_BAREWORD,
	IN_SYMBOL,
	IN_QUOTE,
} scan_state;

#define EQUALS(s1, s2) (!strcmp((s1), (s2)))
/**
 * Given a string with one or more characters, returns the token_type
 *
 * char string[] - The string to classify, of at most MAXTOKENLEN chars
 *
 * NOTE: We use an argument here instead of a static global variable because
 *       this scanner MUST be thread-safe
 */
static token_type classify_string(const char string[MAXTOKENLEN])	{
	int i;
	
	if (string[0] == '\0') return UNKNOWN;	/* Empty token */
	
	i = 0;
	while (1)	{
		if (tokens[i].type == EOT)	{
			break;
		}
		
		if (tokens[i].literal && EQUALS(string, tokens[i].literal))	{
			return tokens[i].type;
		}
		
		++i;
	}
		
	return UNKNOWN;	// We don't return "ID" here because that classification depends on
					// the context in which this function was called classify_string
}

static int current_line = 0;
static int current_column = 0;

#define APPEND(c) *p++=c
#define RESET p=tok->string
#define IS_WS(c) (c==' ' || c=='\t' || c=='\r' || c=='\n')
#define ISALNUM(c) (c=='_' || isalnum(c))
#define READCHAR ch=fgetc(fp)



/**
 * Gets the next character, but then puts it back
 */
static int fpeekc(FILE* fp)	{
	int ch;
	ch = fgetc(fp);
	ungetc(ch, fp);
	
	return ch;
}

/**
 * Scans the input stream fp and fills in the given token with information about the first token
 * recognized.
 *
 * NOTE: Tokens received will include COMMENT and COMMENT_BLOCK tokens (because many parsers wish
 * 			to retain comment information downstream).  It is the caller's responsibility to throw
 *			out these tokens!
 */
void get_token(FILE* fp, token* tok)	{
	char* p;
	int ch, quotechar;
	int state;
	
	RESET;
	READCHAR;
	
	/* Determine initial state */
	if (IS_WS(ch))	{
		state = IN_WHITESPACE;
	} else if (ch != EOF) {
		state = IN_NON_WHITESPACE;
	} else {
		tok->type = EOT;
		tok->string[0] = '\0';
		return;
	}
	
	while(1)	{
		switch(state)	{
			//
			// Handle chunks of whitespace
			//
			case IN_WHITESPACE:
				READCHAR;
				if (IS_WS(ch))	{
					state = IN_WHITESPACE;
				} else if (ch == EOF)	{
					tok->type = EOT;
					tok->string[0] = '\0';
					return;
				} else {
					state = IN_NON_WHITESPACE;
				}
				break;
			//
			// Handle all other non-whitespace and determine what to do
			//
			case IN_NON_WHITESPACE:
				APPEND(ch);
				if(ch == '\'' || ch=='"')	{
					quotechar = ch;
					state = IN_QUOTE;
				} else if (ISALNUM(ch))	{
					state = IN_BAREWORD;
				} else {
					state = IN_SYMBOL;
				}
				break;
			//
			// Handle Quotes
			//
			case IN_QUOTE:
				READCHAR;
				if (ch == '\\')	{
					// This is going to be an escape of somekind, so just get
					// the other character
					APPEND(ch);
					READCHAR;
					APPEND(ch);
					state = IN_QUOTE;	// Just being explicit
				} else if (ch == quotechar)	{
					// We've now balanced the quote
					APPEND(ch);
					*p = '\0';
					tok->type = STRING;
					return;	// DONE!
				} else if (ch == EOF)	{
					fprintf(stderr, "Syntax Error: Unexpected End of File\n");
					exit(1);
				} else {
					APPEND(ch);
					state = IN_QUOTE;
				}
				break;
			//
			// Handle Bare Words (could be a keyword, identifier, etc.)
			//
			case IN_BAREWORD:
				READCHAR;
				if (ISALNUM(ch))	{
					APPEND(ch);
					state = IN_BAREWORD;
				} else {
					// Rewind to let the next iteration catch whatever this is
					if (ch != EOF)	{
						ungetc(ch, fp);
					}
					
					// We're at the end of the BAREWORD
					*p = '\0';
					tok->type = classify_string(tok->string);
					
					if (tok->type == UNKNOWN)	{
						tok->type = ID;		
					}
					
					return ;			// DONE!
				}
				break;
			//
			// Handle Symbols of all kinds 
			//
			case IN_SYMBOL:
				if (ch == '/')	{
					// We could be starting a comment
					READCHAR;
					if (ch == '*')	{
						RESET;
						state = IN_COMMENT_BLOCK;
						continue;
					} else if (ch == '/')	{
						RESET;
						state = IN_COMMENT;
						continue;
					} else {
						ungetc(ch, fp);
					}
				} 
				
				*p = '\0';
				tok->type = classify_string(tok->string);
				return;
				break;
			//
			// Handle Comments (terminated by Endline)
			//
			case IN_COMMENT:
				READCHAR;
				if (ch == '\n' || ch == EOF)	{
					*p = '\0';
					tok->type = COMMENT;
									
					return;	// DONE
				} else {
					APPEND(ch);
					state = IN_COMMENT;
				}
				break;
			// 
			// Handle Comment Blocks (terminated by comment endsymbol)
			//
			case IN_COMMENT_BLOCK:
				READCHAR;
				if (ch == '*')	{
					READCHAR;
					if (ch == '/')	{
						*p = '\0';
						tok->type = COMMENT_BLOCK;
						return;	// DONE
					} else {
						APPEND('*');	// It was just a literal asterisk			
					}
				}
				
				if (ch == EOF)	{
					fprintf(stderr, "Syntax Error: Unexpected End of File in middle of block comment\n");
					exit(1);
				}
				
				APPEND(ch);
				state = IN_COMMENT_BLOCK;
				break;
		}
	}
}



/**
 * Given a token type, returns the name of the token ready for printing 
 */
const char* get_token_name(int tok)	{
	return tokens[(int)tok].name;
}