#include "lex.h"



#define is_number(ch) ((ch) >= '0' & (ch) <= '9')
#define is_hex_number(ch) (is_number((ch)) || (ch) >= 'a' & (ch) <= 'f' || (ch) >= 'A' & (ch) <= 'F')
#define is_alphanum(ch)   (is_number((ch)) || (ch) >= 'a' & (ch) <= 'z' || (ch) >= 'A' & (ch) <= 'Z')

internal const i8 CHAR_TO_NUM[] = {
	['0'] = 0,  ['1'] = 1,  ['2'] = 2,  ['3'] = 3,
	['4'] = 4,  ['5'] = 5,  ['6'] = 6,  ['7'] = 7,
	['8'] = 8,  ['9'] = 9,  ['a'] = 10, ['b'] = 11,
	['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15,
	['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13,
	['E'] = 14, ['F'] = 15,
};

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


// X-macros ahead

int init_keywords(Lexer* l) {
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
	l.start = d->base;
	l.begin = l.cursor = d->stream;

	init_keywords(&l);

	return l;
}


internal Pos calc_pos(Lexer* l, cstr c) {
	return l->start + c - l->begin;
}

internal Token make_token(Lexer* l, cstr tb, Token t) {
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
#	define mk_invalid(msg) (make_token(lp, init,
		(Token) { .kind = TK_INVALID, .tinvalid = (msg) }))
#	define LEX2(Char1, Char2, Tok2) \
		case (Char1): { \
			if(peek() == (Char2)) { \
				next(); \
				return mk_token((Tok2)); \
			} \
			return mk_token((Char1)); \
		}


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
			default  : return mk_invalid("Invalid escape character");
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
				default  : return mk_invalid("Invalid escape character");
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
			default : push('0');         break;
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



#undef is_number
#undef is_hex_number
#undef is_alphanum
