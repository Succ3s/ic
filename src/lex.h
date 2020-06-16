#ifndef DASH_LEX_INCLUDE
#define DASH_LEX_INCLUDE

#include "token.h"
#include "common.h"

typedef struct {
	usize line, column;
	string str;
} Lexer;

Lexer lexBuild(string s);
// Token lexBuildToken(Lexer* lex, usize TokenKind, usize begin, usize end);

Token lexNext(Lexer* lex);
Token lexPeek(Lexer* lex);
char lexNextChar(Lexer* lex);
char lexPreviousChar(Lexer* lex);
char lexPeekChar(Lexer* lex, usize offset);

#endif
