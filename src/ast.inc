#ifndef GLOBAL
#	define GLOBAL(...)
#   define ENDGLOBAL
#endif

GLOBAL(typedef AstItem* AstItemPtr;)
GLOBAL(ChunkedListImpl(AstItemPtr, 8))
AST(File, file, AstFile,
	AstPackage* package;
	ChunkedList(AstItemPtr, 8) items;
})

AST(Package, package, AstPackage,
	cstring name;
})

// ===== ITEM

GLOBAL(enum {
	ItemKindImport = 1,
	ItemKindTypeDecl,
	ItemKindProc,
	ItemKindImpl,
	ItemKindDecl
};)
AST(Item, item, AstItem,
	u8 itemKind;
	union {
		AstImport*   import;
		AstTypeDecl* typeDecl;
		AstProc*     proc;
		AstImpl*     Impl;
		AstDecl*     decl;
	} item;
})

GLOBAL(ChunkedListImpl(cstring, 8))
AST(Import, import, AstImport,
	cstring namespace;
	cstring path;
})

AST(TypeDecl, typeDecl, AstTypeDecl,
	bool     isAlias;
	cstring   newName;
	AstType* type;
})

GLOBAL(typedef struct {
	cstring   name;
	AstType* type;
} ProcArguments;)
GLOBAL(ChunkedListImpl(ProcArguments, 4))
AST(Proc, proc, AstProc,
	cstring                         name;
	ChunkedList(ProcArguments, 4)  arguments;
	AstType*                       _return;
	AstBlock*                      block; // opt
})

// GLOBAL(typedef AstProc* AstProcPtr;)
GLOBAL(typedef AstType* AstTypePtr;)
GLOBAL(ChunkedListImpl(AstTypePtr, 2))
GLOBAL(ChunkedListImpl(AstProc, 8))
AST(Impl, impl, AstImpl,
	cstring                    name;
	ChunkedList(AstTypePtr, 2) implements;
	ChunkedList(AstProc, 8)    implementations;
})

AST(Decl, decl, AstDecl,
	bool     isConst;
	cstring   name;
	AstType* type;
	AstExpr* expr;
})






// ===== TYPE

GLOBAL(enum {
	TypeKindStruct = 1,
	TypeKindUnion,
	TypeKindEnum,
	TypeKindProcType,
	TypeKindTrait,
	TypeKindNamed,
	TypeKindPointer,
	TypeKindSlice,
	TypeKindArray,
	TypeKindImplList,
};)
GLOBAL(typedef struct { AstExpr* inner; AstType* type; } TypeArray;)
AST(Type, type, AstType,
	u8 typeKind;
	union {
		AstStruct*                 _struct;
		AstUnion*                  _union;
		AstEnum*                   _enum;
		AstTrait*                  trait;
		AstProcType*               procT;
		cstring                    named;
		AstType*                   ptr;
		AstType*                   slice;
		TypeArray                  array;
	} type;
})


GLOBAL(typedef AstExpr* AstExprPtr;)
// GLOBAL(ChunkedListImpl(AstExprPtr, 8))
AST(Struct, _struct, AstStruct,
	ChunkedList(ProcArguments, 4) fields;
})

GLOBAL(ChunkedListImpl(AstType, 4))
AST(Union, _union, AstUnion,
	ChunkedList(AstType, 4) types;
})


GLOBAL(typedef struct {
	cstring field;
	AstExpr* expr;
} EnumKey;)
GLOBAL(ChunkedListImpl(EnumKey, 8))
AST(Enum, _enum, AstEnum,
	ChunkedList(EnumKey, 8) fields;
})

AST(ProcType, procType, AstProcType,
	ChunkedList(ProcArguments, 4) arguments;
	AstType*                       _return;
})


GLOBAL(ChunkedListImpl(AstProcType, 8))
AST(Trait, trait, AstTrait,
	ChunkedList(AstProc, 8) procs;
})







// ===== Stmt

GLOBAL(enum {
	stmtKindItem = 1,
	stmtKindBlock,
	stmtKindFor,
	stmtKindExpr,
	stmtKindReturn,
	stmtKindBreak,
	stmtKindContinue,
};)
AST(Stmt, stmt, AstStmt,
	u8 stmtKind;
	union {
		AstItem   item;
		// 'for' expr? BLOCK
		// 'for' ident,* 'in' expr BLOCK
		AstFor*   _for;
		AstExpr*  expr; // exprWithBlock | exprWithoutBlock ';'
		AstExpr*  _return;
		bool breakOrContinue; // 1 = break, 0 = continue
	} stmt;
})

