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
	TK_LE,          // <=
	TK_GE,          // >=
	TK_BANG_EQ,     // !=

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

typedef i32 Pos;

typedef struct Src Src;
struct Src {
	cstr path;

	cstr stream;

	i32  size;
	i32  start;
	i32  eol;
};

typedef struct Srcs Srcs;
struct Srcs {
	Buf(Src) list;
	Pos end;
};

isize srcs_add(Srcs* srcs, cstr stream, cstr path) {
	isize len = cstr_len(stream);
	cstr dup = cstr_clone(stream);
	Src src = (Src) {
		.path = path,
		.stream = dup,
		.size = len,
		.start = srcs->end
	};
	srcs->end += src.size;

	buf_push(srcs->list, src);
	return buf_len(srcs->list)-1;
}

isize srcs_find(Srcs* srcs, Pos pos) {
	isize blen = buf_len(srcs->list);
	for(isize i = 0; i < blen; i++) {
		i32 start = srcs->list[i].start;
		i32 end = start + srcs->list[i].size;

		if(pos >= start && pos < end) {
			return i;
		}
	}
	return -1;
}

typedef struct Token Token;
struct Token {
	u8 kind;
	u32 pos;
	union {
		cstr tstr;
		f64  tfloat;
		u64  tuint;
		char tchar;
	};

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

	// lex_next_char(&l);

	return l;
}

char lex_next_char(Lexer* lexer) {
	char nxt = *(++lexer->data);
	lexer->line += nxt == '\n';
	return nxt;
}

#endif
