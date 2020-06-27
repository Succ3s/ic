#include <string.h>

#define ERR_MESSAGE(var, msg)             \
const char* var = msg;                    \
const usize SIZEOF_ ## var = sizeof(msg);

ERR_MESSAGE(ERR_UNEXPECTED, "Unexpected token: expected %s, but got %s")

#define MAKE_ERR_UNEXPECTED(EXPECTED, GOT, ...)            \
cstring expected = token_to_cstring(EXPECTED);             \
cstring got      = token_to_cstring(GOT);                  \
cstring str      = allocator_alloc(                        \
	p->allocator,                                          \
	SIZEOF_ERR_UNEXPECTED + strlen(expected) + strlen(got) \
);                                                         \
sprintf(str, ERR_UNEXPECTED, expected, got);               \
parser_push_err(p, (Error) {                                 \
	.lvl = ErrorLevelError,                                \
	.loc = __VA_ARGS__,                                    \
	.msg = string_from_cstring(str),                       \
});

AstPackage* _parse_package(Parser* p);
AstItem*    _parse_item(Parser* p);
AstType*    _parse_type(Parser* p);
AstType*    _parse_type_without_block(Parser* p);
AstType*    _parse_type_with_block(Parser* p);
AstExpr*    _parse_expr(Parser* p);
ChunkedList(ProcArguments, 4) _parse_arguments(Parser* p, TokenKey separator);

AstFile* parse(Parser* p) {
	AstFile* a = allocator_alloc(p->allocator, sizeof(AstFile));
	a->package = _parse_package(p);
	a->items = (ChunkedList(AstItemPtr, 8)) { 0 };
	AstItem* item;
	for(;(item = _parse_item(p)) != null;) {
		ChunkedList_push(AstItemPtr, 8)(&a->items, p->allocator, item);
	}
	return a;
}

AstPackage* _parse_package(Parser* p) {
	// '#package' ident
	Token package = p->lex->curr;
	if(package.tok != TkKwHashPackage) {
		p->flags |= PARSE_NOPACKAGE;
		return null;
	}
	LineColumn begin = package.begin;


	AstPackage* pkg = allocator_alloc(
		p->allocator,
		sizeof(AstPackage) + sizeof(Location)
	);
	pkg->loc = (Location*)(pkg + 1);




	Token pkg_name = lex_next(p->lex);
	*pkg->loc = (Location) { begin, pkg_name.end };
	if(pkg_name.tok != TkLitIdent) {
		MAKE_ERR_UNEXPECTED(TkLitIdent, pkg_name.tok, (Location) { pkg_name.begin, pkg_name.end });
		pkg->name = null;
		return pkg;
	}
	lex_next(p->lex);

	cstring cstr = string_to_cstring(p->allocator, pkg_name.str);
	pkg->name = cstr;
	return pkg;
}


// TODO(pgs): proc
// TODO(pgs): decl
AstItem* _parse_item(Parser* p) {
	Token fst = p->lex->curr;
	bool is_alias = false; // line 161
	switch(fst.tok) {
		// #import
		case TkKwHashImport: {
			lex_next(p->lex);


			usize alloc_size = sizeof(AstItem) + sizeof(AstImport) + sizeof(Location);
			AstItem* item = allocator_alloc(p->allocator, alloc_size);
			memset(item, 0, alloc_size);


			AstImport* import = (AstImport*) (item + 1);
			item->item.import = import;

			item->item_kind = ItemKindImport;
			// NOTE(pgs): redundant Locations
			item->loc         = (Location*)  (import + 1);
			import->loc       = item->loc;
			item->loc->begin = fst.begin;

			Token nxt = p->lex->curr;
			switch(nxt.tok) {
				case TkLitIdent:
					import->namespace = string_to_cstring(p->allocator, nxt.str);
					break;
				case TkLitString: // nice error

					parser_push_err(p, (Error) {
						.lvl = ErrorLevelError,
						.loc = (Location) { nxt.begin, nxt.end },
						.msg = string_from_cstring("Missing import name"),
					});
					parser_push_err(p, (Error) {
						.lvl = ErrorLevelNote,
						.hideInfo = true,
						.msg = string_from_cstring("usage: #import name \"path\""),
					});


					item->loc->end = nxt.end;
					return item;
				default: {
					MAKE_ERR_UNEXPECTED(TkLitIdent, nxt.tok, (Location) { nxt.begin, nxt.end });
					item->loc->end = nxt.end;
					return item;
				}
			}

			Token path = lex_next(p->lex);
			item->loc->end = path.end;

			if(path.tok != TkLitString) {
				MAKE_ERR_UNEXPECTED(TkLitString, path.tok, (Location) { path.begin, path.end });

				return item;
			}
			lex_next(p->lex);

			cstring path1 = string_to_cstring(p->allocator, path.str);
			import->path = path1;
			return item;
		}



		// alias & type
		case TkKwAlias:
			is_alias = true;
		case TkKwType: {
			lex_next(p->lex);

			AstItem* item = allocator_alloc(
				p->allocator,
				sizeof(AstItem) + sizeof(AstTypeDecl) + sizeof(Location)
			);
			AstTypeDecl* t_decl  = (AstTypeDecl*) (item + 1);
			t_decl->is_alias     = is_alias;
			item->item_kind      = ItemKindTypeDecl;
			item->item.type_decl  = t_decl;
			item->loc            = (Location*) (t_decl + 1);
			t_decl->loc          = item->loc;
			item->loc->begin     = fst.begin;


			// ident
			Token name = p->lex->curr;
			if(name.tok != TkLitIdent) {
				MAKE_ERR_UNEXPECTED(TkLitIdent, name.tok, (Location) { name.begin, name.end });
				item->loc->end = name.end;
				return item;
			}
			t_decl->new_name = string_to_cstring(p->allocator, name.str);
			lex_next(p->lex);

			// type
			AstType* t = _parse_type_without_block(p);
			if(t == null) {
				t = _parse_type_with_block(p);
				if(t == null) {
					// TODO(pgs): handle error
					printf("expected type\n");
					abort();
				}
				goto nosemicolon;
			}
			Token semic = p->lex->curr;
			if(semic.tok != TkSemiColon) {
				MAKE_ERR_UNEXPECTED(TkSemiColon, semic.tok, (Location) { semic.begin, semic.end });
				item->loc->end = semic.end;
				return item;
			}
			lex_next(p->lex);
			nosemicolon:

			t_decl->type = t;
			item->loc->end = t->loc->end;
			return item;
		}



		default: return null; 
	}
}


