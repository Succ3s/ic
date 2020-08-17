#ifndef G_HEADER_LEX
#define G_HEADER_LEX

#include "common.h"


typedef enum tokenKind tokenKind;
enum TokenKind {
	// : ; . ,
	// ( ) [ ] { }
	// + - * / %
	// = < >

	TK_DOT_MINUS = 128, // .-
	TK_DOT_STAR,        // .*
	TK_DOT_AND,         // .&
	TK_DOT_BANG,        // .!
	TK_DOT_DOT,         // ..
	TK_DOT_DOT_LARROW,  // ..<
	TK_PLUS_EQ,         // +=
	TK_MINUS_EQ,        // -=
	TK_SLASH_EQ,        // /=
	TK_STAR_EQ,         // *=
	TK_PERCENT_EQ,      // %=
	TK_EQ_EQ,           // ==
	TK_LARROW_EQ,       // <=
	TK_RARROW_EQ,       // >=
	TK_BANG_EQ,         // !=

	TK_IDENT,
	TK_INT,
	TK_FLOAT,
	TK_STRING,
	TK_CHAR,

	TK_TRUE,
	TK_FALSE,
	TK_NULL,

	TK_HASH_PACKAGE,
	TK_HASH_IMPORT,
	TK_HASH_IF,

	TK_VAR,
	TK_CONST,
	TK_AS,

	TK_AND,
	TK_OR,
	TK_ORELSE,

	TK_PROC,
	TK_DEFER,

	TK_IF,
	TK_ELSE,
	TK_FOR,
	TK_IN,
	TK_BREAK,
	TK_CONTINUE,
	TK_FALLTROUGH,

	TK_TYPE,
	TK_ALIAS,
	TK_STRUCT,
	TK_ENUM,
	TK_UNION,
	TK_TRAIT,
	TK_IMPL
};


typedef struct Token Token;
struct Token {
	TokenKind kind;
	i64 line;
	char* offset;
	patientid id;
};

typedef struct Lexer Lexer;
struct Lexer {
	cstr data;
	i64 line;
	Token last;
};

Lexer lex_init(cstr d) {
	Lexer l = (Lexer) {
		.data = d,
		.line = 1,
	};

	lex_next_char(&l);

	return l;
}

char lex_next_char(Lexer* lexer) {
	char nxt = *(++lexer->data);
	lexer->line += nxt == '\n';
	return nxt;
}

#endif
