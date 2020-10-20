
#include <stdarg.h>

#include "lex.h"
#include "parse.h"
#include "common.h"










// ===========================
// = Sym
//

Sym* sym_find(Buf(Sym) symbols, cstr name) {
	for(isize i = buf_len(symbols)-1; i >= 0; i--) {
		if(symbols[i].name == name) {
			return &symbols[i];
		}
	}
	return null;
}

void sym_scope_remove(Buf(Sym)* symbols, i32 scope) {
	Buf(Sym) s = *symbols;
	for(isize i = buf_len(s)-1; i >= 0; i--) {
		if(s[i].scope < scope) {
			break;
		}
		buf_remove(*symbols, i);
	}
}

//










// ===========================
// = report errors

void report(Ctx* c, Pos p, cstr fmt, ...) {
	Position pos = sources_position(c->srcs, p);
	fprintf(c->report, "%s:%d:%d: ", pos.path, pos.line, pos.column);

	va_list vargs;
	va_start(vargs, fmt);
	vfprintf(c->report, fmt, vargs);
	va_end(vargs);

	fputc('\n', c->report);
}


#define err_expecting(LexGet, Msg) report(ctx, LexGet pos, "Expecting " Msg ", but got '%s'", TOKEN_TO_STRING[LexGet tok])

//










// ===========================
// = Compile

void parse_file(Sources* srcs, isize file_id, Buf(char)* out, FILE* rep) {
	Lexer l = lex(&srcs->list[file_id]);
	Ctx* ctx = &(Ctx) { srcs, rep, *out };

	advance(&l);
	if(l.tok != TK_package) {
		err_expecting(l., "package declaration");
		return;
	}
	advance(&l);
	if(l.tok != TK_IDENT) {
		err_expecting(l., "package name(identifier)");
		return;
	}
	ctx->pkgname = l.data.dident;
	advance(&l);
	if(l.tok != ';') {
		err_expecting(l., "semicolon");;
		return;
	}
	advance(&l);

	buf_write(ctx->out, "// package: ");
	buf_write(ctx->out, ctx->pkgname);
	buf_push(ctx->out, '\n');


	for(;l.tok;) {
		parse_item(&l, ctx);
	}

	*out = ctx->out;
}

void parse_item(Lexer* l, Ctx* ctx) {
	switch(l->tok) {
	case TK_proc: {
		advance(l);



		if(l->tok != TK_IDENT) {
			err_expecting(l->, "procedure name(identifier)");
			return;
		}
		advance(l);

		if(l->tok != '(') {
			err_expecting(l->, "'('");
			return;
		}
		advance(l);


		for(bool first = true;l->tok != ')';) {
			if(!first) {
				if(l->tok != ',') {
					err_expecting(l->, ",");
					return;
				}
				advance(l);
			}
			first = false;

			if(l->tok != TK_IDENT) {
				err_expecting(l->, "argument name(identifier)");
				return;
			}
			advance(l);

			if(l->tok != ':') {
				err_expecting(l->, "':'");
				return;
			}
			advance(l);

			Type* t = parse_type(l, ctx);
			if(t == null) {
				// TODO(pgs): handle
			}
		}
		advance(l);


		// if ret == null, return type is void
		Type* ret = parse_type(l, ctx);

		if(l->tok == ';') {
			advance(l);
			// output declaration,
		} else if(l->tok == '{') {
			parse_block(l, ctx);
		} else {
			err_expecting(l->, "either an ';', or a block");
		}


		break;
	}
	case TK_var:
	case TK_const:
	case TK_extern:
	case TK_type:
	case TK_alias:
		break;
	default:
		err_expecting(l->, "'proc', 'var', 'const', 'extern', 'type', or 'alias'");
		l->tok = 0;
	}
}


Type* parse_type(Lexer* l, Ctx* ctx) {
	switch(l->tok) {
	case '*': {
		advance(l);
		Type* ty = parse_type(l, ctx);
		Type* result = heap_alloc(sizeof(*result));
		result->kind = TY_POINTER;
		result->pointer = ty;
		return result;
	}
	case TK_IDENT: {
		advance(l);
		Type* result = heap_alloc(sizeof(*result));
		result->kind = TY_UINT;
		return result;
	}
	default:
		return null;
	}
}


void parse_block(Lexer* l, Ctx* ctx) {
	advance(l);


	// parse stmts....

	if(l->tok != '}') {
		err_expecting(l->, "'}'");
		return;
	}
	advance(l);
}




#undef err_expecting
