#include "lex.h"
#include "token.h"
#include <ctype.h>


Lexer lex_build(cstring s) {
	Lexer l = {
		.line = 1, .column = 0,
		.str = s
	};
	lex_next(&l);
	return l;
}

Token _lex_build_token(Lexer* newLex, Lexer* oldLex, usize TokenKind) {
	string s = (string) {
		.ptr = oldLex->str,
		.len = (usize)newLex->str -(usize)oldLex->str
	};
	return (Token) {
		.tok = TokenKind,
		.str = s,
		.begin.line = oldLex->line,
		.begin.column = oldLex->column,
		.end.line = newLex->line,
		.end.column = newLex->column,
	};
}

#define _maybe_double(fst, fstTok, scd, scdTok)        \
case fst:                                             \
    if(lex_peek_char(lex, 0) == scd) {                \
        lex_next_char(lex);                           \
        return _lex_build_token(lex, &oldLex, scdTok); \
    }                                                 \
    return _lex_build_token(lex, &oldLex, fstTok);

#define _build_token(tok)               _lex_build_token(lex, &oldLex, tok)
#define _check_null_terminate(ch, expr) if((ch) == '\0') { expr; }

bool  _lex_string(Lexer* lex);
Token _lex_ident(Lexer* lex, Lexer oldLex);
bool  _lex_number(Lexer* lex, char ch1);
Token _lex_next_w(Lexer* lex);

Token lex_next(Lexer* lex) {
	return lex->curr = _lex_next_w(lex);
}

Token _lex_next_w(Lexer* lex) {
	Lexer oldLex = *lex;
	char ch;
	switch(ch = lex_next_char(lex)) {
		case ' ' : /* FALLTROUGH */
		case '\n':
		// TODO(pgs): handle tabs properly(change line/column values right)
		case '\t':
		case '\r': return lex_next(lex);

		case ';': return _build_token(TkSemiColon);
		case ':': return _build_token(TkColon);
		// case '.': return _build_token(TkDot);
		case '.':
			switch(lex_peek_char(lex, 0)) {
				case '-': lex_next_char(lex); return _build_token(TkDotMinus);
				case '*': lex_next_char(lex); return _build_token(TkDotStar);
				case '&': lex_next_char(lex); return _build_token(TkDotAmpersand);
				case '!': lex_next_char(lex); return _build_token(TkDotBang);
				case '.':
					lex_next_char(lex);
					if(lex_peek_char(lex, 0) == '<') {
						lex_next_char(lex);
						return _build_token(TkDotDotLArrow);
					}
					return _build_token(TkDotDot);
						
			}
			// if(lex_peek_char(lex, 0) == '.') {
			// 	lex_next_char(lex);
			// 	if(lex_peek_char(lex, 0) == '<') {
			// 		lex_next_char(lex);
			// 		return _build_token(TkDotDotLArrow);
			// 	}
			// 	return _build_token(TkDotDot);
			// }
			return _build_token(TkDot);
		case ',': return _build_token(TkComma);

		case '(': return _build_token(TkOpenParen);
		case '[': return _build_token(TkOpenBracket);
		case '{': return _build_token(TkOpenBrace);

		case ')': return _build_token(TkCloseParen);
		case ']': return _build_token(TkCloseBracket);
		case '}': return _build_token(TkCloseBrace);

		_maybe_double('+', TkPlus,    '=', TkPlusEq);
		_maybe_double('-', TkMinus,   '=', TkMinusEq);
		_maybe_double('*', TkStar,    '=', TkStarEq);
		_maybe_double('/', TkSlash,   '=', TkSlashEq);
		_maybe_double('%', TkPercent, '=', TkPercentEq);

		_maybe_double('=', TkEq,      '=', TkEqEq);
		_maybe_double('!', TkINVALID, '=', TkBangEq);
		_maybe_double('<', TkLArrow,  '=', TkLArrowEq);
		_maybe_double('>', TkRArrow,  '=', TkRArrowEq);

		// string
		case '"': {
			if(_lex_string(lex)) {
				return _build_token(TkLitString);
			}
			return _build_token(TkINVALID);
		}
		// char
		case '\'': {
			ch = lex_next_char(lex);
			_check_null_terminate(ch, return _build_token(TkINVALID));
			if(ch == '\\') {
				ch = lex_next_char(lex);
				_check_null_terminate(ch, return _build_token(TkINVALID));
			}
			if(lex_next_char(lex) != '\'') return _build_token(TkINVALID);
			return _build_token(TkLitChar);
		}
		default: {
			// number
			if(isdigit(ch)) {
				if(!_lex_number(lex, ch)) { *lex = oldLex; lex_next_char(lex); }
				// TODO(pgs): scientific notation
				if(lex_peek_char(lex, 0) == '.' && isdigit(lex_peek_char(lex, 1))) {
					lex_next_char(lex);
					ch = lex_peek_char(lex, 0);
					while(isdigit(ch) || ch == '_') {
						lex_next_char(lex);
						ch = lex_peek_char(lex, 0);
					}
					return _build_token(TkLitFloat);
				}
				return _build_token(TkLitInteger);
			}
			// identifiers and keywords
			if((isalpha(ch) || ch == '_' || ch == '#')) {
				return _lex_ident(lex, oldLex);
			}
			return _build_token(TkINVALID);
		}
	}
}

