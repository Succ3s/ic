#include <string.h>

#define errMessage(var, msg)              \
const char* var = msg;                    \
const usize sizeof_ ## var = sizeof(msg);

errMessage(errUnexpected, "Unexpected token: expected %s, but got %s")
errMessage(errUnexpected2, "Unexpected tokens: expected %s, or %s, but got %s")
 
#define makeErrUnexpected(EXPECTED, GOT, ...)             \
cstring expected = tokenToCString(EXPECTED);              \
cstring got      = tokenToCString(GOT);                   \
cstring str      = allocator_alloc(                       \
	p->allocator,                                         \
	sizeof_errUnexpected + strlen(expected) + strlen(got) \
);                                                        \
sprintf(str, errUnexpected, expected, got);               \
parserPushErr(p, (Error) {                                \
	.lvl = ErrorLevelError,                               \
	.loc = __VA_ARGS__,                                   \
	.msg = newString(str),                                \
});

AstPackage* parsePackage(Parser* p);
AstItem*    parseItem(Parser* p);
AstType*    parseType(Parser* p);
AstType*    parseTypeWithoutBlock(Parser* p);
AstType*    parseTypeWithBlock(Parser* p);
AstExpr*    parseExpr(Parser* p);
ChunkedList(ProcArguments, 4) parseArguments(Parser* p);

AstFile* parse(Parser* p) {
	AstFile* a = allocator_alloc(p->allocator, sizeof(AstFile));
	a->package = parsePackage(p);
	a->items = (ChunkedList(AstItemPtr, 8)) { 0 };
	AstItem* item;
	for(;(item = parseItem(p)) != null;) {
		 ChunkedList_push(AstItemPtr, 8)(&a->items, p->allocator, item);
	}
	return a;
}

AstPackage* parsePackage(Parser* p) {
	// '#package' ident
	Token package = lexNext(p->lex);
	if(package.tok != TkKwHashPackage) {
		p->flags |= PARSE_NOPACKAGE;
		return null;
	}
	LineColumn begin = package.begin;

	Token pkgName = lexNext(p->lex);
	LineColumn end = pkgName.end;

	AstPackage* pkg = allocator_alloc(
		p->allocator,
		sizeof(AstPackage) + sizeof(Location)
	);
	pkg->loc = (Location*)((u8*)pkg + sizeof(AstPackage));
	*pkg->loc = (Location) { begin, end };

	if(pkgName.tok != TkLitIdent) {
		makeErrUnexpected(TkLitIdent, pkgName.tok, (Location) { pkgName.begin, pkgName.end });
		pkg->name = null;
		return pkg;
	}

	cstring cstr = stringToCString(p->allocator, pkgName.str);
	pkg->name = cstr;
	return pkg;
}


AstItem* parseItem(Parser* p) {
	Token fst = lexPeek(p->lex);
	bool isAlias = false; // line 161
	switch(fst.tok) {
		// #import
		case TkKwHashImport: {
			lexNext(p->lex);


			usize allocationSize = sizeof(AstItem) + sizeof(AstImport) + sizeof(Location);
			AstItem* item = allocator_alloc(p->allocator, allocationSize);
			memset(item, 0, allocationSize);


			AstImport* import = (AstImport*) (item + 1);
			item->item.import = import;

			item->itemKind = ItemKindImport;
			// NOTE(pgs): redundant Locations
			item->loc         = (Location*)  (import + 1);
			import->loc       = item->loc;
			item->loc->begin = fst.begin;

			Token nxt = lexNext(p->lex);
			switch(nxt.tok) {
				case TkLitIdent:
					import->namespace = stringToCString(p->allocator, nxt.str);
					break;
				case TkLitString: // nice error

					parserPushErr(p, (Error) {
						.lvl = ErrorLevelError,
						.loc = (Location) { nxt.begin, nxt.end },
						.msg = newString("Missing import name"),
					});
					parserPushErr(p, (Error) {
						.lvl = ErrorLevelNote,
						.hideInfo = true,
						.msg = newString("usage: #import name \"path\""),
					});


					item->loc->end = nxt.end;
					return item;
				default: {
					makeErrUnexpected(TkLitIdent, nxt.tok, (Location) { nxt.begin, nxt.end });
					item->loc->end = nxt.end;
					return item;
				}
			}

			Token path = lexNext(p->lex);
			item->loc->end = path.end;

			if(path.tok != TkLitString) {
				makeErrUnexpected(TkLitString, path.tok, (Location) { path.begin, path.end });

				return item;
			}

			cstring path1 = stringToCString(p->allocator, path.str);
			import->path = path1;
			return item;
		}



		// alias & type
		case TkKwAlias:
			isAlias = true;
		case TkKwType: {
			lexNext(p->lex);

			AstItem* item = allocator_alloc(
				p->allocator,
				sizeof(AstItem) + sizeof(AstTypeDecl) + sizeof(Location)
			);
			AstTypeDecl* tDecl  = (AstTypeDecl*) (item + 1);
			tDecl->isAlias = isAlias;
			item->itemKind      = ItemKindTypeDecl;
			item->item.typeDecl = tDecl;
			item->loc           = (Location*) (tDecl + 1);
			tDecl->loc          = item->loc;
			item->loc->begin    = fst.begin;


			// ident
			Token name = lexNext(p->lex);
			if(name.tok != TkLitIdent) {
				makeErrUnexpected(TkLitIdent, name.tok, (Location) { name.begin, name.end });
				item->loc->end = name.end;
				return item;
			}
			tDecl->newName = stringToCString(p->allocator, name.str);

			// type
			AstType* t = parseTypeWithoutBlock(p);
			if(t == null) {
				t = parseTypeWithBlock(p);
				if(t == null) {
					// TODO(pgs): handle error
					abort();
				}
				goto nosemicolon;
			}
			Token semic = lexPeek(p->lex);
			if(semic.tok != TkSemiColon) {
				makeErrUnexpected(TkSemiColon, semic.tok, (Location) { semic.begin, semic.end });
				item->loc->end = semic.end;
				return item;
			}
			lexNext(p->lex);
			nosemicolon:

			tDecl->type = t;
			item->loc->end = t->loc->end;
			return item;
		}



		default: return null; 
	}
}


