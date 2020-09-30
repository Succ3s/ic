#ifndef TTN_HEADER_LEX
#define TTN_HEADER_LEX

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
	TK_MOD_SET,     // %=
	TK_EQ,          // ==
	TK_NE,          // !=
	TK_LE,          // <=
	TK_GE,          // >=

	TK_IDENT,
	TK_INT,
	TK_FLOAT,
	TK_STRING,
	TK_CHAR,

	TK_INVALID
};


internal const cstr TOKEN_TO_STRING[] = {
	['_']           = "_",
	[':']           = ":",
	[';']           = ";",
	['.']           = ".",
	[',']           = ",",
	['(']           = "(",
	[')']           = ")",
	['[']           = "[",
	[']']           = "]",
	['{']           = "{",
	['}']           = "}",
	['+']           = "+",
	['*']           = "*",
	['/']           = "/",
	['%']           = "%",
	['=']           = "=",
	['<']           = "<",
	['>']           = ">",
	['!']           = "!",
	[TK_DEREF]      = ".*",
	[TK_REF]        = ".&",
	[TK_IRANGE]     = "..",
	[TK_EXRANGE]    = "..<",
	[TK_ADD_SET]    = "+=",
	[TK_SUB_SET]    = "-=",
	[TK_MUL_SET]    = "*=",
	[TK_DIV_SET]    = "/=",
	[TK_MOD_SET]    = "%=",
	[TK_EQ]         = "==",
	[TK_NE]         = "!=",
	[TK_LE]         = "<=",
	[TK_GE]         = ">=",
	[TK_IDENT]      = "identifier",
	[TK_INT]        = "integer",
	[TK_FLOAT]      = "float",
	[TK_STRING]     = "string",
	[TK_CHAR]       = "char",

	[TK_INVALID]    = "INVALID",
};

cstr parse_number(cstr* inout_stream, i8 base, char* buf, i32 buf_size, i32* inout_len);


typedef struct Token Token;
struct Token {
	u8 kind;
	Pos pos;
	union {
		cstr tinvalid;
		cstr tstr;
		cstr tident;
		f64  tfloat;
		u64  tuint;
		i32  tchar;
	};

};

typedef struct Lexer Lexer;
struct Lexer {
	i32 start;
	cstr begin, cursor;
	i64 line;
	Token last;
	StrIntern intern;
};

#define KEYWORDS \
	o(true) \
	o(false) \
	o(null) \
	o(package) \
	o(import) \
	o(var) \
	o(const) \
	o(as) \
	o(and) \
	o(or) \
	o(orelse) \
	o(proc) \
	o(defer) \
	o(if) \
	o(else) \
	o(for) \
	o(in) \
	o(break) \
	o(continue) \
	o(falltrough) \
	o(type) \
	o(alias) \
	o(struct) \
	o(enum) \
	o(union)

#define o(name) cstr t ## name;
KEYWORDS
#undef o
int init_keywords(Lexer* l);
bool is_keyword(cstr interned);

Lexer lex_init(Source* d);

Token lex_next(Lexer* lp);

#if 0
int test_lexer(int argc, cstr* argv) {
	Sources s = {0};
	isize idx = sources_add(&s, "10 _ 1_0 0x_10 \"Ffo\\too\" 10.4e-2 foobar if foo_b foo3231 'a' 'b '\\t' .. .& + = += != !", "build.ttn");
	Lexer l = lex_init(&s.list[idx]);
	Token tk = lex_next(&l);
	for(;tk.kind != 0;) {
		cstr ss = TOKEN_TO_STRING[tk.kind];
		#define PRINTT "Knd: %10s, Pos: %d"
		switch(tk.kind) {
		// TODO(pgs): print utf8
		case TK_CHAR:
			printf(PRINTT ", Chr: '%c' \n", ss, tk.pos, tk.tchar);
			break;
		case TK_STRING:
			printf(PRINTT ", Str: '%s' \n", ss, tk.pos, tk.tstr);
			break;
		case TK_IDENT:
			printf(PRINTT ", Idn: '%s', Kw: %d \n", ss, tk.pos, tk.tident, is_keyword(tk.tident));
			break;
		case TK_FLOAT:
			printf(PRINTT ", Flt: '%lf'\n", ss, tk.pos, tk.tfloat);
			break;
		case TK_INT:
			printf(PRINTT ", Unt: '%li'\n", ss, tk.pos, tk.tuint);
			break;
		case TK_INVALID:
			printf(PRINTT ", Inv: '%s' \n", ss, tk.pos, tk.tinvalid);
			break;
		default:
			printf(PRINTT "\n", ss, tk.pos);
			break;
		}

		tk = lex_next(&l);
	}
}
#endif


#endif
