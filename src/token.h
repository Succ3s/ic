#ifndef DASH_TOKEN_INCLUDE
#define DASH_TOKEN_INCLUDE

#include "common.h"
#include "common.h"
#include <string.h>
#include <stdio.h>

typedef u8 TokenKey;

typedef struct {
	LineColumn begin, end;
	string str;
	TokenKey tok;
} Token;


enum {
#	define Tok(x)      x,
#	define TokKw(x, s) x,
#		include "tokens.inc"
#	undef Tok
#	undef TokKw
TkINVALID = -1,
};



TokenKey getKeywordOrIdent(string s) {

#	define Tok(x) 
#	define TokKw(x, ss) if(strncmp(s.ptr, #ss, sizeof(#ss) - 1) == 0) { return x; } else
#		include "tokens.inc"
#	undef Tok
#	undef TokKw
	return TkLitIdent;

}


cstring tokenToCString(TokenKey k) {

	switch(k) {
#	define Tok(x)       case x: return #x;
#	define TokKw(x, ss) case x: return #x;
#		include "tokens.inc"
#	undef Tok
#	undef TokKw
		default: return "INVALID";
	}
}

#endif