AstType* parseType(Parser* p) {
	AstType* t = parseTypeWithoutBlock(p);
	if(t == null) {
		t = parseTypeWithBlock(p);
	}
	return t;
}


// TODO(pgs): proc type
AstType* parseTypeWithoutBlock(Parser* p) {
	Token fst = lexPeek(p->lex);
	switch(fst.tok) {
		case TkLitIdent: {
			lexNext(p->lex);

			AstType* type = allocator_alloc(
				p->allocator,
				sizeof(AstType) + sizeof(Location)
			);
			type->typeKind      = TypeKindNamed;
			type->type.named    = stringToCString(p->allocator, fst.str);
			type->loc           = (Location*) (type + 1);
			*type->loc          = (Location ) { fst.begin, fst.end };
			return type;
		}
		case TkStar: {
			lexNext(p->lex);

			AstType* typ = parseType(p);
			if(typ == null) {
				// TODO(pgs): handle error
				return null;
			}

			AstType* type = allocator_alloc(
				p->allocator,
				sizeof(AstType) + sizeof(Location)
			);
			type->typeKind      = TypeKindPointer;
			type->type.ptr      = typ;
			type->loc           = (Location*) (type + 1);
			*type->loc          = (Location ) { fst.begin, typ->loc->end };
			return type;
		}

		case TkOpenBracket: {
			lexNext(p->lex);
			switch(lexPeek(p->lex).tok) {
				case TkCloseBracket: {
					lexNext(p->lex);

					AstType* typ = parseType(p);
					if(typ == null) {
						// TODO(pgs): handle error
						return null;
					}

					AstType* type = allocator_alloc(
						p->allocator,
						sizeof(AstType) + sizeof(Location)
					);
					type->typeKind      = TypeKindSlice;
					type->type.slice    = typ;
					type->loc           = (Location*) (type + 1);
					*type->loc          = (Location ) { fst.begin, typ->loc->end };
					return type;
				}
				default: {
					AstExpr* expr = parseExpr(p);
					if(expr == null) {
						// TODO(pgs): handle error
						return null;
					}
					if(lexNext(p->lex).tok != TkCloseBracket) {
						// TODO(pgs): handle error
						return null;
					}

					AstType* typ = parseType(p);
					if(typ == null) {
						// TODO(pgs): handle error
						return null;
					}

					AstType* type = allocator_alloc(
						p->allocator,
						sizeof(AstType) + sizeof(Location)
					);
					type->typeKind      = TypeKindArray;
					type->type.array    = (TypeArray) {
						.inner = expr,
						.type  = typ,
					};
					type->loc  = (Location*) (type + 1);
					*type->loc = (Location ) { fst.begin, typ->loc->end };
					return type;
				}
			}
		}
	}
	return null;
}


AstType* parseTypeWithBlock(Parser* p) {
	Token fst = lexPeek(p->lex);
	switch(fst.tok) {
		case TkKwStruct: {
			lexNext(p->lex);

			usize allocSize = sizeof(AstType) + sizeof(AstStruct) + sizeof(Location);
			AstType* type = allocator_alloc(p->allocator, allocSize);
			memset(type, 0, allocSize);

			AstStruct* strct   = (AstStruct*) (type + 1);
			type->typeKind     = TypeKindStruct;
			type->type._struct = strct;

			type->loc  = (Location*) (strct + 1);
			strct->loc = type->loc;

			type->loc->begin = fst.begin;



			Token tmp = lexPeek(p->lex);
			if(tmp.tok != TkOpenBrace) {
				makeErrUnexpected(TkOpenBrace, tmp.tok, (Location) { tmp.begin, tmp.end });
				type->loc->end = tmp.end;
				return type;
			}
			lexNext(p->lex);



			strct->fields = parseArguments(p);


			tmp = lexPeek(p->lex);
			type->loc->end = tmp.end;
			if(lexPeek(p->lex).tok != TkCloseBrace) {
				makeErrUnexpected(TkCloseBrace, tmp.tok, (Location) { tmp.begin, tmp.end });
				return type;
			}
			lexNext(p->lex);

			return type;
		}
		default: return null;
	}
}

AstExpr* parseExpr(Parser* p) {
	return null;
}


ChunkedList(ProcArguments, 4) parseArguments(Parser* p) {
	return (ChunkedList(ProcArguments, 4)) {0};
}
