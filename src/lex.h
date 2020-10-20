#ifndef TTN_HEADER_LEX
#define TTN_HEADER_LEX

#include "common.h"

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
	o(extern) \
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

enum {
	TK_INVALID = 0,
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

	#define o(name) TK_ ## name ,
	KEYWORDS
	#undef o
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

	#define o(name) [TK_ ## name] = #name,
	KEYWORDS
	#undef o

	[TK_INVALID]    = "INVALID",
};


typedef u8 Token;

typedef struct Lexer Lexer;
struct Lexer {
	i32 start;
	cstr begin, cursor;
	i64 line;
	
	union {
		cstr dinvalid;
		cstr dstr;
		cstr dident;
		f64  dfloat;
		u64  duint;
		i32  dchar;
	} data;
	Pos pos;
	Token tok;


	StrIntern intern;
};

Lexer lex(Source* d);
void advance(Lexer* lp);

#if 0
#define TMAIN test_lexer
int test_lexer(int argc, cstr* argv) {
	Sources s = {0};
	isize idx = sources_add(&s, "10 _ 1_0 0x_10 \"Ffo\\too\" 10.4e-2 foobar if foo_b foo3231 'a' 'b '\\t' .. .& + = += != !", "build.ttn");
	Lexer l = lex(&s.list[idx]);
	advance(&l);
	for(;l.tok != 0;) {
		cstr ss = TOKEN_TO_STRING[l.tok];
		#define PRINTT "Knd: %10s, Pos: %d"
		switch(l.tok) {
		// TODO(pgs): print utf8
		case TK_CHAR:
			printf(PRINTT ", Chr: '%c' \n", ss, l.pos, l.data.dchar);
			break;
		case TK_STRING:
			printf(PRINTT ", Str: '%s' \n", ss, l.pos, l.data.dstr);
			break;
		case TK_IDENT:
			printf(PRINTT ", Idn: '%s'\n", ss, l.pos, l.data.dident);
			break;
		case TK_FLOAT:
			printf(PRINTT ", Flt: '%lf'\n", ss, l.pos, l.data.dfloat);
			break;
		case TK_INT:
			printf(PRINTT ", Unt: '%li'\n", ss, l.pos, l.data.duint);
			break;
		case TK_INVALID:
			printf(PRINTT ", Inv: '%s' \n", ss, l.pos, l.data.dinvalid);
			break;
		default:
			printf(PRINTT "\n", ss, l.pos);
			break;
		}

		advance(&l);
	}
	printf("%d", buf_len(l.intern));
}
#endif


#endif
