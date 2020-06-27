#ifndef DASH_LEX_INCLUDE
#define DASH_LEX_INCLUDE

#include "token.h"
#include "common.h"

typedef struct {
	usize line, column;
	cstring str;
	Token curr;
} Lexer;

Lexer lexBuild(cstring s);
// Token lexBuildToken(Lexer* lex, usize TokenKind, usize begin, usize end);

Token lexNext(Lexer* lex);
char lexNextChar(Lexer* lex);
char lexPeekChar(Lexer* lex, usize offset);

#endif
