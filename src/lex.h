#ifndef G_HEADER_LEX
#define G_HEADER_LEX

#include "token.h"
#include "common.h"

typedef struct {
	usize line, column;
	cstr str;
	Token curr;
} Lexer;

Lexer lex_build(cstr s);
Token lex_next(Lexer* lex);
char  lex_next_char(Lexer* lex);
char  lex_peek_char(Lexer* lex, usize offset);

#endif
