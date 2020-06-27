#ifndef DASH_AST_INCLUDE
#define DASH_AST_INCLUDE

#include "common.h"

enum {
#	define AST(U, l, t, D) AstKind ## U ,
#		include "ast.inc"
#	undef AST
	AstKindINVALID
};


#define AST(U, l, t, D) typedef struct t t;
#	include "ast.inc"
#undef AST

#define AST(U, l, t, D) struct t { Location* loc; D;
#define GLOBAL(...) __VA_ARGS__
#	include "ast.inc"
#undef GLOBAL
#undef AST

typedef struct {
	u8 ast_kind;
	union {
#	define AST(U, l, t, D) t l;
#		include "ast.inc"
#	undef AST
	} ast;
} Ast;



void print_size() {

#	define AST(U, l, t, D) printf("sizeof(%s) = %d\n", #t, sizeof(t));
#		include "ast.inc"
#	undef AST

}


#endif
