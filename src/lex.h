#ifndef DASH_HEADER_LEX
#define DASH_HEADER_LEX

#include "token.h"
#include "common.h"

typedef struct {
	usize line, column;
	cstring str;
	Token curr;
} Lexer;

Lexer lex_build(cstring s);
Token lex_next(Lexer* lex);
char  lex_next_char(Lexer* lex);
char  lex_peek_char(Lexer* lex, usize offset);

#endif
