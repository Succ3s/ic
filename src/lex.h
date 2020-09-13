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

	// TK_TRUE,
	// TK_FALSE,
	// TK_NULL,

	// TK_PACKAGE,
	// TK_IMPORT,

	// TK_VAR,
	// TK_CONST,
	// TK_AS,

	// TK_AND,
	// TK_OR,
	// TK_ORELSE,

	// TK_PROC,
	// TK_DEFER,

	// TK_IF,
	// TK_ELSE,
	// TK_FOR,
	// TK_IN,
	// TK_BREAK,
	// TK_CONTINUE,
	// TK_FALLTROUGH,

	// TK_TYPE,
	// TK_ALIAS,
	// TK_STRUCT,
	// TK_ENUM,
	// TK_UNION,


	TK_INVALID
};


cstr TOKEN_TO_STRING[] = {
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
	// [TK_TRUE]       = "true",
	// [TK_FALSE]      = "false",
	// [TK_NULL]       = "null",
	// [TK_PACKAGE]    = "package",
	// [TK_IMPORT]     = "import",
	// [TK_VAR]        = "var",
	// [TK_CONST]      = "const",
	// [TK_AS]         = "as",
	// [TK_AND]        = "and",
	// [TK_OR]         = "or",
	// [TK_ORELSE]     = "orelse",
	// [TK_PROC]       = "proc",
	// [TK_DEFER]      = "defer",
	// [TK_IF]         = "if",
	// [TK_ELSE]       = "else",
	// [TK_FOR]        = "for",
	// [TK_IN]         = "in",
	// [TK_BREAK]      = "break",
	// [TK_CONTINUE]   = "continue",
	// [TK_FALLTROUGH] = "falltrough",
	// [TK_TYPE]       = "type",
	// [TK_ALIAS]      = "alias",
	// [TK_STRUCT]     = "struct",
	// [TK_ENUM]       = "enum",
	// [TK_UNION]      = "union",

	[TK_INVALID]    = "INVALID",
};

