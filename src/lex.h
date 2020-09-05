#ifndef IC_HEADER_LEX
#define IC_HEADER_LEX

#include "common.h"

enum {
	// : ; . ,
	// ( ) [ ] { }
	// + - * / %
	// = < >
	// !

	TK_DEREF = 128, // .*
	TK_REF,         // .&
	TK_IRANGE,      // ..
	TK_EXRANGE,     // ..<
	TK_ADD_SET,     // +=
	TK_SUB_SET,     // -=
	TK_MUL_SET,     // *=
	TK_DIV_SET,     // /=
	TK_MOD_EQ,      // %=
	TK_EQ,          // ==
	TK_NE,          // !=
	TK_LE,          // <=
	TK_GE,          // >=

	TK_IDENT,
	TK_INT,
	TK_FLOAT,
	TK_STRING,
	TK_CHAR,

	TK_TRUE,
	TK_FALSE,
	TK_NULL,

	TK_PACKAGE,
	TK_IMPORT,

	TK_CONST_IF,
	TK_CONST_FOR,

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
	TK_UNION
};


typedef struct Token Token;
struct Token {
	u8 kind;
	Pos pos;
	union {
		cstr tstr;
		f64  tfloat;
		u64  tuint;
		char tchar;
	};

};

typedef struct Lexer Lexer;
struct Lexer {
	i32 start;
	cstr begin, cursor;
	i64 line;
	Token last;
};

Lexer lex_init(Source* d) {
	Lexer l = {0};
	l.line = 1;
	l.start = d->start;
	l.begin = l.cursor = d->stream;

	// lex_next(&l);

	return l;
}

Pos calc_pos(Lexer* l, cstr c) {
	return l->start + c - l->begin;
}

Token make_token(Lexer* l, cstr tb, Token t) {
	t.pos = calc_pos(l, tb);
	l->last = t; 
	return t;
}

Token lex_next(Lexer* lp) {
	if(!*lp->cursor) { return (Token) { .pos = -1 }; }

#	define peek() (*(lp->cursor))
#	define next() (*lp->cursor++)


	/* LABEL */repeat:;
	lp->cursor += *lp->cursor == ' ';
	cstr init = lp->cursor;
	char nxt = next();
	switch(nxt) {
		case '\n': lp->line++;
		case ' ':  goto repeat;
		case '.': {
			u8 kind = 0;
			char p = peek();
			switch(p) {
				case '*': kind = TK_DEREF; break;
				case '&': kind = TK_REF; break;
				case '.': kind = (next(), peek() == '<') ? TK_EXRANGE : TK_IRANGE; break;
			}
			if(kind != 0) {
				next();
				return make_token(lp, init, (Token) { .kind = kind });
			}
			return make_token(lp, init, (Token) { .kind = '.' });
			break;
		}
	default:
		return make_token(lp, init, (Token) { .kind = nxt });
	}

#	undef peek
#	undef next
}

#endif
