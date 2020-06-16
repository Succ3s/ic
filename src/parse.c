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

			Token nxt = lexNext(p->lex);

			AstItem* item = allocator_alloc(
				p->allocator,
				sizeof(AstItem) + sizeof(AstImport) + sizeof(Location)
			);
			AstImport* import = (AstImport*) ((u8*)item   + sizeof(AstItem));
			item->item.import = import;
			item->loc         = (Location*)  ((u8*)import + sizeof(AstImport));
			item->itemKind = ItemKindImport;
			import->loc       = item->loc;
			item->loc->begin = fst.begin;

			switch(nxt.tok) {
				case TkLitIdent:
					import->namespace = stringToCString(p->allocator, nxt.str);
					if(lexPeek(p->lex).tok != TkComma) { break; }
					lexNext(p->lex);
					if(lexPeek(p->lex).tok != TkOpenBrace) { break; }
					lexNext(p->lex);
					/* FALLTROUGH */
				case TkOpenBrace:
					for(;lexPeek(p->lex).tok == TkLitIdent;) {
						ChunkedList_push(cstring, 8)(
							&import->global, p->allocator,
							stringToCString(p->allocator, lexNext(p->lex).str)
						);
						if(lexPeek(p->lex).tok != TkComma) {
							break;
						}
					}
					if(lexPeek(p->lex).tok != TkCloseBrace) {
						Token err = lexNext(p->lex);
						makeErrUnexpected(TkCloseBrace, err.tok, (Location) { err.begin, err.end });
						item->loc->end = err.end;
						return item;
					}
					break;
				case TkLitString:
					// nice error
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
					cstring expected0 = tokenToCString(TkLitIdent);
					cstring expected1 = tokenToCString(TkOpenBrace);
					cstring got       = tokenToCString(nxt.tok);
					cstring str      = allocator_alloc(
						p->allocator,
						sizeof_errUnexpected + strlen(expected0) + strlen(expected1) + strlen(got)
					);
					sprintf(str, errUnexpected2, expected0, expected1, got);
					parserPushErr(p, (Error) {
						.lvl = ErrorLevelError,
						.loc = (Location) { nxt.begin, nxt.end },
						.msg = newString(str),
					});

					*import = (AstImport) {0};
					item->loc->end = nxt.end;
					return item;
				}
			}

			Token path = lexNext(p->lex);
			item->loc->end = path.end;

			if(path.tok != TkLitString) {
				makeErrUnexpected(TkLitString, path.tok, (Location) { path.begin, path.end });
				import->path = 0;
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
			AstTypeDecl* tDecl  = (AstTypeDecl*) ((u8*)item + sizeof(AstItem));
			tDecl->isAlias = isAlias;
			item->itemKind      = ItemKindTypeDecl;
			item->item.typeDecl = tDecl;
			item->loc           = (Location*) ((u8*)tDecl + sizeof(AstTypeDecl));
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
			Token semic = lexNext(p->lex);
			if(semic.tok != TkSemiColon) {
				makeErrUnexpected(TkSemiColon, semic.tok, (Location) { semic.begin, semic.end });
				item->loc->end = semic.end;
			}
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
			type->loc           = (Location*) ((u8*)type + sizeof(AstType));
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
			type->loc           = (Location*) ((u8*)type + sizeof(AstType));
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
					type->loc           = (Location*) ((u8*)type + sizeof(AstType));
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
					type->loc  = (Location*) ((u8*)type + sizeof(AstType));
					*type->loc = (Location ) { fst.begin, typ->loc->end };
					return type;
				}
			}
		}
	}
	return null;
}


AstType* parseTypeWithBlock(Parser* p) {
	return null;
}

AstExpr* parseExpr(Parser* p) {
	return null;
}
