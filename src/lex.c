#include "lex.h"
#include "token.h"
#include <ctype.h>


Lexer lexBuild(string s) {
	return (Lexer) {
		.line = 1, .column = 0,
		.str = s,
	};
}

Token lexBuildToken(Lexer* newLex, Lexer* oldLex, usize TokenKind) {
	string s = oldLex->str;
	s.len = oldLex->str.len - newLex->str.len;
	return (Token) {
		.tok = TokenKind,
		.str = s,
		.begin.line = oldLex->line,
		.begin.column = oldLex->column,
		.end.line = newLex->line,
		.end.column = newLex->column,
	};
}

#define maybeDouble(fst, fstTok, scd, scdTok)       \
case fst:                                           \
	if(lexPeekChar(lex, 0) == scd) {                \
		lexNextChar(lex);                           \
		return lexBuildToken(lex, &oldLex, scdTok); \
	}                                               \
	return lexBuildToken(lex, &oldLex, fstTok);

#define buildToken(tok) lexBuildToken(lex, &oldLex, tok)
#define checkNullTerminate(ch, expr) if((ch) == '\0') { expr; }

bool lexString(Lexer* lex);
Token lexIdent(Lexer* lex, Lexer oldLex);
bool lexNumber(Lexer* lex, char ch1);

Token lexNext(Lexer* lex) {
	Lexer oldLex = *lex;
	char ch;
	switch(ch = lexNextChar(lex)) {
		case ' ' : /* FALLTROUGH */
		case '\n':
		// TODO(pgs): handle tabs properly(change line/column values right)
		case '\t':
		case '\r': return lexNext(lex);

		case ';': return buildToken(TkSemiColon);
		case ':': return buildToken(TkColon);
		// case '.': return buildToken(TkDot);
		case '.':
			switch(lexPeekChar(lex, 0)) {
				case '-': lexNextChar(lex); return buildToken(TkDotMinus);
				case '*': lexNextChar(lex); return buildToken(TkDotStar);
				case '&': lexNextChar(lex); return buildToken(TkDotAmpersand);
				case '!': lexNextChar(lex); return buildToken(TkDotBang);
				case '.':
					lexNextChar(lex);
					if(lexPeekChar(lex, 0) == '<') {
						lexNextChar(lex);
						return buildToken(TkDotDotLArrow);
					}
					return buildToken(TkDotDot);
						
			}
			// if(lexPeekChar(lex, 0) == '.') {
			// 	lexNextChar(lex);
			// 	if(lexPeekChar(lex, 0) == '<') {
			// 		lexNextChar(lex);
			// 		return buildToken(TkDotDotLArrow);
			// 	}
			// 	return buildToken(TkDotDot);
			// }
			return buildToken(TkDot);
		case ',': return buildToken(TkComma);

		case '(': return buildToken(TkOpenParen);
		case '[': return buildToken(TkOpenBracket);
		case '{': return buildToken(TkOpenBrace);

		case ')': return buildToken(TkCloseParen);
		case ']': return buildToken(TkCloseBracket);
		case '}': return buildToken(TkCloseBrace);

		maybeDouble('+', TkPlus,    '=', TkPlusEq);
		maybeDouble('-', TkMinus,   '=', TkMinusEq);
		maybeDouble('*', TkStar,    '=', TkStarEq);
		maybeDouble('/', TkSlash,   '=', TkSlashEq);
		maybeDouble('%', TkPercent, '=', TkPercentEq);

		maybeDouble('=', TkEq,      '=', TkEqEq);
		maybeDouble('!', TkINVALID, '=', TkBangEq);
		maybeDouble('<', TkLArrow,  '=', TkLArrowEq);
		maybeDouble('>', TkRArrow,  '=', TkRArrowEq);

		// string
		case '"': {
			if(lexString(lex)) {
				return buildToken(TkLitString);
			}
			return buildToken(TkINVALID);
		}
		// char
		case '\'': {
			ch = lexNextChar(lex);
			checkNullTerminate(ch, return buildToken(TkINVALID));
			if(ch == '\\') {
				ch = lexNextChar(lex);
				checkNullTerminate(ch, return buildToken(TkINVALID));
			}
			if(lexNextChar(lex) != '\'') return buildToken(TkINVALID);
			return buildToken(TkLitChar);
		}
		default: {
			// number
			if(isdigit(ch)) {
				if(!lexNumber(lex, ch)) { *lex = oldLex; lexNextChar(lex); }
				// TODO(pgs): scientific notation
				if(lexPeekChar(lex, 0) == '.' && isdigit(lexPeekChar(lex, 1))) {
					lexNextChar(lex);
					ch = lexPeekChar(lex, 0);
					while(isdigit(ch) || ch == '_') {
						lexNextChar(lex);
						ch = lexPeekChar(lex, 0);
					}
					return buildToken(TkLitFloat);
				}
				return buildToken(TkLitInteger);
			}
			// identifiers and keywords
			if((isalpha(ch) || ch == '_' || ch == '#')) {
				return lexIdent(lex, oldLex);
			}
			return buildToken(TkINVALID);
		}
	}
}