const i8 CHAR_TO_NUM[] = {
	['0'] = 0,  ['1'] = 1,  ['2'] = 2,  ['3'] = 3,
	['4'] = 4,  ['5'] = 5,  ['6'] = 6,  ['7'] = 7,
	['8'] = 8,  ['9'] = 9,  ['a'] = 10, ['b'] = 11,
	['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15,
	['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13,
	['E'] = 14, ['F'] = 15,
};

#define is_number(ch) ((ch) >= '0' & (ch) <= '9')

bool is_hex_number(char ch) {
	return is_number(ch)
	       || (ch >= 'a' & ch <= 'f')
	       || (ch >= 'A' & ch <= 'F');
}

bool is_alphanum(char ch) {
	return is_number(ch)
	       || (ch >= 'a' & ch <= 'z')
	       || (ch >= 'A' & ch <= 'Z');
}

cstr parse_number(cstr* inout_stream, i8 base, char* buf, i32 buf_size, i32* inout_len) {
	#define push(x) (len > buf_size ? printf("BUFFER OVERFLOW\n") : (buf[len++] = (x)))
	#define SAVE *inout_stream = stream; *inout_len = len;

	if(!inout_len && !inout_stream) {
		return "inout_stream and inout_len MUST NOT be null";
	}

	cstr stream = *inout_stream;
	i32 len = *inout_len;

	for(;;) {
		for(;*stream == '_'; stream++) { }

		char ch = *stream;
		if(!is_hex_number(ch) || (CHAR_TO_NUM[ch] > base)) {
			break;
		}

		stream++;

		push(ch);
	}
	push('\0');

	SAVE;
	return null;

	#undef SAVE
	#undef push


}


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

int init_keywords(Lexer* l) {
	// cstr cstr_intern(StrIntern* h, cstr str);
#	define o(name) t ## name = cstr_intern(&l->intern, # name);
	KEYWORDS
#	undef o
	return 0;
}

bool is_keyword(cstr interned) {
	return
#	define o(name) t ## name == interned ||
	KEYWORDS
#	undef o
	false;
}

Lexer lex_init(Source* d) {
	Lexer l = {0};
	l.line = 1;
	l.start = d->start;
	l.begin = l.cursor = d->stream;

	init_keywords(&l);

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
#	define peek1() (*(lp->cursor+1))
#	define next() (*lp->cursor++)
#	define mk_token(Kind) (make_token(lp, init, (Token) { .kind = (Kind) }))
#   define mk_invalid(msg) (make_token(lp, init, (Token) { .kind = TK_INVALID, .tinvalid = (msg) }))
#	define LEX2(Char1, Char2, Tok2) \
		case (Char1): { if(peek() == (Char2)) { next(); return mk_token((Tok2)); } return mk_token((Char1)); }


	/* LABEL */ repeat:;
	lp->cursor += *lp->cursor == ' ';
	cstr init = lp->cursor;
	char nxt = next();
	switch(nxt) {
		case '\r': if(peek() == '\n') { next(); }
		case '\n': lp->line++;
		case ' ': case '\t':  goto repeat;
		case '.': {
			u8 kind = 0;
			char p = peek();
			switch(p) {
				case '*': kind = TK_DEREF; break;
				case '&': kind = TK_REF; break;
				case '.': kind = (peek1() == '<') ? (next(), TK_EXRANGE) : TK_IRANGE; break;
			}
			if(kind != 0) {
				next();
				return mk_token(kind);
			}
			return mk_token('.');
		}


		LEX2('+', '=', TK_ADD_SET);
		LEX2('-', '=', TK_SUB_SET);
		LEX2('*', '=', TK_MUL_SET);
		LEX2('/', '=', TK_DIV_SET);
		LEX2('%', '=', TK_MOD_SET);

		LEX2('=', '=', TK_EQ);
		LEX2('!', '=', TK_NE);
		LEX2('<', '=', TK_LE);
		LEX2('>', '=', TK_GE);

		// TODO(pgs): utf8 support
		case '\'': {
			i32 lit = 0;
			if(peek() == '\\') {
				next();
				switch(next()) {
					case 'n' : lit = (i32)'\n'; break;
					case 'r' : lit = (i32)'\r'; break;
					case 't' : lit = (i32)'\t'; break;
					case '\'': lit = (i32)'\''; break;
					case '0' : lit = (i32)'\0'; break;
					case '\\': lit = (i32)'\\'; break;
					case 'b' : lit = (i32)'\b'; break;
					default: return mk_invalid("Invalid escape character");
				}
 			} else if(peek() != '\'') {
 				lit = (i32)next();
 			} else {
 				return mk_token(TK_INVALID);
 			}
 			if(peek() != '\'') {
 				return mk_invalid("Expecting \' but got something else");
 			}
 			next();

 			Token t = {
				.kind = TK_CHAR,
				.tchar = lit,
			};
			return make_token(lp, init, t);
		}
		case '"': {
			Buf(char) buf = {0};
			char ch;
			for(;(ch = next()) != '"';) {
				if(ch == '\\') {
					switch(next()) {
						case 'n' : buf_push(buf, '\n'); break;
						case 'r' : buf_push(buf, '\r'); break;
						case 't' : buf_push(buf, '\t'); break;
						case '\'': buf_push(buf, '\''); break;
						case '0' : buf_push(buf, '\0'); break;
						case '\\': buf_push(buf, '\\'); break;
						case 'b' : buf_push(buf, '\b'); break;
						default: return mk_invalid("Invalid escape character");
					}
					continue;
				}
				buf_push(buf, ch);
			}
			buf_push(buf, '\0');
			Token t = mk_token(TK_STRING);
			t.tstr = buf;
			return t;
		}

		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		case '8': case '9': {
			bool is_float = false;

			char buf[kilobyte] = {0};
			i32 blen = 0;
			#define push(x) (blen > kilobyte ? printf("BUFFER OVERFLOW\n") : (buf[blen++] = (x)))

			i8 base = 10;

			if(nxt == '0') {
				switch(peek()) {
					case 'x': next(); base = 16; break;
					case 'b': next(); base = 2;  break;
					case 'o': next(); base = 8;  break;
					case 'd': next();            break;
					default: push('0'); break;
				}
			} else {
				push(*(lp->cursor-1));
			}

			parse_number(&lp->cursor, base, buf, kilobyte, &blen);

			if(peek() == '.') {
				blen--; // pop null terminator
				push(next());

				if(is_hex_number(peek())) {
					parse_number(&lp->cursor, base, buf, kilobyte, &blen);
				}

				is_float = true;
			}

			if(peek() == 'E' || peek() == 'e') {
				blen--; // pop null terminator
				push(next());

				if(peek() == '-' || peek() == '+') {
					push(next());
				}

				if(is_hex_number(peek())) {
					parse_number(&lp->cursor, base, buf, kilobyte, &blen);
				}
				is_float = true;
			}


			#undef push


			if(is_float) {
				f64 num = strtod(buf, null);
				Token t = {
					.kind = TK_FLOAT,
					.tfloat = num,
				};
				return make_token(lp, init, t);
			}

			u64 num = strtoul(buf, null, base);
			Token t = {
				.kind = TK_FLOAT,
				.tuint = num,
			};
			return make_token(lp, init, t);
		}


		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
		case 'v': case 'w': case 'x': case 'y': case 'z':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
		case 'V': case 'W': case 'X': case 'Y': case 'Z': {
			Buf(char) buf = {0};
			buf_push(buf, nxt);

			while(is_alphanum(peek()) || peek() == '_') {
				buf_push(buf, next());
			}


			cstr intern = cstr_intern(&lp->intern, buf);
			buf_dealloc(buf);

			Token t = {
				.kind = TK_IDENT,
				.tident = intern,
			};
			return make_token(lp, init, t);
		}
			// TODO(pgs): parse identifiers


		case '_':

		default: return mk_token(nxt);
	}

#	undef peek
#	undef peek1
#	undef next
#	undef mk_invalid
#	undef mk_token
#	undef LEX2
}




int test_lexer(int argc, cstr* argv) {
	Sources s = {0};
	isize idx = sources_add(&s, "10 _ 1_0 0x_10 \"Ffo\\too\" 10.4e-2 foobar if foo_b foo3231 'a' 'b '\\t' .. .& + = += != !", "build.ic");
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