Token _lex_ident(Lexer* lex, Lexer oldLex) {
	char ch = lex_peek_char(lex, 0);
	while(isalnum(ch) || ch == '_') {
		_check_null_terminate(lex_next_char(lex), return _build_token(TkINVALID));
		ch = lex_peek_char(lex, 0);
	}
	Token t = _build_token(TkLitIdent);
	TokenKey key = get_keyword_or_ident(t.str);
	if(key != TkLitIdent) {
		t.tok = key;
	}
	return t;
}

bool _lex_string(Lexer* lex) {
	for(;lex_peek_char(lex, 0) != '"';) {
		_check_null_terminate(lex_next_char(lex), return false);
		if(lex_peek_char(lex, 0) == '\\') {
			lex_next_char(lex);
			if(lex_peek_char(lex, 0) == '"') {
				lex_next_char(lex);
			}
		}
	}
	lex_next_char(lex);
	return true;
}

#define _lex_number_lit(_case, cond, extCond)                     \
_case {                                                           \
	/* TODO(pgs): error */                                        \
	if(ch1 != '0') return false;                                  \
	ch = lex_next_char(lex);                                      \
	if(!(cond)) return false;                                     \
	for(;;) {                                                     \
		ch = lex_peek_char(lex, 0);                               \
		if((cond) || (extCond)) { lex_next_char(lex); continue; } \
		return true;                                              \
	}                                                             \
}

bool _lex_number(Lexer* lex, char ch1) {
	char ch;
	switch(ch = lex_next_char(lex)) {
		// bin
		_lex_number_lit(case 'b':, ch == '1' || ch == '0', ch == '_')
		// oct
		_lex_number_lit(case 'o':, ch >= '0' && ch <= '7', ch == '_')
		// dec
		_lex_number_lit(case 'd':, isdigit(ch),            ch == '_')
		// hex
		_lex_number_lit(case 'x':, isxdigit(ch),           ch == '_')
		// dec
		default: {
			if(!(isdigit(ch) || ch == '_')) return false;
			for(;;) {
				ch = lex_peek_char(lex, 0);
				if(isdigit(ch) || ch == '_') { lex_next_char(lex); continue; } 
				return true;
			}
		}
	}
	return false;
}
#undef _lex_number_lit

char lex_next_char(Lexer* lex) {
	char ch = *(lex->str);
	if(ch == '\0') { return ch; }
	lex->str += 1;
	// lex->line += ch == '\n';
	if(ch == '\n') {
		lex->line  += 1;
		lex->column = 0;
	} else {
		lex->column += 1;
	}
	return ch;
}


// char lex_next_char(Lexer* lex) {
// 	if(lex->str.len <= 0) { return '\0'; }
// 	char ch = *(lex->str.ptr);
// 	lex->str.ptr += 1;
// 	lex->str.len -= 1;
// 	lex->line += ch == '\n';
// 	return ch;
// }

// char lex_next_char(Lexer* lex) {
// 	char ch = *(lex->str);
// 	if(ch == '\0') return ch;
// 	lex->str += 1;
// 	lex->line += ch == '\n';
// 	return ch;
// }

char lex_peek_char(Lexer* lex, usize offset) {
	return *(lex->str + offset);
}

#undef _maybe_double
#undef _build_token
#undef _check_null_terminate