AstType* _parse_type(Parser* p) {
	AstType* t = _parse_type_without_block(p);
	if(t == null) {
		t = _parse_type_with_block(p);
	}
	return t;
}


// TODO(pgs): proc type
// TODO(pgs): nonull type
AstType* _parse_type_without_block(Parser* p) {
	Token fst = p->lex->curr;

	switch(fst.tok) {
		case TkLitIdent: {
			lex_next(p->lex);

			AstType* type = allocator_alloc(
				p->allocator,
				sizeof(AstType) + sizeof(Location)
			);
			type->type_kind      = TypeKindNamed;
			type->type.named    = string_to_cstring(p->allocator, fst.str);
			type->loc           = (Location*) (type + 1);
			*type->loc          = (Location ) { fst.begin, fst.end };
			return type;
		}
		case TkStar: {
			lex_next(p->lex);

			AstType* typ = _parse_type(p);
			if(typ == null) {
				// TODO(pgs): handle error
				return null;
			}

			AstType* type = allocator_alloc(
				p->allocator,
				sizeof(AstType) + sizeof(Location)
			);
			type->type_kind      = TypeKindPointer;
			type->type.ptr      = typ;
			type->loc           = (Location*) (type + 1);
			*type->loc          = (Location ) { fst.begin, typ->loc->end };
			return type;
		}

		case TkOpenBracket: {
			lex_next(p->lex);
			switch(p->lex->curr.tok) {
				case TkCloseBracket: {
					lex_next(p->lex);

					AstType* typ = _parse_type(p);
					if(typ == null) {
						// TODO(pgs): handle error
						return null;
					}

					AstType* type = allocator_alloc(
						p->allocator,
						sizeof(AstType) + sizeof(Location)
					);
					type->type_kind      = TypeKindSlice;
					type->type.slice    = typ;
					type->loc           = (Location*) (type + 1);
					*type->loc          = (Location ) { fst.begin, typ->loc->end };
					return type;
				}
				default: {
					AstExpr* expr = _parse_expr(p);
					if(expr == null) {
						// TODO(pgs): handle error
						return null;
					}
					if(p->lex->curr.tok != TkCloseBracket) {
						// TODO(pgs): handle error
						return null;
					}
					lex_next(p->lex);

					AstType* typ = _parse_type(p);
					if(typ == null) {
						// TODO(pgs): handle error
						return null;
					}

					AstType* type = allocator_alloc(
						p->allocator,
						sizeof(AstType) + sizeof(Location)
					);
					type->type_kind      = TypeKindArray;
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


AstType* _parse_type_with_block(Parser* p) {
	Token fst = p->lex->curr;
	switch(fst.tok) {
		case TkKwStruct: {
			lex_next(p->lex);

			usize alloc_size = sizeof(AstType) + sizeof(AstStruct) + sizeof(Location);
			AstType* type = allocator_alloc(p->allocator, alloc_size);
			memset(type, 0, alloc_size);

			AstStruct* strct   = (AstStruct*) (type + 1);
			type->type_kind     = TypeKindStruct;
			type->type._struct = strct;

			type->loc  = (Location*) (strct + 1);
			strct->loc = type->loc;

			type->loc->begin = fst.begin;



			Token tmp = p->lex->curr;
			if(tmp.tok != TkOpenBrace) {
				MAKE_ERR_UNEXPECTED(TkOpenBrace, tmp.tok, (Location) { tmp.begin, tmp.end });
				type->loc->end = tmp.end;
				return type;
			}
			lex_next(p->lex);



			strct->fields = _parse_arguments(p, TkComma);


			tmp = p->lex->curr;
			type->loc->end = tmp.end;
			if(tmp.tok != TkCloseBrace) {
				MAKE_ERR_UNEXPECTED(TkCloseBrace, tmp.tok, (Location) { tmp.begin, tmp.end });
				return type;
			}
			lex_next(p->lex);

			return type;
		}
		case TkKwUnion: {
			lex_next(p->lex);


			usize alloc_size = sizeof(AstType) + sizeof(AstUnion) + sizeof(Location);
			AstType* type = allocator_alloc(p->allocator, alloc_size);
			memset(type, 0, alloc_size);

			AstUnion* unin     = (AstUnion*) (type + 1);
			type->type_kind     = TypeKindUnion;
			type->type._union  = unin;

			type->loc  = (Location*) (unin + 1);
			unin->loc  = type->loc;

			type->loc->begin = fst.begin;




			Token tmp = p->lex->curr;
			if(tmp.tok != TkOpenBrace) {
				MAKE_ERR_UNEXPECTED(TkOpenBrace, tmp.tok, (Location) { tmp.begin, tmp.end });
				type->loc->end = tmp.end;
				return type;
			}
			lex_next(p->lex);


			ChunkedList(AstType, 4) types = {0};
			for(;;) {
				AstType* t = _parse_type(p);
				if(t == null) { break; }
				ChunkedList_push(AstType, 4)(&types, p->allocator, *t);
				if(p->lex->curr.tok != TkComma) { break; }
				lex_next(p->lex);
			}
			unin->types = types;

			tmp = p->lex->curr;
			type->loc->end = tmp.end;
			if(tmp.tok != TkCloseBrace) {
				MAKE_ERR_UNEXPECTED(TkCloseBrace, tmp.tok, (Location) { tmp.begin, tmp.end });
				return type;
			}
			lex_next(p->lex);

			return type;		
		}
		case TkKwEnum: {
			lex_next(p->lex);

			usize alloc_size = sizeof(AstType) + sizeof(AstEnum) + sizeof(Location);
			AstType* type = allocator_alloc(p->allocator, alloc_size);
			memset(type, 0, alloc_size);

			AstEnum* enm       = (AstEnum*) (type + 1);
			type->type_kind     = TypeKindEnum;
			type->type._enum   = enm;

			type->loc  = (Location*) (enm + 1);
			enm->loc = type->loc;

			type->loc->begin = fst.begin;

			Token tmp = p->lex->curr;
			if(tmp.tok != TkOpenBrace) {
				MAKE_ERR_UNEXPECTED(TkOpenBrace, tmp.tok, (Location) { tmp.begin, tmp.end });
				type->loc->end = tmp.end;
				return type;
			}
			lex_next(p->lex);





			ChunkedList(EnumKey, 8) fields = {0};
			for(;;) {
				EnumKey key = {0};
				Token tmp = p->lex->curr;
				if(tmp.tok != TkLitIdent) { break; }
				key.field = string_to_cstring(p->allocator, tmp.str);
				lex_next(p->lex);


				tmp = p->lex->curr;
				if(tmp.tok != TkEq) { goto noexpr; }
				lex_next(p->lex);

				AstExpr* expr = _parse_expr(p);
				if(expr == null) {
					// TODO(pgs): handle error
					printf("expected expr\n");
					abort();
				}
				key.expr = expr;

				noexpr:
				if(p->lex->curr.tok != TkComma) { break; }
				lex_next(p->lex);
				ChunkedList_push(EnumKey, 8)(&fields, p->allocator, key);
			}
			enm->fields = fields;




			tmp = p->lex->curr;
			type->loc->end = tmp.end;
			if(tmp.tok != TkCloseBrace) {
				MAKE_ERR_UNEXPECTED(TkCloseBrace, tmp.tok, (Location) { tmp.begin, tmp.end });
				return type;
			}
			lex_next(p->lex);

			return type;
		}
		default: return null;
	}
}

AstExpr* _parse_expr(Parser* p) {
	return null;
}

ChunkedList(ProcArguments, 4) _parse_arguments(Parser* p, TokenKey separator) {
	// return (ChunkedList(ProcArguments, 4)) {0};
	ChunkedList(ProcArguments, 4) args = {0};
	for(;;) {
		ProcArguments arg = {0};

		Token tmp = p->lex->curr;
		if(tmp.tok != TkLitIdent) { break; }
		arg.name = string_to_cstring(p->allocator, tmp.str);
		lex_next(p->lex);


		if(p->lex->curr.tok != TkColon) {
			// TODO(pgs): handle error
			printf("expected colon\n");
			abort();
		}
		lex_next(p->lex);

		AstType* t = _parse_type(p);
		if(t == null) {
			// TODO(pgs): handle error
			printf("expected type\n");
			abort();
		}
		arg.type = t;

		ChunkedList_push(ProcArguments, 4)(&args, p->allocator, arg);

		if(p->lex->curr.tok != separator) { break; }
		lex_next(p->lex);
	}
	return args;
}