GLOBAL(ChunkedListImpl(cstring, 4))
AST(For, _for, AstFor,
	ChunkedList(cstring, 4) captureList; // opt
	AstExpr*               expr;
	AstBlock*              block;
})







// ===== Block

// 'do' stmt | '{' stmt* '}'
GLOBAL(ChunkedListImpl(AstStmt, 8))
AST(Block, block, AstBlock,
	ChunkedList(AstStmt, 8) stmts;
})






// ===== Expr

AST(Expr, expr, AstExpr,
	u8 exprKind;
	union {
		AstBlock                 block; // '{' stmt* '}'
		AstIf*                   _if;
		AstSwitch*               _switch;
		AstImpl*                 impl;
		AstInfix*                infix;
		// TODO
		AstPostfix*              postfix;
		AstLiteral*              literal;
	} expr;
})

// if := 'if'
GLOBAL(typedef struct { AstExpr* cond; AstBlock then; } IfData;)
GLOBAL(ChunkedListImpl(IfData, 4))
AST(If, _if, AstIf,
	IfData data;
	ChunkedList(IfData, 4) elseIf;
	AstBlock _else;
})

GLOBAL(typedef struct { cstring cap; AstType t; AstExpr* cond; } TypeCase;)
GLOBAL(typedef struct { AstExpr _case; AstExpr* cond; } SimpleCase;)
GLOBAL(ChunkedListImpl(SimpleCase, 2))
GLOBAL(enum {
	BranchKindElse = 1,
	BranchKindType,
	BranchKindSimple,
};)
GLOBAL(typedef struct {
	u8 branchKind;
	union {
		TypeCase                   type;
		ChunkedList(SimpleCase, 2) simple;
	} branch;
	AstBlock* block;
} SwitchBranch;)
GLOBAL(ChunkedListImpl(SwitchBranch, 4))
AST(Switch, _switch, AstSwitch,
	AstExpr* expr;
	ChunkedList(SwitchBranch, 4) branchs;
})

GLOBAL(enum {
	InfixAdd = 1,  // +
	InfixSub,      // -
	InfixMul,      // *
	InfixDiv,      // /
	InfixMod,      // %


	InfixAssign,    // =
	InfixAddAssign, // +=
	InfixSubAssign, // -=
	InfixMulAssign, // *=
	InfixDivAssign, // /=
	InfixModAssign, // %=

	InfixGt,       // >
	InfixLt,       // <
	InfixEq,       // ==
	InfixNe,       // !=
	InfixGe,       // >=
	InfixLe,       // <=

	InfixAnd,      // or
	InfixOr,       // and

	InfixRange,    // ..
	InfixExRange,  // ..<
	InfixOrElse,   // orelse
	InfixAs,       // as
};)
AST(Infix, infix, AstInfix,
	AstExpr* right;
	union {
		AstExpr* left;
		AstType* leftT;
	};
	u8 op;
})

GLOBAL(enum {
	PostfixNegation = 1, // .-
	PostfixRef,          // .&
	PostfixDeref,        // .*
	PostfixNot,          // .!
	PostfixCall,         // '(' expr,* ')'
	PostfixIndex         // '[' expr ']'
};)
GLOBAL(ChunkedListImpl(AstExpr, 4))
AST(Postfix, postfix, AstPostfix,
	AstExpr* begin;
	u8 op;
	union {
		ChunkedList(AstExpr, 4) call;
		AstExpr*                index;
	} ope;
})

GLOBAL(enum {
	LiteralKindUint = 1,
	LiteralKindInt,
	LiteralKindFloat,
	LiteralKindIdent,
	LiteralKindStr,
	LiteralKindChar,
	LiteralKindGroup
};)
AST(Literal, literal, AstLiteral,
	u8 litKind;
	union {
		u64      uint;
		i64      _int;
		f64      _float;
		cstring   ident;
		cstring   str;
		char     ch;
		AstExpr* group;
	} lit;
})



#ifdef ENDGLOBAL
#	undef GLOBAL
#	undef ENDGLOBAL
#endif