Token lexIdent(Lexer* lex, Lexer oldLex) {
	char ch = lexPeekChar(lex, 0);
	while(isalnum(ch) || ch == '_') {
		checkNullTerminate(lexNextChar(lex), return buildToken(TkINVALID));
		ch = lexPeekChar(lex, 0);
	}
	Token t = buildToken(TkLitIdent);
	TokenKey key = getKeywordOrIdent(t.str);
	if(key != TkLitIdent) {
		t.tok = key;
	}
	return t;
}

bool lexString(Lexer* lex) {
	for(;lexPeekChar(lex, 0) != '"';) {
		checkNullTerminate(lexNextChar(lex), return false);
		if(lexPeekChar(lex, 0) == '\\') {
			lexNextChar(lex);
			if(lexPeekChar(lex, 0) == '"') {
				lexNextChar(lex);
			}
		}
	}
	lexNextChar(lex);
	return true;
}

#define lexNumberLit(_case, cond, extCond)                      \
_case {                                                         \
	/* TODO(pgs): error */                                      \
	if(ch1 != '0') return false;                                \
	ch = lexNextChar(lex);                                      \
	if(!(cond)) return false;                                   \
	for(;;) {                                                   \
		ch = lexPeekChar(lex, 0);                               \
		if((cond) || (extCond)) { lexNextChar(lex); continue; } \
		return true;                                            \
	}                                                           \
}

bool lexNumber(Lexer* lex, char ch1) {
	char ch;
	switch(ch = lexNextChar(lex)) {
		// bin
		lexNumberLit(case 'b':, ch == '1' || ch == '0', ch == '_')
		// oct
		lexNumberLit(case 'o':, ch >= '0' && ch <= '7', ch == '_')
		// dec
		lexNumberLit(case 'd':, isdigit(ch),            ch == '_')
		// hex
		lexNumberLit(case 'x':, isxdigit(ch),           ch == '_')
		// dec
		default: {
			if(!(isdigit(ch) || ch == '_')) return false;
			for(;;) {
				ch = lexPeekChar(lex, 0);
				if(isdigit(ch) || ch == '_') { lexNextChar(lex); continue; } 
				return true;
			}
		}
	}
	return false;
}
#undef lexNumberLit



Token lexPeek(Lexer* lex) {
	Lexer tmp = *lex;
	Token t = lexNext(lex);
	*lex = tmp;
	return t;
}

char lexNextChar(Lexer* lex) {
	if(lex->str.len <= 0) { return '\0'; }
	char ch = *(lex->str.ptr);
	lex->str.ptr += 1;
	lex->str.len -= 1;
	if(ch == '\n') {
		lex->line  += 1;
		lex->column = 0;
	} else {
		lex->column += 1;
	}
	return ch;
}

char lexPreviousChar(Lexer* lex) {
	return *(lex->str.ptr - 1);
}

char lexPeekChar(Lexer* lex, usize offset) {
	return *(lex->str.ptr + offset);
}

#undef maybeDouble
#undef buildToken
#undef checkNullTerminate
